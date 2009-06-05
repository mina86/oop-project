/**
 * \file
 * Random Set Generator.
 * $Id: gen-set.c,v 1.3 2006/11/26 19:04:32 mina86 Exp $
 * Released to Public Domain
 * By Michal Nazarewicz (mina86/AT/mina86.com)
 *
 * This program simply prints a lot of random numbers to standard
 * output.  If an argument is given and the first character of this
 * argument is \c b a list of big integers, ie. up to 50 digits, will
 * be printed.  There is no guarantee that all the numbers will be
 * unique.
 *
 * This program is used for testing the \c is_expr and \c bis_expr
 * programs.  See files set_expr.cpp as well as test-set.sh.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>


int main(int argc, char **argv) {
	int count, bigint = argc>1 && argv[1][0]=='b';

	srand(time(0) ^ getpid());

	for (count = 100 + rand() % 1000; --count; ) {
		if (!bigint) {
			if (rand() & 1) putchar('-');
			printf("%d\n", rand());
		} else {
			char buffer[64], *p = buffer;
			int digits = rand() % 50;

			if (rand() & 1) *p++ = '-';
			*p++ = '1' + (rand() % 9);
			while (digits--) {
				*p++ = '0' | (rand() % 10);
			}

			*p = 0;
			puts(buffer);
		}
	}

	return 0;
}
