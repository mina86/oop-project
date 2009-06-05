/**
 * \file
 * Pair Header File.
 * $Id: pair.hpp,v 1.4 2006/12/01 22:26:17 mina86 Exp $
 * Copyright 2006 by Michal Nazarewicz (mina86/AT/mina86.com)
 * Licensed under the Academic Free License version 2.1
 */

#ifndef MN_PAIR_HPP
#define MN_PAIR_HPP

#include "policy-compare.hpp"

namespace mina86 {


/**
 * A template class representing an ordered pair.  \a T1 and \a T2
 * template arguments specifies the type of the left and the right
 * element of the pair.
 */
template<class T1, class T2 = T1>
class Pair {
	/**
	 * The left (or the first) element of the ordered pair.
	 */
	T1 a;

	/**
	 * The right (or the second) element of the ordered pair.
	 */
	T2 b;


public:
	/**
	 * Default constructor.  Initializes pair with default object
	 * values.
	 */
	Pair() : a(), b() { }

	/**
	 * Copy constructor.  Copies values of left and right elements
	 * from another Pair.  The Pair values are copied from dosn't have
	 * to have the same compare policy as this object.
	 *
	 * \param[in] p Pair to copy values from
	 * \sa set(const Pair<T1, T2, cmp1, cmp2> &)
	 */
	Pair(const Pair<T1, T2> &p) : a(p.a), b(p.b) { }

	/**
	 * Sets left and right elements to given values.
	 *
	 * \param[in] A value for left element
	 * \param[in] B value for right element
	 * \sa  set(const T1 &A, const T2 &B)
	 */
	Pair(const T1 &A, const T2 &B) : a(A), b(B) { }


	/**
	 * Returns the first element.
	 *
	 * \return const reference to left element
	 */
	const T1 &left () const { return a; }

	/**
	 * Returns the first element.
	 *
	 * \return reference to left element
	 */
	      T1 &left ()       { return a; }

	/**
	 * Returns the second element.
	 *
	 * \return const reference to right element
	 */
	const T2 &right() const { return b; }

	/**
	 * Returns the second element.
	 *
	 * \return reference to right element
	 */
	      T2 &right()       { return b; }


	/**
	 * Copies values of left and right elements from another Pair.
	 *
	 * \param[in] p Pair to copy values from
	 */
	void set(const Pair<T1, T2> &p) { a = p.a; b = p.b; }

	/**
	 * Sets left and right elements to given values.
	 *
	 * \param[in] A value for left element
	 * \param[in] B value for right element
	 */
	void set(const T1 &A, const T2 &B) { a = A; b = B; }

	/**
	 * Sets first element.
	 *
	 * \param[in] A value for left element
	 */
	void setLeft (const T1 &A) { a = A; }

	/**
	 * Sets rights element.
	 *
	 * \param[in] B value for left element
	 */
	void setRight(const T2 &B) { b = B; }


	/**
	 * Compare policy for Pair objects.  Linear order of Pair objects
	 * is defined as follows: one pair is greater then or equal to
	 * another iff either left element of the first pair is greater
	 * then left element of the second pair or left elements are equal
	 * and right element of the first part is greater then or equal to
	 * right element of the second pair, ie:
	 *
	 * \code
	 * (x, y) >= (a, b)  <=>  (x > a) or ( (x == a) and (y >= b) )
	 * \endcode
	 */
	template<class cmp1 = default_cmp_policy<T1>,
	         class cmp2 = default_cmp_policy<T2> >
	struct cmp_policy {
		/**
		 * Whether linear order exists.  Types of both elements have
		 * to have a linear order for it to be \c true.
		 */
		static const bool ordered = cmp1::ordered && cmp2::ordered;

		/**
		 * Compares two pair elements.
		 *
		 * \param[in] a first Pair
		 * \param[in] b second Pair
		 * \return \c true if pairs are equal
		 */
		static bool eq(const Pair<T1, T2> &a, const Pair<T1, T2> &b) {
			return cmp1::eq(a.left(), b.left())
				&& cmp2::eq(a.right(), b.right());
		}

		/**
		 * Compares two pair elements.
		 *
		 * \param[in] a first Pair
		 * \param[in] b second Pair
		 * \return \c true if pair a is greater then or equal to pair b
		 */
		static bool ge(const Pair<T1, T2> &a, const Pair<T1, T2> &b) {
			return cmp1::gt(a.left(), b.left()) ||
				(cmp1::eq(a.left(), b.left()) &&
				 cmp2::ge(a.right(), b.right()));
		}

		/**
		 * Compares two pair elements.
		 *
		 * \param[in] a first Pair
		 * \param[in] b second Pair
		 * \return \c true if pair a is greater then pair b
		 */
		static bool gt(const Pair<T1, T2> &a, const Pair<T1, T2> &b) {
			return cmp1::gt(a.left(), b.left()) ||
				(cmp1::eq(a.left(), b.left()) &&
				 cmp2::gt(a.right(), b.right()));
		}
	};


};



/**
 * Sends Pair to output stream in the form: (left, right).
 *
 * \param[in] os output stream to send data to
 * \param[in] p  a pair to send
 * \return \a os
 */
template<class T1, class T2>
std::ostream &operator<<(std::ostream &os, const Pair<T1, T2> &p) {
	return os << '(' << p.left() << ", " << p.right() << ')';
}



/**
 * Specialization of is_ordered template.  Simply says that Pair<T1,
 * T2> has a linear order.  Such pair has it if both types have some
 * kind of linear order.
 *
 * \a cmp1 and \a cmp2 template arguments specifies compare policy for
 * left and right elements.  This need to be a structure with a static
 * function eq() checking whether two given arguments are equal.
 */
template<class T1, class T2>
struct is_ordered< Pair<T1, T2> > {
	/**
	 * Field says whether objects of type \a T has default linear
	 * order relation.  In most cases it means that you can use ==,
	 * !=, >=, >, < and <= operators on them in an obvious manner, not
	 * always though.  For instance, \c char* is considered to have an
	 * linear order relation even though >= operator won't work on it.
	 */
	static const bool v = is_ordered<T1>::v && is_ordered<T2>::v;
};



/**
 * Default compare policy for Pair objects.  Linear order of Pair
 * objects is defined as follows: one pair is greater then or equal to
 * another iff either left element of the first pair is greater then
 * left element of the second pair or left elements are equal and
 * right element of the first part is greater then or equal to right
 * element of the second pair, ie:
 *
 * \code
 * (x, y) >= (a, b)  <=>  (x > a) or ( (x == a) and (y >= b) )
 * \endcode
 */
template<class T1, class T2>
struct default_cmp_policy<Pair<T1, T2> >
	: public Pair<T1, T2>::template cmp_policy<> {
};


/**
 * Checks if pairs are equal.  Uses default_cmp_policy to compare
 * elements.
 *
 * \param[in] a the first Pair to test for equality
 * \param[in] b the second Pair to test for equality
 * \return \c true if both pairs are equal, \c false otherwise
 */
template<class T1, class T2>
bool operator==(const Pair<T1, T2> &a, const Pair<T1, T2> &b) {
	return default_cmp_policy<Pair<T1, T2> >::eq(a, b);
}


/**
 * Checks if pairs are not equal.  Uses Pair::cmp_policy to compare
 * elements.
 *
 * \param[in] a the first Pair to test for equality
 * \param[in] b the second Pair to test for equality
 * \return \c true if pairs are not equal, \c false otherwise
 */
template<class T1, class T2>
bool operator!=(const Pair<T1, T2> &a, const Pair<T1, T2> &b) {
	return !default_cmp_policy<Pair<T1, T2> >::eq(a, b);
}



}

#endif
