/**
 * \file
 * BigInt RPN Calculator Mian file.
 * $Id: rpn.cpp,v 1.6 2006/11/26 19:05:03 mina86 Exp $
 * Copyright 2006 by Michal Nazarewicz (mina86/AT/mina86.com)
 * Licensed under the Academic Free License version 2.1
 */


#include <iostream>

#include "bigint.hpp"
#include "array.hpp"

using mina86::BigInt;
using mina86::Array;


/******************** Commands ********************/
static void func_add(Array<BigInt> &stack);
static void func_sub(Array<BigInt> &stack);
static void func_mul(Array<BigInt> &stack);
static void func_dup(Array<BigInt> &stack);
static void func_prn(Array<BigInt> &stack);
static void func_del(Array<BigInt> &stack);
static void func_xhg(Array<BigInt> &stack);
static void func_all(Array<BigInt> &stack);
static void func_cls(Array<BigInt> &stack);
static void func_hlp(Array<BigInt> &stack);
static void func_ext(Array<BigInt> &stack);


static struct {
	std::string command;
	void (*handler)(Array<BigInt> &stack);
	unsigned min;
	const char *description;
} commands[] = {
	{ "+"    , func_add, 2, 0 },
	{ "a"    , func_add, 2, 0 },
	{ "add"  , func_add, 2, 0 },
	{ "sum"  , func_add, 2, "Adds two numbers" },
	{ "-"    , func_sub, 2, 0 },
	{ "s"    , func_sub, 2, 0 },
	{ "sub"  , func_sub, 2, 0 },
	{ "diff" , func_sub, 2, "Substracts number at the top from next number" },
	{ "*"    , func_mul, 2, 0 },
	{ "m"    , func_mul, 2, 0 },
	{ "mul"  , func_mul, 2, "Multiplies two numbers" },
	{ "d"    , func_dup, 1, 0 },
	{ "dup"  , func_dup, 1, "Duplicates number at the top" },
	{ "p"    , func_prn, 1, 0 },
	{ "print", func_prn, 1, "Prints number at the top" },
	{ "del"  , func_del, 1, "Removes number from the top" },
	{ "x"    , func_xhg, 2, 0 },
	{ "xchg" , func_xhg, 2, "Exchanges two numbers at the top" },
	{ "P"    , func_all, 0, 0 },
	{ "all"  , func_all, 0, 0 },
	{ "stack", func_all, 0, 0 },
	{ "show" , func_all, 0, "Shows all number at the stack" },
	{ "cls"  , func_cls, 0, 0 },
	{ "clear", func_cls, 0, "Clears stack" },
	{ "?"    , func_hlp, 0, 0 },
	{ "h"    , func_hlp, 0, 0 },
	{ "help" , func_hlp, 0, "Displays help screen" },
	{ "q"    , func_ext, 0, 0 },
	{ "exit" , func_ext, 0, 0 },
	{ "quit" , func_ext, 0, "Exits calculator" },
	{ ""     ,        0, 0, 0 }
};


/******************** Main ********************/
int main(void) {
	Array<BigInt> stack;
	stack.reserve(128);

	/* Header */
	std::cerr <<
		"BigInt RPN Calculator  $Revision: 1.6 $\n" \
		"Copyright 2006 by Michal Nazarewicz (mina86/AT/mina86.com)\n" \
		"Licensed under the Academic Free License version 2.1\n\n";

	/* Main loop */
	for (std::string str; std::cin >> str; ) {
		/* It's a number */
		if ((str[0]>='0' && str[0]<='9') ||
			(str.size()>1 && (str[0]=='-' || str[0]=='+') &&
			 (str[1]>='0' && str[1]<='9'))) {
			stack.push(str);
			continue;
		}

		/* Find command */
		unsigned i = 0;
		while (commands[i].handler && commands[i].command != str) ++i;
		if (!commands[i].handler) {
			std::cerr << "rpn: " << str
			        << ": unknown command; type help for list of commands\n";
			continue;
		}

		/* It's a dummy exit entry */
		if (commands[i].handler == func_ext) break;

		/* Check stack */
		if (commands[i].min > stack.size()) {
			std::cerr << "rpn: " << str << ": requires at least "
			          << commands[i].min << " numbers on stack\n";
			continue;
		}

		/* Run command */
		commands[i].handler(stack);
	}

	return 0;
}



/******************** Commands ********************/
static void func_add(Array<BigInt> &stack) {
	const unsigned s = stack.size();
	stack.rw_at(s - 2) += stack.at(s - 1);
	stack.shrinkBy(1);
}

static void func_sub(Array<BigInt> &stack) {
	const unsigned s = stack.size();
	stack.rw_at(s - 2) -= stack.at(s - 1);
	stack.shrinkBy(1);
}

static void func_mul(Array<BigInt> &stack) {
	const unsigned s = stack.size();
	stack.rw_at(s - 2) *= stack.at(s - 1);
	stack.shrinkBy(1);
}

static void func_dup(Array<BigInt> &stack) {
	stack.push(stack[stack.size() - 1]);
}

static void func_prn(Array<BigInt> &stack) {
	std::cout << stack[stack.size() - 1] << '\n';
}

static void func_del(Array<BigInt> &stack) {
	stack.shrinkBy(1);
}

static void func_xhg(Array<BigInt> &stack) {
	const unsigned s = stack.size();
	stack.swap(s - 1, s - 2);
}

static void func_all(Array<BigInt> &stack) {
	const BigInt *end = stack.end(), *const begin = stack.begin();
	if (end==begin) {
		std::cout << "stack is empty\n";
	} else {
		std::cout << "top: " << *--end << '\n';
		while (end!=begin) {
			std::cout << "     " << *--end << '\n';
		}
	}
}

static void func_cls(Array<BigInt> &stack) {
	stack.clear();
}

static void func_hlp(Array<BigInt> &stack) {
	(void)stack; /* supress warning */
	unsigned i = 0, first = 1, pos = 0;

	std::cout << "Available commands: \n";
	for (; commands[i].handler; ++i) {
		std::cout << (first ? "  " : ", ") << commands[i].command;
		first = 0;
		pos += 2 + commands[i].command.size();
		if (!commands[i].description) continue;

		if (pos>20) {
			std::cout << '\n';
			pos = 0;
		}
		for (; pos<22; ++pos) std::cout << ' ';
		std::cout << commands[i].description << '\n';

		pos = 0;
		first = 1;
	}
	std::cout << '\n';
}

static void func_ext(Array<BigInt> &stack) {
	(void)stack; /* supress warning */
	/* this is dummy function */
}
