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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

static int strings(const char *path, int number, char format)
{
	FILE *f = stdin;
	if (path && strcmp(path, "-")) {
		f = fopen(path, "rb");
	}

	if (f == NULL) {
		return 1;
	}

	char buf[BUFSIZ];
	char tbuf[BUFSIZ];
	int nread;
	int offset = 0;
	int i;
	int count = 0;

	while (!feof(f)) {
		nread = fread(buf, sizeof(char), BUFSIZ, f);
		for (i = 0; i < nread; i++) {
			if (buf[i] == '\0' || buf[i] == '\n') {
				if (count >= number) {
					switch (format) {
					case 'd':
						printf("%d ", offset - count);
						break;
					case 'o':
						printf("%o ", offset - count);
						break;
					case 'x':
						printf("%x ", offset - count);
						break;
					default:
						break;
					}
					fwrite(tbuf, sizeof(char), count,
					       stdout);
					putchar('\n');
				}
				count = 0;
			} else if (isprint(buf[i])) {
				tbuf[count] = buf[i];
				count++;
			} else {
				count = 0;
			}
			offset++;
		}
	}

	if (f != stdin) {
		fclose(f);
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int c;
	int number = 4;
	char format = 0;
	char *end;

	while ((c = getopt(argc, argv, "an:t:")) != -1) {
		switch (c) {
		case 'a':
			/* Ignored. Always scan the whole file. */
			break;

		case 'n':
			number = strtol(optarg, &end, 10);
			if (end != NULL && strlen(end) > 0)
				return 1;
			break;

		case 't':
			if (strlen(optarg) != 1) {
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
	} while (optind++ < argc);

	return 0;
}
