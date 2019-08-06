/*
 * UNG's Not GNU
 *
 * Copyright (c) 2011-2019, Jakob Kaivo <jkk@ung.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#define _POSIX_C_SOURCE 2
#include <ctype.h>
#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int strings(const char *path, size_t number, char format)
{
	FILE *f = stdin;
	if (path) {
		f = fopen(path, "rb");
	}

	if (f == NULL) {
		fprintf(stderr, "strings: %s: %s\n", path, strerror(errno));
		return 1;
	}

	size_t count = 0;
	char string[number];

	int c;
	for (size_t offset = 0; (c = fgetc(f)) != EOF; offset++) {
		if (!isprint(c)) {
			if (count >= number) {
				putchar('\n');
			}
			count = 0;
		} else if (count > number) {
			putchar(c);
		} else if (count < number) {
			string[count++] = c;
		}

		if (count == number) {
			if (format) {
				char fmt[] = { '%', 'z', format, ' ', '\0' };
				printf(fmt, offset - count + 1);
			}
			fwrite(string, 1, number, stdout);
			count++;
		}
	}

	if (f != stdin) {
		fclose(f);
	}

	return 0;
}

static void fixobsolete(int argc, char *argv[])
{
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] != '-') {
			return;
		}

		if (!strcmp(argv[i], "--")) {
			return;
		}

		if (!strcmp(argv[i], "-")) {
			fprintf(stderr, "strings: '-' is obsolete; use '-a'\n");
			argv[i] = "-a";
		}

		if (isdigit(argv[i][1])) {
			fprintf(stderr, "strings: '-#' is obsolete; use '-n #'\n");
			char *opt = malloc(strlen(argv[i]) + 2);
			if (opt == NULL) {
				perror("strings");
				exit(1);
			}
			sprintf(opt, "-n %s", argv[i] + 1);
			argv[i] = opt;
		}
	}
}

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "");

	int c;
	size_t number = 4;
	char format = 0;
	char *end;

	fixobsolete(argc, argv);
	while ((c = getopt(argc, argv, "an:t:")) != -1) {
		switch (c) {
		case 'a':
			/* Ignored. Always scan the whole file. */
			break;

		case 'n':
			number = strtol(optarg, &end, 10);
			if (*end != '\0') {
				fprintf(stderr, "strings: invalid number %s\n", optarg);
				return 1;
			}
			break;

		case 't':
			if (optarg[1] != '\0') {
				fprintf(stderr, "strings: invalid format %s\n", optarg);
				return 1;
			}

			switch (optarg[0]) {
			case 'd':
			case 'o':
			case 'x':
				/* OK */
				break;

			default:
				fprintf(stderr, "strings: invalid format %s\n", optarg);
				return 1;
			}

			format = optarg[0];
			break;

		default:
			return 1;
		}
	}

	int ret = 0;

	do {
		ret |= strings(argv[optind], number, format);
	} while (++optind < argc);

	return 0;
}
