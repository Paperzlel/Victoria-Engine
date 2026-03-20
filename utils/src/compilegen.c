#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#	include <direct.h>
#	define fullpath(m_relative) _fullpath(NULL, m_relative, 1024)
#	define get_cwd() _getcwd(NULL, 0)
#	define string_dup(m_string) _strdup(m_string)
#elif defined(__linux__) || defined(__gnu_linux__)
#	include <unistd.h>
#	define fullpath(m_relative) realpath(m_relative, NULL)
#	define get_cwd() getcwd(NULL, 0)
#	define string_dup(m_string) strdup(m_string)
#else
#	error "Realpath or alternatives must be specified!"
#endif

// Generates a compile_flags.txt file from an input file.
// All arguments are set after --args is specified beyond which no actual
// commands should be placed.

int main(int argc, char **argv) {
	// First argument is the calling directory
	if (argc <= 1) {
		printf("Error: no inputs given.\n");
		return 1;
	}

	char *path = NULL;
	char **args_begin = NULL;
	int output_arg_count = 0;
	for (int i = 1; i < argc; i++) {
		char *arg = argv[i];

		if (strcmp(arg, "--args-begin") == 0) {
			if (args_begin) {
				printf("Error: \"--args-begin\" was already specified.\n");
				return 3;
			}

			args_begin = (char **)(argv + i + 1);
		}

		if (strcmp(arg, "--args") == 0) {
			if (args_begin) {
				printf("Error: \"--args-begin\" was already specified.\n");
				return 3;
			}

			args_begin = (char **)(argv + i + 1);
		}

		if (strcmp(arg, "--args-end") == 0) {
			if (args_begin == NULL) {
				printf("Error: \"--args-end\" was specified but "
					   "\"--args-begin\" was not.");
				return 1;
			}

			// Don't bother with a counter like a moron
			output_arg_count = ((char **)(argv + i) - args_begin);
		}

		if (strcmp(arg, "--output-file") == 0) {
			if (args_begin) {
				continue;
			}

			if (path) {
				printf("Error: path already specified.\n");
				return 3;
			}

			path = string_dup(argv[i + 1]);
		}

		if (strcmp(arg, "-o") == 0) {
			if (args_begin) {
				continue;
			}

			if (path) {
				printf("Error: path already specified.\n");
				return 3;
			}

			path = string_dup(argv[i + 1]);
		}
	}

	// If --args-end is not specified
	if (!output_arg_count) {
		output_arg_count = ((char **)(argv + argc)) - args_begin;
	}

	if (!path) {
		printf("Error: \"--output-file\" not specified.\n");
		return 1;
	}

	// Remove file since it MAY not exist.
#ifdef _MSC_VER
	char *removal = strrchr(path, '\\');
#else
	char *removal = strrchr(path, '/');
#endif

	char *fp = NULL;
	if (removal) {
		int to_remove = removal - path;
		removal = string_dup(removal);
		path[to_remove] = 0;
		fp = fullpath(path);
	} else {
		int len = strlen(path);
		path = realloc(path, len + 2);
		memcpy(path + 1, path, len);
#if _MSC_VER
		path[0] = '\\';
#else
		path[0] = '/';
#endif
		path[len + 1] = 0;
		fp = get_cwd();
	}

	if (!fp) {
		switch (errno) {
			case EACCES:
				printf("Error: Invalid permissions.\n");
				break;
			case EINVAL:
				printf("Error: Path is NULL.\n");
				break;
			case EIO:
				printf("Error: An I/O error occured.\n");
				break;
			case ELOOP:
				printf("Error: Too many symbolic links.\n");
				break;
			case ENAMETOOLONG:
				printf("Error: Part of the pathname was too long.");
				break;
			case ENOENT:
				printf("Error: The specified file does not exist.\n");
				break;
			case ENOMEM:
				printf("Error: System was out of memory.\n");
				break;
			case ENOTDIR:
				printf("Error: A component of the path is not a directory.\n");
				break;
		}

		printf("Error: Failed to obtain full filepath.\n");
		return 2;
	}

	// Append removal string back to full path
	if (!removal) {
		removal = path;
	}

	fp = realloc(fp, strlen(fp) + strlen(removal) + 1);
	strcat(fp, removal);

	FILE *fptr = fopen(fp, "w");
	if (!fptr) {
		printf("Error: failed to open or create file.\n");
		return 2;
	}

    
	for (int i = 0; i < output_arg_count; i++) {
        char *flag = args_begin[i];

        if (!flag) {
            continue;
        }

        // Check for duplicate arguments and set NULL if needed.
        for (int j = i + 1; j < output_arg_count; j++) {
            if (!args_begin[j]) {
                continue;
            }

            if (strcmp(args_begin[j], flag) == 0) {
                args_begin[j] = NULL;
            }
        }
        
        if (!flag) {
            continue;
        }

		fwrite(flag, sizeof(char), strlen(flag), fptr);
		fwrite("\n", sizeof(char), 1, fptr);
	}

	fclose(fptr);

	free(removal);
	free(fp);

	// All successful, can now return
	return 0;
}