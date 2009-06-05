/**
 * \file
 * BigInt RPN Calculator test files generator.
 * $Id: make-in.c,v 1.4 2006/11/26 19:04:41 mina86 Exp $
 * Released to Public Domain
 * By Michal Nazarewicz (mina86/AT/mina86.com)
 *
 * This program generates a list of arithmetic expressions involving
 * addition, substraction and multpilication of big integers.  The
 * expressions are printed one per line in the form: <code>number
 * operator number</code>.
 *
 * If argument is given it should be a number representing how many
 * expressions program should generate.  The default is 100000
 * (ie. one hundreds thousands).
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


int main(int argc, char **argv) {
	char buf[64];
	int num = argc==1 ? 100000 : strtol(argv[1], 0, 0);
	if (num<1) num = 100;

	srand(time(0));

	do {
		char *p = buf;
		int len;

		if (rand() & 1) {
			*p++ = '-';
		}
		*p++ = '1' + (rand() % 9);
		for (len = rand() % 20; len; --len) {
			*p++ = '0' | (rand() % 10);
		}
		*p++ = ' ';

		if (argc>2 && argv[2]) {
			*p++ = *argv[2];
		} else {
			switch (rand() % 3) {
			case 0: *p++ = '+'; break;
			case 1: *p++ = '*'; break;
			case 2: *p++ = '-'; break;
			}
		}
		*p++ = ' ';

		*p++ = '1' + (rand() % 9);
		for (len = rand() % 20; len; --len) {
			*p++ = '0' | (rand() % 10);
		}
		*p++ = '\n';

		*p = 0;
		fputs(buf, stdout);
	} while (--num);

	return 0;
}
