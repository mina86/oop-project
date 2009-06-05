/**
 * \file
 * Set Expression Main file.
 * $Id: set_expr.cpp,v 1.10 2006/11/26 19:05:30 mina86 Exp $
 * Copyright 2006 by Michal Nazarewicz (mina86/AT/mina86.com)
 * Licensed under the Academic Free License version 2.1
 */

#include <cstring>
#include <iostream>
#include <fstream>
#include <istream>


#include "set.hpp"

using mina86::Set;
using namespace std;


/******************** Which type ********************/
#ifdef TYPE
# undef TYPE
#endif
#ifdef BIGINT_SET
# define TYPE BigInt
# include "bigint.hpp"
using mina86::BigInt;
#else
# define TYPE long
# include <cstdlib>
#endif


/******************** Commands ********************/
static int func_add(Set<TYPE> &a, Set<TYPE> &b);
static int func_mul(Set<TYPE> &a, Set<TYPE> &b);
static int func_sub(Set<TYPE> &a, Set<TYPE> &b);
static int func_dif(Set<TYPE> &a, Set<TYPE> &b);

static int func_lt (Set<TYPE> &a, Set<TYPE> &b);
static int func_le (Set<TYPE> &a, Set<TYPE> &b);
static int func_eq (Set<TYPE> &a, Set<TYPE> &b);
static int func_ne (Set<TYPE> &a, Set<TYPE> &b);
static int func_ge (Set<TYPE> &a, Set<TYPE> &b);
static int func_gt (Set<TYPE> &a, Set<TYPE> &b);

static int func_num(Set<TYPE> &a, Set<TYPE> &b);
static int func_emp(Set<TYPE> &a, Set<TYPE> &b);
static int func_prn(Set<TYPE> &a, Set<TYPE> &b);


static struct {
	const char *command;
	int (*handler)(Set<TYPE> &a, Set<TYPE> &b);
} commands[] = {
	{ "add" , func_add }, { "+"   , func_add },
	{ "mul" , func_mul }, { "*"   , func_mul },
	{ "sub" , func_sub }, { "-"   , func_sub },
	{ "diff", func_dif },

	{ "lt"  , func_lt  }, { "<"   , func_lt  },
	{ "le"  , func_le  }, { "<="  , func_le  },
	{ "eq"  , func_eq  }, { "=="  , func_eq  },
	{ "ne"  , func_ne  }, { "!="  , func_ne  },
	{ "ge"  , func_ge  }, { ">="  , func_ge  },
	{ "gt"  , func_gt  }, { ">"   , func_gt  },

	{ "num" , func_num }, { "#"   , func_num },
	{ "empty", func_emp },
	{ "print", func_prn },

	{ 0, 0 }
};



/******************** Declarations ********************/
void usage(const char *argv0);
int read_set(Set<TYPE> &set, int argc, const char *const *argv, int pos);
int read_set_minusF(Set<TYPE> &set, int argc, const char *const *argv,
                    int pos, const char *prefix);



/********** Main **********/
int main(int argc, char **argv) {
	const char *prefix = 0;
	/* Prefix */
	if (argc>1 && !strncmp(argv[1], "-F", 2)) {
		prefix = argv[1] + 2;
		argv[1] = argv[0];
		++argv;
		--argc;
	}

	/* Usage */
	if (argc==1 || !strcmp(argv[1], "help")
		|| !strcmp(argv[1], "--help") || !strcmp(argv[1], "-h")) {
		usage(*argv);
		return 0;
	}

	/* Find handler */
	unsigned i = 0;
	while (commands[i].command && strcmp(argv[1], commands[i].command)) ++i;
	if (!commands[i].command) {
		std::cerr << argv[0] << ": " << argv[1]
		          << ": invalid command; use --help for help\n";
		return 1;
	}

	/* Read sets */
	Set<TYPE> a, b;
	if (!prefix) {
		read_set(b, argc, argv, read_set(a, argc, argv, 2));
	} else {
		const int pos = read_set_minusF(a, argc, argv, 2, prefix);
		read_set_minusF(b, argc, argv, pos, prefix);
	}

	/* Do */
	return commands[i].handler(a, b);
}



/******************** Usage ********************/
void usage(const char *argv0) {
	for (const char *ch = argv0; *ch; ++ch) {
		if (*ch=='/' && ch[1]) {
			argv0 = ch + 1;
		}
	}

	std::cout <<
		argv0 << "  - set expression evaluator  $Revision: 1.10 $\n"
		"Copyright 2006 by Michal Nazarewicz (mina86/AT/mina86.com)\n"
		"Licensed under the Academic Free License version 2.1\n"
		"\n"
		"usage: " << argv0 << " [ -F[<prefix>] ] <command> <set1> [ -- <set2> ]\n"
		"<command> can be:\n"
		"  +  or  add   -- calculates <set1> u <set2>\n"
		"  *  or  mul   -- calculates <set1> n <set2>\n"
		"  -  or  sub   -- calculates <set1> \\ <set2>\n"
		"         diff  -- calculates <set1> -:- <set2>\n"
		"\n"
		"  <   or  lt   -- checks if <set1> is proper subset of <set2>\n"
		"  <=  or  le   -- checks if <set1> is subset of <set2>\n"
		"  ==  or  eq   -- checks if <set1> is equal to <set2>\n"
		"  !=  or  ne   -- checks if <set1> is not equal to <set2>\n"
		"  >=  or  ge   -- checks if <set1> is superset of <set2>\n"
		"  >   or  gt   -- checks if <set1> is proper superset of <set2>\n"
		"\n"
		"  #   or  num  -- prints number of elements in <set1>\n"
		"  empty        -- checks if <set1> is empty\n"
		"  print        -- prints <set1>\n"
		"\n"
		"Calculations are syngalized by outputing space separated list of all\n"
		"elements in resulting set.  True/false checks are sygnalized by\n"
		"exiting with 0/1 exit code.\n"
		"\n"
		"<set1> and <set2> should be a list of space separated integers.\n"
		"Arguments which are not numbers are treated as 0.  In aditional, '-f'\n"
		"followed by file name or '-' may be specified which means to read\n"
		"integers from given file or standard input.\n"
		"\n"
		"With -F<prefix> option given as the first argument, each argument\n"
		"within <set1> and <set2> is treated as if it was prefixed by\n"
		"'-f<prefix>' therefore <set1> and <set2> becomes a list of file names\n"
		"(or file name sufixes) to read set elements from.\n";
}



/******************** Reads set ********************/
void read_from_file(Set<TYPE> &set, const char *file, const char *prefix=0) {
	if (!*file || (!file[1] && file[0]=='-')) {
		for (TYPE num; cin >> num; set.add(num));
	} else if (!prefix) {
		std::ifstream ifs(file);
		if (!ifs) {
			std::cerr << file << ": could not open\n";
		} else {
			for (TYPE num; ifs >> num; set.add(num));
		}
	} else {
		std::string str(prefix);
		str += file;
		std::ifstream ifs(str.c_str());
		if (!ifs) {
			std::cerr << str << ": could not open\n";
		} else {
			for (TYPE num; ifs >> num; set.add(num));
		}
	}
}

int read_set(Set<TYPE> &set, int argc, const char *const *argv, int pos) {
	for (; pos<argc && strcmp(argv[pos], "--"); ++pos) {
		if (strncmp(argv[pos], "-f", 2)) {
#ifdef BIGINT_SET
			set.add(BigInt(argv[pos]));
#else
			set.add(strtol(argv[pos], 0, 0));
#endif
			continue;
		}

		if (argv[pos][2]) {
			read_from_file(set, argv[pos] + 2);
		} else if (++pos<argc) {
			read_from_file(set, argv[pos]);
		} else {
			std::cerr << "-f: requires an argument";
		}
	}
	return pos + 1;
}

int read_set_minusF(Set<TYPE> &set, int argc, const char *const *argv,
                    int pos, const char *prefix) {
	if (!*prefix) prefix = 0;
	for (; pos<argc && strcmp(argv[pos], "--"); ++pos) {
		read_from_file(set, argv[pos], prefix);
	}
	return pos + 1;
}


/******************** Commands implementation ********************/
static int func_add(Set<TYPE> &a, Set<TYPE> &b) {
	std::cout << a.add(b) << '\n';
	return 0;
}

static int func_mul(Set<TYPE> &a, Set<TYPE> &b) {
	std::cout << a.mul(b) << '\n';
	return 0;
}

static int func_sub(Set<TYPE> &a, Set<TYPE> &b) {
	std::cout << a.sub(b) << '\n';
	return 0;
}

static int func_dif(Set<TYPE> &a, Set<TYPE> &b) {
	std::cout << a.diff(b) << '\n';
	return 0;
}


static int func_lt (Set<TYPE> &a, Set<TYPE> &b) {
	return !a.isProperSubsetOf(b);
}

static int func_le (Set<TYPE> &a, Set<TYPE> &b) {
	return !a.isSubsetOf(b);
}

static int func_eq (Set<TYPE> &a, Set<TYPE> &b) {
	return !a.eq(b);
}

static int func_ne (Set<TYPE> &a, Set<TYPE> &b) {
	return !a.neq(b);
}

static int func_ge (Set<TYPE> &a, Set<TYPE> &b) {
	return !a.isSupersetOf(b);
}

static int func_gt (Set<TYPE> &a, Set<TYPE> &b) {
	return !a.isProperSupersetOf(b);
}

static int func_num(Set<TYPE> &a, Set<TYPE> &b) {
	(void)b; /* Supress warning */
	std::cout << a.size() << '\n';
	return 0;
}

static int func_emp(Set<TYPE> &a, Set<TYPE> &b) {
	(void)b; /* Supress warning */
	return (bool)a;
}

static int func_prn(Set<TYPE> &a, Set<TYPE> &b) {
	(void)b; /* Supress warning */
	std::cout << a << '\n';
	return 0;
}
