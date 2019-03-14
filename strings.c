/*
 * UNG's Not GNU
 * 
 * Copyright (c) 2011, Jakob Kaivo <jakob@kaivo.net>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>

const char *strings_desc = "find printable strings in files";
const char *strings_inv  = "strings [-a] [-t format] -[n number] [file...]";

static int strings (FILE *f, int number, char format)
{
  char buf[BUFSIZ];
  char tbuf[BUFSIZ];
  int nread;
  int offset = 0;
  int i;
  int count = 0;

  while (!feof (f)) {
    nread = fread (buf, sizeof(char), BUFSIZ, f);
    for (i = 0; i < nread; i++) {
      if (buf[i] == '\0' || buf[i] == '\n') {
        if (count >= number) {
          switch (format) {
            case 'd':
              printf ("%d ", offset - count);
              break;
            case 'o':
              printf ("%o ", offset - count);
              break;
            case 'x':
              printf ("%x ", offset - count);
              break;
            default:
              break;
          }
          fwrite (tbuf, sizeof(char), count, stdout);
          putchar ('\n');
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
  return 0;
}

int
main(int argc, char **argv)
{
  int c;
  int number = 4;
  char format = 0;
  char *end;

  while ((c = getopt (argc, argv, ":an:t:")) != -1) {
    switch (c) {
      case 'a':
        // Always scan the entire file
        break;
      case 'n':
        number = strtol (optarg, &end, 10);
        if (end != NULL && strlen (end) > 0)
          return 1;
        break;
      case 't':
        if (strlen(optarg) != 1)
          return 1;
        format = optarg[0];
        break;
      default:
        return 1;
    }
  }

  if ((format != 0 && format != 'd' && format != 'o' && format != 'x')
      || number < 1)
    return 1;

  if (optind >= argc) {
    strings (stdin, number, format);
  }

  while (optind < argc) {
    FILE *f = fopen (argv[optind], "r");
    strings (f, number, format);
    fclose (f);
    optind++;
  }

  return 0;
}
