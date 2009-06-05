/**
 * \file
 * Relation Demo Program.
 * $Id: rel-demo.cpp,v 1.4 2006/12/06 20:32:32 mina86 Exp $
 * Copyright 2006 by Michal Nazarewicz (mina86/AT/mina86.com)
 * Licensed under the Academic Free License version 2.1
 */

#include <iostream>
#include <ostream>

#include "relation.hpp"

using mina86::Relation;
using mina86::Set;


#define CL_NORM     "\x1b[0m"
#define CL_EXC      "\x1b[0;31m  "
#define CL_HEADING  "\x1b[1;32m  "
#define CL_SUBHEAD  "\x1b[0;32m"
#define CL_PROMPT   "\x1b[1;37m  "


std::ostream &operator<<(std::ostream &os, const Relation<std::string> &r) {
	return os << CL_SUBHEAD "  Domain: " CL_NORM << r.getDomain()
	          << CL_SUBHEAD "\nCodomain: " CL_NORM << r.getCodomain()
	          << CL_SUBHEAD "\n   Graph: " CL_NORM << r.getGraph() << "\n";
}



int main() {
	Relation<std::string> A, B;
	Set<std::string> set;
	std::string str1, str2;


	std::cout << "Relation Demo   $Id: rel-demo.cpp,v 1.4 2006/12/06 20:32:32 mina86 Exp $\n\n";


	/* Read first relation */
	std::cout <<
		CL_HEADING "The first relation (A):\n"
		CL_PROMPT  "Enter domain (end with '--'):\n" CL_NORM;
	for (; std::cin >> str1 && str1 != "--"; set.add(str1));
	A.setDomain(set);

	std::cout <<
		CL_PROMPT "Now codomain (empty means the same as domain):\n" CL_NORM;
	for (set.clear(); std::cin >> str1 && str1 != "--"; set.add(str1));
	A.setCodomain(set ? set : A.getDomain());

	std::cout << CL_PROMPT "Now relations:\n" CL_NORM;
	while (std::cin>>str1 && str1!="--" && std::cin>>str2 && str2!="--") {
		try {
			A.add(str1, str2);
		}
		catch (const Relation<std::string>::BadPair &e) {
			std::cerr << CL_EXC "BadPair: invalid value in pair "
			          << e.getPair() << "\n" CL_NORM;
		}
	}


	/* Checking */
	std::cout <<
		CL_PROMPT "Now enter pairs to check if they are in relation:\n"
		CL_NORM;
	while (std::cin>>str1 && str1!="--" && std::cin>>str2 && str2!="--") {
		std::cout << CL_SUBHEAD << str1 << "<A>" << str2 << CL_NORM ": "
		          << ((str1<A>str2) ? "yes\n" : "no\n");
	}


	/* Read second relation */
	std::cout <<
		CL_HEADING "The second relation (B):\n"
		CL_PROMPT
		"Enter domain (empty means same as the first relaton's domain):\n"
		CL_NORM;
	for (set.clear(); std::cin >> str1 && str1 != "--"; set.add(str1));
	B.setDomain(set ? set : A.getDomain());

	std::cout <<
		CL_PROMPT "Now codomain (empty means the same as domain):\n" CL_NORM;
	for (set.clear(); std::cin >> str1 && str1 != "--"; set.add(str1));
	B.setCodomain(set ? set : B.getDomain());

	std::cout << CL_PROMPT "Now relations:\n" CL_NORM;
	while (std::cin>>str1 && str1!="--" && std::cin>>str2 && str2!="--") {
		try {
			B.add(str1, str2);
		}
		catch (const Relation<std::string>::BadPair &e) {
			std::cerr << CL_EXC "BadPair: invalid value in pair "
			          << e.getPair() << "\n" CL_NORM;
		}
	}


	/* Print */
#define PRINT(expr) do {								\
		std::cout << CL_HEADING #expr CL_NORM ":\n";	\
		try {											\
			std::cout << (expr) << "\n";				\
		}												\
		catch(const Relation<std::string>::BadPair&e) {	\
			std::cerr <<								\
				CL_EXC "BadPair: one of the values are "\
				"not in domain or codomain\n" CL_NORM;	\
		}												\
	} while (0)

	PRINT(A);
	PRINT(B);

#define OP(op) do { PRINT(A op B); PRINT(B op A); } while (0)

	OP(+);
	OP(-);
	OP(*);
	OP(^);

#undef PRINT
#undef OP

#define TEST_PROP(set, prop)								\
	do {													\
		std::cout << " " CL_SUBHEAD #set CL_NORM ": ";		\
		try {												\
			std::cout << ((set.is ##prop())?"yes":"no ");	\
		}													\
		catch (const Relation<std::string>::BadPair &e) {	\
			std::cerr << CL_EXC "BadPair" CL_NORM;			\
		}													\
		std::cout << ';';									\
	} while (0)

#define PROP(prop)							\
	do {									\
		std::cout << CL_HEADING;			\
		std::cout.width(15);				\
		std::cout << #prop << CL_NORM ":";	\
		TEST_PROP(A, prop);					\
		TEST_PROP(B, prop);					\
		std::cout << '\n';					\
	} while (0)

	PROP(Empty);
	PROP(LeftTotal);
	PROP(RightTotal);
	PROP(Functional);
	PROP(Surjective);
	PROP(Injective);
	PROP(Bijective);
	PROP(Reflexive);
	PROP(Irreflexive);
	PROP(Coreflexive);
	PROP(Symmetric);
	PROP(Antisymmetric);
	PROP(Asymmetric);
	PROP(Transitive);
	PROP(Total);
	PROP(Linear);
	PROP(Trichotomous);
	PROP(Euclidean);
	PROP(Extendable);
	PROP(Serial);
	PROP(PreOrder);
	PROP(QuasiOrder);
	PROP(Equivalence);
	PROP(PartialOrder);
	PROP(TotalOrder);
	PROP(LinearOrder);

#undef PROP
#undef TEST_PROP

	return 0;
}
