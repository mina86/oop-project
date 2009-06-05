/**
 * \file
 * BigInt Implementation.
 * $Id: bigint.cpp,v 1.9 2006/11/26 19:04:00 mina86 Exp $
 * Copyright 2006 by Michal Nazarewicz (mina86/AT/mina86.com)
 * Licensed under the Academic Free License version 2.1
 */

#include <cstdio>
#include <cmath>

#include "bigint.hpp"


namespace mina86 {


/**
 * Static buffer for convertin integers into BigInt objects.
 */
static char buffer[2 + 3 * sizeof(long)];



/********** integer -> BigInt **********/
void BigInt::set(long num) {
	if (!num) {
		sign = 0;
	} else {
		sign = num < 0 ? -1 : 1;
		int len = sprintf(buffer, "%+ld", num);
		set_digits(buffer + 1, buffer + len);
	}
}

void BigInt::set(unsigned long num) {
	if ((sign = !!num)) {
		int len = sprintf(buffer, "%lu", num);
		set_digits(buffer, buffer + len);
	}
}


/********** floating point -> BigInt **********/
void BigInt::set(double num) {
	if (fabs(num)<1) {
		zero();
		return;
	}

	if (num<0) {
		sign = -1;
		num = -num;
	} else {
		sign = 1;
	}

	digits.resize(0, (int)log10(num) + 4);
	do {
		digits.push((char)(fmod(num, 10)));
	} while ((num /= 10)>=1);
}


/********** C string -> BigInt **********/
bool BigInt::set(const char *str) {
	while (*str==' '  || *str=='\t' || *str=='\n' ||
	       *str=='\r' || *str=='\v' || *str=='\f') ++str;

	int s = 1;
	if (*str=='-') {
		s = -1;
		++str;
	} else if (*str=='+') {
		++str;
	}

	const bool got = *str=='0';
	while (*str=='0') ++str;

	const char *end = str;
	while (*end>='0' && *end<='9') ++end;

	if (str!=end) {
		sign = s;
		set_digits(str, end);
		return true;
	} else if (got) {
		zero();
		return true;
	} else {
		return false;
	}
}


/********** Adds number to *this **********/
BigInt &BigInt::add(const BigInt &n) {
	if (!n.sign) {
		/* nothing */
	} else if (!sign) {
		*this = n;
	} else if (sign * n.sign > 0) {
		abs_add(n);
	} else {
		abs_sub(n);
	}
	return *this;
}


/********** Substracts number from *this **********/
BigInt &BigInt::sub(const BigInt &n) {
	/* At least one of the operands is zero */
	if (!n.sign) {
		return *this;
	}
	if (!sign) {
		digits = n.digits;
		sign = -n.sign;
		return *this;
	}

	/* Have different signs */
	if (sign * n.sign < 0) {
		abs_add(n);
		return *this;
	}

	/* Both have the same sign */
	sign *= -1;
	abs_sub(n);
	sign *= -1;
	return *this;
}


/********** Multiplies *this by number **********/
BigInt &BigInt::mul(const BigInt &n) {
	/* At least one of the operands is zero */
	if (!sign) {
		return *this;
	} else if (!n.sign) {
		digits.clear();
		sign = 0;
		return *this;
	}

	sign *= n.sign;

	/* Init */
	const unsigned l = digits.size(), nl = n.digits.size();
	const char *const d = digits.begin(), *const nd = n.digits.begin();

	unsigned i = l + nl;
	char *vec = new char[i];
	do { vec[--i] = 0; } while (i);

	/* The loop */
	for (i = 0; i<l; ++i) {
		if (!d[i]) {
			continue;
		}
		unsigned char memory = 0;
		unsigned ni = 0;

		for (; ni<nl; ++ni) {
			const unsigned char tmp = vec[i + ni] + d[i] * nd[ni] + memory;
			vec[i + ni] = tmp % 10;
			memory      = tmp / 10;
		}

		for (ni += i; memory; ++ni) {
			const unsigned char tmp = vec[ni] + memory;
			vec[ni] = tmp % 10;
			memory  = tmp / 10;
		}
	}

	/* Remove zeros */
	i = l + nl;
	if (!vec[i - 1]) --i;
	digits.set_no_copy(vec, i, l + nl);
	return *this;
}



/********** Compare numbers **********/
int     BigInt::cmp(const BigInt &n) const {
	if (sign<n.sign) {
		return -1;
	} else if (sign>n.sign) {
		return  1;
	} else if (!sign) {
		return  0;
	} else if (digits.size()<n.digits.size()) {
		return -sign;
	} else if (digits.size()>n.digits.size()) {
		return  sign;
	} else {
		const char *d = digits.end(), *nd = n.digits.end();
		unsigned i = digits.size();
		while (i && *--d == *--nd) --i;
		return i ? *d < *nd ? -sign : sign : 0;
	}
}



/********** Adds number to *this ignoring sign **********/
void BigInt::abs_add(const BigInt &n) {
	/* Append zeros */
	if (digits.size()<n.digits.size()) {
		unsigned i = digits.size();
		digits.resize(n.digits.size());
		char *d = digits.rw_begin() + i;
		i = n.digits.size() - i;
		do *d++ = 0; while (--i);
	}

	/* Init */
	const char *nd = n.digits.begin();
	char *d = digits.rw_begin();
	unsigned char memory = 0;

	/* Add */
	unsigned len = n.digits.size();
	do {
		const char tmp = *d + *nd++ + memory;
		*d++   = tmp % 10;
		memory = tmp / 10;
	} while (--len);

	/* Pending memory */
	if (memory) {
		for (len = digits.size() - n.digits.size(); len && *d==9; --len) {
			*d++ = 0;
		}
		if (len) {
			++*d;
		} else {
			digits.push(1);
		}
	}
}



/********** Substracts number from *this ignoring sign **********/
void BigInt::abs_sub(const BigInt &n) {
	/* Compare */
	int c;
	if (!sign) {
		c = n.sign ? -1 : 0;
	} else if (!n.sign) {
		c = 1;
	} else if (digits.size() > n.digits.size()) {
		c = 1;
	} else if (digits.size() < n.digits.size()) {
		c = -1;
	} else {
		const char *d = digits.end(), *nd = n.digits.end();
		unsigned i = digits.size();
		while (i && *--d == *--nd) --i;
		c = i ? *d < *nd ? -1 : 1 : 0;
	}

	/* Check compersion result */
	switch (c) {
	case  0:
		zero();
		return;

	case -1:
		BigInt tmp(n);
		tmp.abs_sub(*this);
		*this = tmp;
		return;
	}

	/* Init */
	const char *nd = n.digits.begin(), *const end = n.digits.end();
	char *d = digits.rw_begin();
	unsigned char memory = 0;

	/* Substract */
	do {
		const unsigned char tmp = memory + *nd++;
		*d -= (memory = tmp > *d) ? tmp - 10 : tmp;
		++d;
	} while (nd!=end);

	/* Pendig memory */
	if (memory) {
		while (!*d) *d++ = 9;
		--*d;
	}

	/* Remove zeros */
	nd = digits.end();
	if (!*--nd) {
		while (!*--nd);
		digits.resize(nd - digits.begin() + 1);
	}
}



/********** Prints BigInt **********/
std::ostream &operator<<(std::ostream &os, const BigInt &n) {
	if (!n.sgn()) {
		return os << '0';
	}

	if (n.sgn()<0) os << '-';
	const char *const begin = n.getDigits().begin();
	const char *end = n.getDigits().end();
	do os << (char)(*--end | '0'); while (end!=begin);
	return os;
}



/********** Reads BigInt **********/
std::istream &operator>>(std::istream &is, BigInt &n) {
	int ch, s = 1;

	/* Skip whitepsace */
	do {
		ch = is.get();
	} while (ch==' '  || ch=='\t' || ch=='\n' ||
	         ch=='\r' || ch=='\v' || ch=='\f');

	/* Sign */
	if (ch=='-') {
		s = -1;
		ch = is.get();
	} else if (ch=='+') {
		ch = is.get();
	}

	/* Not a digit */
	if (ch<'0' || ch>'9') {
		is.unget();
		is.setstate(std::istream::failbit);
		return is;
	}

	/* Skip zeros */
	const bool got = ch=='0';
	while (ch=='0') ch = is.get();

	/* Read digits */
	Array<char> arr;
	for (; ch>='0' && ch<='9'; ch = is.get()) {
		arr.push(ch & 15);
	}

	/* Set */
	if (arr) {
		arr.reverse();
		n.digits = arr;
		n.sign = s;
	} else if (got) {
		n.digits.clear();
		n.sign = 0;
	} else {
		is.unget();
		is.setstate(std::istream::failbit);
	}

	return is;
}


}
