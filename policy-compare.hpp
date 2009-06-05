/**
 * \file
 * Compare Policy Header File.
 * $Id: policy-compare.hpp,v 1.4 2006/11/26 21:34:03 mina86 Exp $
 * Copyright 2006 by Michal Nazarewicz (mina86/AT/mina86.com)
 * Licensed under the Academic Free License version 2.1
 */

#ifndef MN_POLICY_COMPARE_HPP
#define MN_POLICY_COMPARE_HPP

#include <string>
#include <cstring>
#include <cmath>

namespace mina86 {


/**
 * A template structure specifying whether given type has some kind of
 * standard linear order relation.  It is mostly used by
 * default_cmp_policy to set \a ordered field.  It contains field \a v
 * which answers the question.
 */
template<class T>
struct is_ordered {
	/**
	 * Field says whether objects of type \a T has default linear
	 * order relation.  In most cases it means that you can use ==,
	 * !=, >=, >, < and <= operators on them in an obvious manner, not
	 * always though.  For instance, \c char* is considered to have an
	 * linear order relation even though >= operator won't work on it.
	 */
	static const bool v = false;
};


/**
 * A default policy for comparing objects of given type.  This default
 * policy order object in more or less standard way, ie. using default
 * ==, !=, >=, >, < and <= operators, however not always.  In
 * particular, \c char* are compared using strcmp() function from
 * standard library and floating point numbers are said to be equal if
 * they difference is less then \c 1e-9.
 *
 * User defined compare policies have to contain at least #ordered
 * field and eq() method plus ge() and gt() methods if #ordered is \c
 * true.  Relation defined by ge() method has to be a linear order
 * relation.  If it's not funny things may happen.
 *
 * Linear order defined by this template does not always have sens but
 * it's linear. :)
 */
template<class T>
struct default_cmp_policy {
	/**
	 * Specifies whether ge() and gt() methods exist and can be used
	 * to order objects of given type.  This is copied from the value
	 * is_ordered<T>::v.
	 */
	static const bool ordered = is_ordered<T>::v;

	/**
	 * Checks if both arguments are equal.
	 *
	 * \param[in] a first object
	 * \param[in] b second object
	 * \return \c true if both objects are equal
	 */
	static bool eq(const T &a, const T &b) { return a == b; }

	/**
	 * Compares two objects.
	 *
	 * \param[in] a first object
	 * \param[in] b second object
	 * \return \c true if first object is greater then on equal to the
	 *         second
	 */
	static bool ge(const T &a, const T &b) { return a >= b; }

	/**
	 * Compares two objects.
	 *
	 * \param[in] a first object
	 * \param[in] b second object
	 * \return \c true if first object is greater then the second
	 */
	static bool gt(const T &a, const T &b) { return a >  b; }
};


/** \cond REALLY_ALL */

template<class T> struct is_ordered<const T> {
	static const bool v = is_ordered<T>::v;
};

template<class T> struct is_ordered<T&> {
	static const bool v = is_ordered<T>::v;
};

template<> struct is_ordered<         char       >{static const bool v=true;};
template<> struct is_ordered<  signed char       >{static const bool v=true;};
template<> struct is_ordered<unsigned char       >{static const bool v=true;};
template<> struct is_ordered<  signed short      >{static const bool v=true;};
template<> struct is_ordered<unsigned short      >{static const bool v=true;};
template<> struct is_ordered<  signed int        >{static const bool v=true;};
template<> struct is_ordered<unsigned int        >{static const bool v=true;};
template<> struct is_ordered<  signed long       >{static const bool v=true;};
template<> struct is_ordered<unsigned long       >{static const bool v=true;};
template<> struct is_ordered<         float      >{static const bool v=true;};
template<> struct is_ordered<         double     >{static const bool v=true;};
template<> struct is_ordered<         long double>{static const bool v=true;};
template<> struct is_ordered<         std::string>{static const bool v=true;};
template<> struct is_ordered<         char*      >{static const bool v=true;};
template<> struct is_ordered<  signed char*      >{static const bool v=true;};
template<> struct is_ordered<unsigned char*      >{static const bool v=true;};



template<class T>
struct default_cmp_policy<const T> {
	static const bool ordered = default_cmp_policy<T>::ordered;

	static bool eq(const char *a, const char *b) {
		return default_cmp_policy<T>::eq(a, b);
	}

	static bool ge(const char *a, const char *b) {
		return default_cmp_policy<T>::ge(a, b);
	}

	static bool gt(const char *a, const char *b) {
		return default_cmp_policy<T>::gt(a, b);
	}
};


	template<class T>
struct default_cmp_policy<T&> {
	static const bool ordered = default_cmp_policy<T>::ordered;

	static bool eq(const char *a, const char *b) {
		return default_cmp_policy<T>::eq(a, b);
	}

	static bool ge(const char *a, const char *b) {
		return default_cmp_policy<T>::ge(a, b);
	}

	static bool gt(const char *a, const char *b) {
		return default_cmp_policy<T>::gt(a, b);
	}
};


template<>
struct default_cmp_policy<         char*> {
	static const bool ordered = true;
	static bool eq(const char *a, const char *b) { return strcmp(a, b)==0; }
	static bool ge(const char *a, const char *b) { return strcmp(a, b)>=0; }
	static bool gt(const char *a, const char *b) { return strcmp(a, b)> 0; }
};


template<>
struct default_cmp_policy<  signed char*> {
	static const bool ordered = true;
	static bool eq(const char *a, const char *b) { return strcmp(a, b)==0; }
	static bool ge(const char *a, const char *b) { return strcmp(a, b)>=0; }
	static bool gt(const char *a, const char *b) { return strcmp(a, b)> 0; }
};


template<>
struct default_cmp_policy<unsigned char*> {
	static const bool ordered = true;
	static bool eq(const char *a, const char *b) { return strcmp(a, b)==0; }
	static bool ge(const char *a, const char *b) { return strcmp(a, b)>=0; }
	static bool gt(const char *a, const char *b) { return strcmp(a, b)> 0; }
};



template<>
struct default_cmp_policy<long double> {
	static const bool ordered = true;
	static bool eq(long double a, long double b) { return fabsl(a-b) < 1e-9; }
	static bool ge(long double a, long double b) { return a + 1e-9 >= b; }
	static bool gt(long double a, long double b) { return a - 1e-9 >  b; }
};


template<>
struct default_cmp_policy<    double> {
	static const bool ordered = true;
	static bool eq(     double a,      double b) { return fabs(a-b) < 1e-9; }
	static bool ge(     double a,      double b) { return a + 1e-9 >= b; }
	static bool gt(     double a,      double b) { return a - 1e-9 >  b; }
};


template<>
struct default_cmp_policy<     float > {
	static const bool ordered = true;
	static bool eq(     float  a,      float  b) { return fabsf(a-b) < 1e-9; }
	static bool ge(     float  a,      float  b) { return a + 1e-9 >= b; }
	static bool gt(     float  a,      float  b) { return a - 1e-9 >  b; }
};


/** \endcond */


}

#endif
