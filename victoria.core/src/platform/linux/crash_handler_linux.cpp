#include "crash_handler_linux.h"

#include "core/string/print_string.h"
#include "core/version.h"

#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

static constexpr int BACKTRACE_COUNT = 128;

static void __execute(const char *p_file, const Vector<String> &p_args, String *r_output) {
	// Run w/ pipe
	if (r_output) {
		String cmd = "\"" + String(p_file) + "\"";
		for (const String &arg : p_args) {
			cmd += " \"" + arg + "\"";
		}
		cmd += " 2>/dev/null";

		fflush(stdout);
		FILE *fptr = popen(cmd.get_data(), "r");
		if (fptr) {
			char ret[65536];
			int count = fread(ret, 1, 65536, fptr);
			if (count) {
				r_output->resize(count + 1);
				Memory::vcopy_memory(r_output->ptrw(), ret, count);
				(*r_output)[count] = 0;
			}

			pclose(fptr);
		}

		return;
	}

	int pipes[2];
	if (pipe(pipes) == -1) {
		// TODO:
	}

	pid_t proc_id = fork();

	// Parent process
	if (proc_id == 0) {
		dup2(pipes[1], STDOUT_FILENO);
		close(pipes[0]);
		close(pipes[1]);

		// Needs char *const from args
		Vector<char *> args;
		for (const String &arg : p_args) {
			args.push_back((char *)arg.get_data());
		}
		args.push_back(nullptr);

		execvp(p_file, &args[0]);
		fprintf(stderr, "Failed to run process %s.\n", p_file);
		raise(SIGKILL);
	}

	close(pipes[1]);
	if (r_output) {
		char buf[65536];
		int count = read(pipes[0], buf, sizeof(buf));
		r_output->resize(count + 1);
		Memory::vcopy_memory(r_output->ptrw(), buf, count);
		r_output[count] = 0;
	}

	close(pipes[0]);
}

static void __run_addr2line(void **p_symbols, char **p_symbolstrings, int p_backtrace_count) {
	Vector<String> args;
	for (int i = 1; i < p_backtrace_count; i++) {
		// Still need module info
		String module_path;
		uint64_t load_offset = 0;
		Dl_info info;
		if (dladdr(p_symbols[i], &info)) {
			module_path = info.dli_fname;
			load_offset = (uint64_t)info.dli_fbase;
		}

		// Reset arglist
		args.clear();
		args.push_back("-e");
		args.push_back(module_path);
		args.push_back("-f");
		args.push_back("-p");
		args.push_back("-C");

		// Calculate offset
		char ofs[128];
		snprintf(ofs, 128, "%p", (void *)((uintptr_t)p_symbols[i] - load_offset));
		args.push_back(ofs);

		String output;
		__execute("addr2line", args, &output);
		if (output.is_empty()) {
			print_line(vformat("[%d] <could not find symbols>", i));
			continue;
		}

		// Remove final newline from the output
		String final_output = output.substr(0, output.length() - 1);
		String final_module_name = module_path;
		if (!module_path.is_empty()) {
			final_module_name = module_path.get_file();
		}

		if (final_output.ends_with("??:0")) {
			final_output = p_symbolstrings[i];
			if (info.dli_sname && info.dli_sname[0] == '_') {
				int status = 0;
				char *newname = abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);
				if (status == 0 && newname) {
					final_output = newname;
				}

				if (newname) {
					free(newname);
				}
			}
		}

		print_line(vformat("[%d] %s+%x - %s",
						   (uint64_t)i,
						   final_module_name.get_data(),
						   (uint64_t)p_symbols[i] - load_offset,
						   final_output.get_data()));
	}
}

static void _crash_handler(int p_signal) {
	// Reset handlers so a crash here doesn't loop.
	signal(SIGSEGV, SIG_DFL);
	signal(SIGILL, SIG_DFL);
	signal(SIGABRT, SIG_DFL);

	void *symbols[BACKTRACE_COUNT] = {};
	int actual_bt_count = backtrace(symbols, BACKTRACE_COUNT);
	if (actual_bt_count > BACKTRACE_COUNT) {
		print_line("Recieved %d backtraces when a maximum of %d is allowed. Total backtrace will not be shown.\n",
				   actual_bt_count,
				   BACKTRACE_COUNT);
	}

	print_line("================================ CRASH OCCURRED ================================");
	print_line("Program crashed with signal:", p_signal);
	print_line("Engine version:", String(VICTORIA_FULL_VERSION_STRING));
	print_line("========================== BEGINNING OF C++ BACKTRACE ==========================");

	char **strings = backtrace_symbols(symbols, actual_bt_count);

	if (strings) {
		// Look for addr2line existence
		Vector<String> args;
		args.push_back("--version");
		String output;
		__execute("addr2line", args, &output);
		bool has_addr2line = output.length() > 0;
		if (has_addr2line) {
			__run_addr2line(symbols, strings, actual_bt_count);
		} else {
			for (int i = 1; i < actual_bt_count; i++) {
				if (i >= BACKTRACE_COUNT) {
					break;
				}

				Dl_info info;
				String module_name;
				uint64_t module_offset = 0;
				String output_name = String(strings[i]);
				if (dladdr(symbols[i], &info)) {
					module_name = String(info.dli_fname).get_file();
					module_offset = (uint64_t)info.dli_fbase;

					// Is a C++ name
					if (info.dli_sname && info.dli_sname[0] == '_') {
						int ret = 0;
						char *newname = abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &ret);
						if (ret == 0 && newname) {
							output_name = newname;
						}

						if (newname) {
							free(newname);
						}
					}
				} else {
					module_name = "<unknown>";
				}
				// Found address, print
				print_line(vformat("[%d] %s+%x - %s",
								   (uint64_t)i,
								   module_name.get_data(),
								   (uint64_t)symbols[i] - module_offset,
								   output_name.get_data()));
			}
		}
	}

	print_line("============================= END OF C++ BACKTRACE =============================");

	abort();
}

void CrashHandlerLinux::initialize() {
	signal(SIGSEGV, _crash_handler);
	signal(SIGILL, _crash_handler);
	signal(SIGABRT, _crash_handler);
}