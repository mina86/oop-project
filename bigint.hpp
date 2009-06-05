/**
 * \file
 * BigInt Header File.
 * $Id: bigint.hpp,v 1.6 2006/11/26 19:04:00 mina86 Exp $
 * Copyright 2006 by Michal Nazarewicz (mina86/AT/mina86.com)
 * Licensed under the Academic Free License version 2.1
 */

#ifndef MN_BIGINT_HPP
#define MN_BIGINT_HPP

#include <istream>
#include <ostream>
#include <string>

#include "array.hpp"


namespace mina86 {


/**
 * Class representing big integer numbers.  Numbers are stored as an
 * array of digits and therefore can hold any integer value.  This
 * class does not (yet) support division.
 */
class BigInt {
	/**
	 * Sign of the number.  \c -1 if number is negative, \c 0 if it's
	 * zero, \c 1 otherwise.
	 */
	int sign;

	/**
	 * Array holding digits.
	 */
	Array<char> digits;



	/**
	 * Sums two BigInt numbers ignoring their sign.  Treats both
	 * numbers are positive (or negative if you prefer).
	 *
	 * \warning This method assumes that none of the number is zero.
	 *
	 * \param[in] n number to add
	 * \sa abs_sub(), add()
	 */
	void abs_add(const BigInt &n);

	/**
	 * Substracts two BigInt numbers ignoring their sign.  Treats both
	 * numbers are positive (or negative if you prefer).  If absolute
	 * value of \a n is greater then that of this object this object
	 * is assigned sign of \a n.
	 *
	 * \param[in] n number to substract
	 * \sa abs_add(), sub()
	 */
	void abs_sub(const BigInt &n);


	/**
	 * Copies digits from an array.  Digits in passed array are in
	 * "natural" order.  The first digit must not be zero.
	 *
	 * \warning This method does not check for any error conditions
	 *          (such as invalid digit, no digits, zero at the
	 *          begining).  It is callers responsibility to check for
	 *          those.
	 *
	 * \param[in] begin first digit
	 * \param[in] end last digit
	 */
	void set_digits(const char *begin, const char *end) {
		digits.resize(end - begin);
		for (char *d = digits.rw_begin(); begin!=end; *d++ = (*--end & 15));
	}


public:
	/**
	 * Default constructor.
	 */
	BigInt() : sign(0), digits() { }

	/**
	 * Copy constructor.
	 *
	 * \param[in] n BigInt object to copy
	 */
	BigInt(const BigInt &n) : sign(n.sign), digits(n.digits) { }

	/**
	 * Converts a floating point number into BigInt.  Rounds \a num
	 * towards zero.
	 *
	 * \param[in] num number to convert
	 */
	BigInt(double num) : sign(0), digits(0) {
		set(num);
	}

	/**
	 * Parses string.
	 *
	 * \param[in] str string to parse
	 * \sa set(std::string)
	 */
	BigInt(std::string str) : sign(0), digits() { set(str.c_str()); }

	/**
	 * Parses C string.
	 *
	 * \param[in] str string to parse
	 * \sa set(const char*)
	 */
	BigInt(const char *str) : sign(0), digits() { set(str); }



	/**
	 * Sets value of BigInt number from a long number.
	 *
	 * \param[in] num number to convert
	 */
	void set(long num);

	/**
	 * Sets value of BigInt number from an unsigned long number.
	 *
	 * \param[in] num number to convert
	 */
	void set(unsigned long num);

	/**
	 * Sets value of BigInt number from a floating point number.
	 * Rounds \a num towards zero.
	 *
	 * \param[in] num number to convert
	 */
	void set(double num);

	/**
	 * Parses string and sets value of BigInt number.
	 *
	 * \param[in] str string to parse
	 * \return \c true if string contained valid number, \c false
	 *         othewise
	 */
	bool set(std::string str) { return set(str.c_str()); }

	/**
	 * Parses C string and sets value of BigInt number.
	 *
	 * \param[in] str string to parse
	 * \return \c true if string contained valid number, \c false
	 *         othewise
	 */
	bool set(const char *str);

	/**
	 * Sets value of this object to the value of another BigInt
	 * object.
	 *
	 * \param[in] n number to copy
	 */
	void set(const BigInt &n) {
		digits = n.digits;
		sign = n.sign;
	}

	/**
	 * Assigns zero to the object.
	 */
	void zero() {
		sign = 0;
		digits.clear();
	}



	/**
	 * Adds a BigInt number.
	 *
	 * \param[in] n number to add
	 * \return reference to this object
	 * \sa sub(), mul(), abs_add()
	 */
	BigInt &add(const BigInt &n);

	/**
	 * Substracts a BigInt number.
	 *
	 * \param[in] n number to substract
	 * \return reference to this object
	 * \sa add(), mul(), abs_sub()
	 */
	BigInt &sub(const BigInt &n);

	/**
	 * Mulptilies a BigInt number.
	 *
	 * \param[in] n number to multiply by
	 * \return reference to this object
	 * \sa add(), sub()
	 */
	BigInt &mul(const BigInt &n);



	/**
	 * Compares two BigInt numbers.
	 *
	 * \param[in] n number to compare to
	 * \return \c -1 if \a n is greater then this, \c 0 if they are
	 *         equal, \c 1 if \n is lower then this
	 */
	int  cmp(const BigInt &n) const;



	/**
	 * Returns sign of the BigInt number.
	 *
	 * \return \c -1 if number is negative, \c 0 if it's zero, \c 1
	 *         otherwise
	 * \sa neg(), abs(), #sign
	 */
	int  sgn() const { return sign; }

	/**
	 * Negates number.
	 *
	 * \return reference to this object
	 * \sa sgn(), abs()
	 */
	BigInt &neg() { sign = -sign; return *this; }

	/**
	 * Assigns number it's absolute value.  Changes #sign to \c 1 if
	 * it was \c -1.
	 *
	 * \return reference to this object
	 * \sa sgn(), neg(), #sign
	 */
	BigInt &abs() { if (sign<0) sign = 1; return *this; }



	/**
	 * Returns number of character this number would take.  It is
	 * number of digits plus one if number is negative (since a minus
	 * sign consumes one character as well.
	 *
	 * \return number of characters needed to represent number
	 */
	unsigned size() const {
		return sign ? digits.size() + (sign==-1 ? 1 : 0) : 1;
	}

	/**
	 * Returns reference to an Array holding digits.  Note that digits
	 * are stored in "reverse order", ie. the first digit is the least
	 * significient.
	 *
	 * \return reference to digits Array
	 */
	const Array<char> &getDigits() const { return digits; }



	/**
	 * Adds number to this object.
	 *
	 * \param[in] n number to add
	 * \return reference to this object after addition
	 * \sa add()
	 */
	BigInt &operator+=(const BigInt &n) { return add(n); }

	/**
	 * Substracts number from this object.
	 *
	 * \param[in] n number to substract
	 * \return reference to this object ofter substraction
	 * \sa sub()
	 */
	BigInt &operator-=(const BigInt &n) { return sub(n); }

	/**
	 * Multiplies this object by a number.
	 *
	 * \param[in] n number to multiple by
	 * \return reference to this object ofter multiplication
	 * \sa add()
	 */
	BigInt &operator*=(const BigInt &n) { return mul(n); }



	/**
	 * Sets value of this object to the value of another BigInt
	 * object.
	 *
	 * \param[in] n number to copy
	 * \sa set(const BigInt &n)
	 */
	BigInt &operator= (const BigInt &n) {
		digits = n.digits;
		sign = n.sign;
		return *this;
	}



	/**
	 * Checks if number is not zero.
	 *
	 * \return \c true if number is not zero.
	 */
	operator bool() const { return sign; }

	/**
	 * Checks if number is zero.
	 *
	 * \return \c true if number is zero.
	 */
	bool operator !() const { return !sign; }



	/* Friends */
	friend std::istream &operator>>(std::istream &is, BigInt &n);
};




/**
 * Adds two BigInt numbers.
 *
 * \param[in] a first BigInt number
 * \param[in] b second BigInt number
 * \return new BigInt object which equal \a a plus \a b
 * \sa BigInt::add()
 */
inline BigInt operator+(const BigInt &a, const BigInt &b) {
	return BigInt(a).add(b);
}

/**
 * Substracts two BigInt numbers.
 *
 * \param[in] a first BigInt number
 * \param[in] b second BigInt number
 * \return new BigInt object which equals \a a minus \a b
 * \sa BigInt::sub()
 */
inline BigInt operator-(const BigInt &a, const BigInt &b) {
	return BigInt(a).sub(b);
}

/**
 * Multiplies two BigInt numbers.
 *
 * \param[in] a first BigInt number
 * \param[in] b second BigInt number
 * \return new BigInt object which equals \a a multiplied by \a b
 * \sa BigInt::mul()
 */
inline BigInt operator*(const BigInt &a, const BigInt &b) {
	return BigInt(a).mul(b);
}



/**
 * Returns it's argument.  A no-op.
 *
 * \param[in] n BigInt number
 * \return \a n
 */
inline const BigInt &operator+ (const BigInt &n) {
	return n;
}

/**
 * Negates number.
 *
 * \param[in] n BigInt numebr to negate
 * \return new BigInt number which is negation of \a n
 * \sa BigInt::neg()
 */
inline BigInt operator-(const BigInt &n) {
	return BigInt(n).neg();
}

/**
 * Returns abstract value of a number.
 *
 * \param[in] n BigInt number
 * \return new BigInt number which equals absolute value of \a n
 * \sa BigInt::abs()
 */
inline BigInt abs(const BigInt &n) {
	return BigInt(n).abs();
}



/**
 * Compares two BigInt numbers.
 *
 * \param[in] a first BigInt number
 * \param[in] b second BigInt number
 * \return \c true if both numebrs are equal
 * \sa BigInt::cmp()
 */
inline bool operator==(const BigInt &a, const BigInt &b) {
	return a.cmp(b)==0;
}

/**
 * Compares two BigInt numbers.
 *
 * \param[in] a first BigInt number
 * \param[in] b second BigInt number
 * \return \c true if numbers are not equal
 * \sa BigInt::cmp()
 */
inline bool operator!=(const BigInt &a, const BigInt &b) {
	return a.cmp(b)!=0;
}

/**
 * Compares two BigInt numbers.
 *
 * \param[in] a first BigInt number
 * \param[in] b second BigInt number
 * \return \c true if \a a is greater then or equal to \a b
 * \sa BigInt::cmp()
 */
inline bool operator>=(const BigInt &a, const BigInt &b) {
	return a.cmp(b)>=0;
}

/**
 * Compares two BigInt numbers.
 *
 * \param[in] a first BigInt number
 * \param[in] b second BigInt number
 * \return \c true if \a a is lower then or equal to \a b
 * \sa BigInt::cmp()
 */
inline bool operator<=(const BigInt &a, const BigInt &b) {
	return a.cmp(b)<=0;
}

/**
 * Compares two BigInt numbers.
 *
 * \param[in] a first BigInt number
 * \param[in] b second BigInt number
 * \return \c true if \a a is greater then \a b
 * \sa BigInt::cmp()
 */
inline bool operator> (const BigInt &a, const BigInt &b) {
	return a.cmp(b)> 0;
}

/**
 * Compares two BigInt numbers.
 *
 * \param[in] a first BigInt number
 * \param[in] b second BigInt number
 * \return \c true if \a a is lower then \a b
 * \sa BigInt::cmp()
 */
inline bool operator< (const BigInt &a, const BigInt &b) {
	return a.cmp(b)< 0;
}



/**
 * Sends BigInt number to an output stream.
 *
 * \param[in] os output stream to send to
 * \param[in] n number to print
 * \return \a os
 */
std::ostream &operator<<(std::ostream &os, const BigInt &n);

/**
 * Reads BigInt number from an input stream.  If there is no valid
 * integer in stream \a failbit is set and stream is left in undefined
 * state, ie. some characters may be lost.
 *
 * \param[in] is input stream to send to
 * \param[in] n number to read
 * \return \a os
 */
std::istream &operator>>(std::istream &is, BigInt &n);



}


#endif
