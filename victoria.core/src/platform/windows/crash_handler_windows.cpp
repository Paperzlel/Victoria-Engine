#include "platform/windows/crash_handler_windows.h"

#include "core/string/print_string.h"
#include "core/version.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
// break.
#include <dbghelp.h>
#include <psapi.h>
#include <shlwapi.h>
#include <signal.h>

// based on https://stackoverflow.com/questions/6205981/windows-c-stack-trace-from-a-running-app

static constexpr int BACKTRACE_COUNT = 256;

struct ModuleData {
	String image_name;
	String module_name;
	void *base_address = nullptr;
	HANDLE process;
	DWORD size;
};

static ModuleData _get_module_data(HANDLE h, HMODULE module) {
	ModuleData ret;
	MODULEINFO mi;
	char tmp[1024];

	GetModuleInformation(h, module, &mi, sizeof(mi));
	ret.base_address = mi.lpBaseOfDll;
	ret.size = mi.SizeOfImage;

	GetModuleFileNameEx(h, module, tmp, sizeof(tmp));
	ret.image_name = tmp;
	GetModuleBaseName(h, module, tmp, sizeof(tmp));
	ret.module_name = tmp;
	SymLoadModule64(h,
					nullptr,
					ret.image_name.get_data(),
					ret.module_name.get_data(),
					(DWORD64)ret.base_address,
					ret.size);
	ret.process = h;
	return ret;
}

static void _handle_crash(int p_signal) {
	CrashHandlerException(nullptr);
}

DWORD CrashHandlerException(EXCEPTION_POINTERS *ep) {
	if (!ep) {
		signal(SIGSEGV, SIG_DFL);
		signal(SIGILL, SIG_DFL);
		signal(SIGABRT, SIG_DFL);
	}

	print_error("================================ CRASH OCCURRED ================================");
	print_error(vformat("Program crashed: %s", __FUNCTION__));
	print_error(vformat("Engine version: %s", VICTORIA_FULL_VERSION_STRING));
	MODULEINFO mi;
	HANDLE process = GetCurrentProcess();
	HANDLE thread = GetCurrentThread();
	GetModuleInformation(process, GetModuleHandle(nullptr), &mi, sizeof(mi));

	Vector<HMODULE> modules{nullptr};
	Vector<ModuleData> md;
	DWORD module_count_needed = 0;

	if (!SymInitialize(process, nullptr, false)) {
		print_error("Unable to initialize the symbol tracing. Debug symbols are unavailable.");
		return EXCEPTION_CONTINUE_SEARCH;
	}

	SymSetOptions(SymGetOptions() | SYMOPT_UNDNAME | SYMOPT_LOAD_LINES | SYMOPT_EXACT_SYMBOLS);
	EnumProcessModules(process, modules.ptrw(), modules.size() * sizeof(HMODULE), &module_count_needed);
	modules.resize(module_count_needed / sizeof(HMODULE));
	EnumProcessModules(process, modules.ptrw(), modules.size() * sizeof(HMODULE), &module_count_needed);
	for (int i = 0; i < modules.size(); i++) {
		ModuleData m = _get_module_data(process, modules[i]);
		md.append(m);
	}

	CONTEXT *ctx = ep->ContextRecord;
	STACKFRAME64 frame;
	frame.AddrPC.Mode = AddrModeFlat;
	frame.AddrStack.Mode = AddrModeFlat;
	frame.AddrFrame.Mode = AddrModeFlat;

#if defined(_M_X64)
	frame.AddrPC.Offset = ctx->Rip;
	frame.AddrStack.Offset = ctx->Rsp;
	frame.AddrFrame.Offset = ctx->Rbp;
#else
#	error "Unsupported platform!"
#endif

	IMAGEHLP_LINE64 line = {};
	line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
	IMAGE_NT_HEADERS *h = ImageNtHeader(md[0].base_address);
	DWORD image_type = h->FileHeader.Machine;
	DWORD offset_from_symbol;

	print_error("========================== BEGINNING OF C++ BACKTRACE ==========================");

	int n = 0;
	do {
		if (frame.AddrPC.Offset != 0) {
			IMAGEHLP_SYMBOL64 *sym = (IMAGEHLP_SYMBOL64 *)::operator new(sizeof(IMAGEHLP_SYMBOL64) + 1024);
			memset(sym, 0, sizeof(*sym));
			sym->SizeOfStruct = sizeof(*sym);
			sym->MaxNameLength = 1024;
			DWORD64 displacement;
			SymGetSymFromAddr64(process, frame.AddrPC.Offset, &displacement, sym);

			String fname;
			if (*sym->Name == '\0') {
				fname = "<cannot map PC to function name>";
			} else {
				fname.resize(1024);
				DWORD nsize = UnDecorateSymbolName(sym->Name, &fname[0], 1024, UNDNAME_COMPLETE);
				fname.resize(nsize + 1);
				fname[nsize] = 0;
			}

			IMAGEHLP_MODULE64 mod_info;
			memset(&mod_info, 0, sizeof(IMAGEHLP_MODULE64));
			mod_info.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
			uint64_t ofs = (uint64_t)(md[0].base_address);
			String mod_name = "main";
			if (SymGetModuleInfo64(process, frame.AddrPC.Offset, &mod_info)) {
				ofs = mod_info.BaseOfImage;
				if (ofs != (uint64_t)(md[0].base_address)) {
					mod_name = String((const char *)mod_info.ImageName).get_file();
				} else if (mod_info.ModuleName[0] != 0) {
					mod_name = String((const char *)mod_info.ModuleName);
				} else {
					mod_name = "<unknown>";
				}
			}

			if (SymGetLineFromAddr64(process, frame.AddrPC.Offset, &offset_from_symbol, &line)) {
				print_error(vformat("[%d] %s+%x - %s (%s:%d)",
									n,
									mod_name.get_data(),
									(uint64_t)frame.AddrPC.Offset - ofs,
									fname.get_data(),
									line.FileName,
									line.LineNumber));
			} else if (!fname.is_empty()) {
				print_error(vformat("[%d] %s+%x - %s",
									n,
									mod_name.get_data(),
									(uint64_t)frame.AddrPC.Offset - ofs,
									fname.get_data()));
			} else {
				print_error(vformat("[%d] %s+%x - ???", n, mod_name.get_data(), (uint64_t)frame.AddrPC.Offset - ofs));
			}
		}

		n++;
		if (!StackWalk64(image_type,
						 process,
						 thread,
						 &frame,
						 ctx,
						 nullptr,
						 SymFunctionTableAccess64,
						 SymGetModuleBase64,
						 nullptr)) {
			break;
		}
	} while (frame.AddrReturn.Offset != 0 && n < BACKTRACE_COUNT);

	// for (int i = 0; i < count; i++) {
	// 	IMAGEHLP_LINE64 line_info{0};
	// 	line_info.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
	// 	DWORD line_pos = 0;
	// 	bool line_exists = SymGetLineFromAddr64(process, (DWORD64)backtrace_symbols[i], &line_pos, &line_info);

	// 	char buf[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
	// 	PSYMBOL_INFO sym = (PSYMBOL_INFO)buf;
	// 	sym->SizeOfStruct = sizeof(SYMBOL_INFO);
	// 	sym->MaxNameLen = MAX_SYM_NAME;

	// 	String fname;
	// 	DWORD64 displacement = 0;
	// 	if (line_exists && SymFromAddr(process, (DWORD64)backtrace_symbols[i], &displacement, sym)) {
	// 		fname = sym->Name;
	// 	}

	// 	String module_name;
	// 	for (int j = 0; j < md.size(); j++) {
	// 		if ((DWORD64)md[j].base_address == sym->ModBase) {
	// 			module_name = md[j].module_name;
	// 			break;
	// 		}
	// 	}

	// 	print_error(vformat("[%d] %s+%x - %s", (uint64_t)i, module_name.get_data(), sym->Address, line_info.FileName));
	// }

	SymCleanup(process);

	// Return to OS
	return EXCEPTION_CONTINUE_SEARCH;
}

void CrashHandlerWindows::initialize() {
	signal(SIGSEGV, _handle_crash);
	signal(SIGILL, _handle_crash);
	signal(SIGABRT, _handle_crash);
}
