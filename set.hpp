/**
 * \file
 * Set Header File.
 * $Id: set.hpp,v 1.8 2006/12/18 20:00:31 mina86 Exp $
 * Copyright 2006 by Michal Nazarewicz (mina86/AT/mina86.com)
 * Licensed under the Academic Free License version 2.1
 */

#ifndef MN_SET_HPP
#define MN_SET_HPP

#include "array.hpp"


namespace mina86 {


/**
 * Class representing a set of elements of given type.  The \a cmp
 * template argument is a compare policy - it has to be a
 * structure/class containing static methods eq, ge and gt checking if
 * two given arguments are equal, the first is greater then or eqaul
 * to the second, or the first is greater then the second.  Those
 * methods have to define linear order which doesn't need to make any
 * sens, eg. you could compare complex numbers by compering their real
 * part and then (if it's equal) imaginary part - this will be a
 * linear order byt it probably won't make much sense in case of
 * algebra and such.  This template ignores \a ordered static member
 * of struct \a cmp.
 */
template<class T, class cmp = default_cmp_policy<T> >
class Set {
	/**
	 * A dynamic array for storing elements.
	 */
	Array<T> arr;



protected:
	/**
	 * Searches for element \a el.  Saves in \a pos index on which
	 * this element is or should be placed in \a arr.
	 *
	 * \param[in] element element to search for
	 * \param[out] pos index on which given element is placed if it's
	 *             in array
	 * \return \c true if element is in array, \c false otherwise
	 */
	bool find(const T &element, unsigned &pos) const;



public:
	/**
	 * Default constructor.
	 */
	Set() : arr() { }

	/**
	 * Copy consutrctor.
	 *
	 * \param[in] set Set object to copy
	 */
	Set(const Set<T, cmp> &set) : arr(set.arr) { }

	/**
	 * Initializes set from array.  This constructor does not copy the
	 * array directly as it does not know if elements in given array
	 * are ordered and unique.
	 *
	 * \param[in] array pointer to the first element of array
	 * \param[in] num number of elements in array
	 */
	Set(const T *array, unsigned num) {
		for (; num; --num) add(*array++);
	}



	/**
	 * Removes all elements from set.
	 */
	void clear() { arr.clear(); }

	/**
	 * Checks if set is empty.
	 *
	 * \return \c true if set is empty, \c false otherwise
	 */
	bool isEmpty() const { return !arr.size(); }

	/**
	 * Returns number of elements.
	 *
	 * \return number of elements in set
	 */
	unsigned size() const { return arr.size(); }



	/**
	 * Checks if element exists.
	 *
	 * \param[in] element element to search for
	 * \return \c true if \a element exists in set, \c false otehrwise
	 */
	bool    exists(const T &element) const {
		unsigned dummy;
		return find(element, dummy);
	}

	/**
	 * Adds element to set.
	 *
	 * \param[in] element element to add to set
	 * \return reference to this object
	 */
	Set<T, cmp> &add (const T &element) {
		unsigned pos;
		if (!find(element, pos)) {
			arr.insert(pos, element);
		}
		return *this;
	}

	/**
	 * Removes element from set.
	 *
	 * \param[in] element element to remove from set
	 * \return reference to this object
	 */
	Set<T, cmp> &sub (const T &element) {
		unsigned pos;
		if (find(element, pos)) {
			arr.remove(pos);
		}
		return *this;
	}



	/**
	 * Calculates union of two sets.
	 *
	 * \param[in] set Set to add to this Set
	 * \return reference to this object
	 */
	Set<T, cmp> &add(const Set<T, cmp> &set);

	/**
	 * Calculates complement of two sets.
	 *
	 * \param[in] set Set to substract from this Set
	 * \return reference to this object
	 */
	Set<T, cmp> &sub(const Set<T, cmp> &set);

	/**
	 * Calculates intersection of two sets.
	 *
	 * \param[in] set Set to calculate intersection with this Set
	 * \return reference to this object
	 */
	Set<T, cmp> &mul(const Set<T, cmp> &set);

	/**
	 * Calculates symetric difference of two sets.
	 *
	 * \param[in] set Set to symetric difference with this Set
	 * \return reference to this object
	 */
	Set<T, cmp> &diff(const Set<T, cmp> &set);



	/**
	 * Checks if this set is subset of given set.
	 *
	 * \param[in] set Set to test for being subset
	 * \return \c true if this Set is subset of \a set
	 */
	bool    isSubsetOf        (const Set<T, cmp> &set) const;

	/**
	 * Checks if this set is proepr subset of given set.
	 *
	 * \param[in] set Set to test for being proper subset
	 * \return \c true if this Set is proper subset of \a set
	 */
	bool    isProperSubsetOf  (const Set<T, cmp> &set) const {
		return size()<set.size() && isSubsetOf(set);
	}

	/**
	 * Checks if this set is superset of given set.
	 *
	 * \param[in] set Set to test for being suberset
	 * \return \c true if this Set is superset of \a set
	 */
	bool    isSupersetOf      (const Set<T, cmp> &set) const {
		return set.isSubsetOf(*this);
	}

	/**
	 * Checks if this set is proper superset of given set.
	 *
	 * \param[in] set Set to test for being proper suberset
	 * \return \c true if this Set is proper superset of \a set
	 */
	bool    isProperSupersetOf(const Set<T, cmp> &set) const {
		return set.isProperSubsetOf(*this);
	}

	/**
	 * Checks if sets are equal.
	 *
	 * \param[in] set Set to test for equality
	 * \return \c true if this Set is equal to this set
	 */
	bool    eq                (const Set<T, cmp> &set) const {
		return arr == set.arr;
	}

	/**
	 * Checks if sets are not equal
	 *
	 * \param[in] set Set to test for equal
	 * \return \c true if this Set is not equal to this Set
	 */
	bool    neq               (const Set<T, cmp> &set) const {
		return !eq(set);
	}



	/**
	 * Adds element to set.
	 *
	 * \param[in] element element to add to set
	 * \return reference to this object
	 */
	Set<T, cmp> &operator+=(const T &element) { return this->add (element); }

	/**
	 * Removes element from set.
	 *
	 * \param[in] element element to remove from set
	 * \return reference to this object
	 */
	Set<T, cmp> &operator-=(const T &element) { return this->sub (element); }



	/**
	 * Calculates union of two sets.
	 *
	 * \param[in] set Set to add to this Set
	 * \return reference to this object
	 */
	Set<T, cmp> &operator+=(const Set<T, cmp> &set) {return this->add (set);}

	/**
	 * Calculates complement of two sets.
	 *
	 * \param[in] set Set to substract from this Set
	 * \return reference to this object
	 */
	Set<T, cmp> &operator-=(const Set<T, cmp> &set) {return this->sub (set);}

	/**
	 * Calculates intersection of two sets.
	 *
	 * \param[in] set Set to calculate intersection with this Set
	 * \return reference to this object
	 */
	Set<T, cmp> &operator*=(const Set<T, cmp> &set) {return this->mul (set);}

	/**
	 * Calculates symetric difference of two sets.
	 *
	 * \param[in] set Set to symetric difference with this Set
	 * \return reference to this object
	 */
	Set<T, cmp> &operator^=(const Set<T, cmp> &set) {return this->diff(set);}



	/**
	 * Checks if set is not empty.
	 *
	 * \return \c true if set is not empty
	 */
	operator bool () {
		return arr.size();
	}

	/**
	 * Checks if set is empty.
	 *
	 * \return \c true if set is empty
	 */
	bool operator!() {
		return !arr.size();
	}



	/**
	 * Returns pointer to the "first" element in set.  Elements are
	 * ordered in linear order defined by \a cmp template argument but
	 * you should not relay on that.
	 *
	 * \return pointer to the "first" element of set
	 * \sa end()
	 */
	const T *begin() const { return arr.begin(); }

	/**
	 * Returns pointer to the one-past the "last" element in set.
	 * Elements are ordered in linear order defined by \a cmp template
	 * argument but you should not relay on that.
	 *
	 * \return pointer to the one-past the "last" element of set
	 * \sa begin()
	 */
	const T *end() const { return arr.end(); }

	/**
	 * Returns Array holding all elements from set Elements are
	 * ordered in linear order defined by \a cmp template argument but
	 * you should not relay on that.
	 *
	 * \return Array with all elements
	 * \sa begin(), end()
	 */
	const Array<T> &getArray() const { return arr; }



	/**
	 * Removes all elements for which \a grp says to.
	 *
	 * \param[in] grp instance of Array::Grep class with overriden ()
	 *                operator which says which elements shall be
	 *                kept.
	 */
	void grep(const typename Array<T>::Grep &grp) {
		arr.grep(grp);
	}
};



/******************** Implementation ******************************/
/**
 * Creates new set which contains all elements from \a set and \a
 * element.
 *
 * \param[in] set set to add element to
 * \param[in] element element to add
 * \return new Set as described
 */
template<class T, class cmp>
inline Set<T, cmp> operator+ (const Set<T, cmp> &set, const T &element) {
	return Set<T, cmp>(set).add (element);
}

/**
 * Creates new set which contains all elments from \a set bug \a
 * element.
 *
 * \param[in] set set to remove element from
 * \param[in] element element to remove
 * \return new Set as described
 */
template<class T, class cmp>
inline Set<T, cmp> operator- (const Set<T, cmp> &set, const T &element) {
	return Set<T, cmp>(set).sub (element);
}



/**
 * Calculates Creates new set which is union of two given sets.
 *
 * \param[in] a the frist Set
 * \param[in] b the second Set
 * \return new Set as described
 */
template<class T, class cmp>
inline Set<T, cmp> operator+ (const Set<T, cmp> &a, const Set<T, cmp> &b) {
	return Set<T, cmp>(a).add (b);
}

/**
 * Calculates Creates new set which is complement of two given sets.
 *
 * \param[in] a the frist Set
 * \param[in] b the second Set
 * \return new Set as described
 */
template<class T, class cmp>
inline Set<T, cmp> operator- (const Set<T, cmp> &a, const Set<T, cmp> &b) {
	return Set<T, cmp>(a).sub (b);
}

/**
 * Calculates Creates new set which is intersection of two given sets.
 *
 * \param[in] a the frist Set
 * \param[in] b the second Set
 * \return new Set as described
 */
template<class T, class cmp>
inline Set<T, cmp> operator* (const Set<T, cmp> &a, const Set<T, cmp> &b) {
	return Set<T, cmp>(a).mul (b);
}

/**
 * Calculates symetric Creates new set which is difference of two
 * given sets.
 *
 * \param[in] a the frist Set
 * \param[in] b the second Set
 * \return new Set as described
 */
template<class T, class cmp>
inline Set<T, cmp> operator^ (const Set<T, cmp> &a, const Set<T, cmp> &b) {
	return Set<T, cmp>(a).diff(b);
}



/**
 * Sends elements of Set to output stream.  Separates elements by
 * single space character.
 *
 * \param[in] os output stream to send data to
 * \param[in] set set to send
 * \return \a os
 */
template<class T, class cmp>
inline std::ostream &operator<<(std::ostream &os, const Set<T, cmp> &set) {
	return os << set.getArray();
}



/**
 * Checks if sets are equal.
 *
 * \param[in] s1 the first Set to test for equality
 * \param[in] s2 the second Set to test for equality
 * \return \c true if both sets are equal
 */
template<class T, class cmp>
bool operator==(const Set<T, cmp> &s1, const Set<T, cmp> &s2) {
	return s1.eq(s2);
}

/**
 * Checks if sets are not equal.
 *
 * \param[in] s1 the first Set to test for equality
 * \param[in] s2 the second Set to test for equality
 * \return \c true if sets are not equal
 */
template<class T, class cmp>
bool operator!=(const Set<T, cmp> &s1, const Set<T, cmp> &s2) {
	return s1.neq(s2);
}



/********** Finds element in array **********/
template<class T, class cmp>
bool Set<T, cmp>::find(const T &element, unsigned &pos) const {
	const T *data = arr.begin();
	unsigned from = 0, to = arr.size();
	while (from<to) {
		const unsigned middle = (from + to) >> 1;
		if (cmp::eq(data[middle], element)) {
			pos = middle;
			return true;
		} else if (cmp::gt(data[middle], element)) {
			to = middle;
		} else {
			from = middle + 1;
		}
	}
	pos = from;
	return false;
}



/********** Union **********/
template<class T, class cmp>
Set<T, cmp> &Set<T, cmp>::add (const Set<T, cmp> &set) {
	if (set.isEmpty()) {
		return *this;
	} else if (isEmpty()) {
		arr = set.arr;
		return *this;
	}

	const T *s_begin = set.arr.begin(), *const s_end = set.arr.end();
	const T *data = arr.begin();
	unsigned pos = 0, _size = arr.size();

	do {
		while (pos<_size && cmp::gt(*s_begin, data[pos])) ++pos;
		if (pos==_size) break;
		if (!cmp::eq(*s_begin, data[pos])) {
			arr.insert(pos, *s_begin);
			data = arr.begin();
			++_size;
		}
		++pos;
	} while (++s_begin!=s_end);

	if (s_begin!=s_end) {
		arr.enlargeBy(s_end - s_begin);
		T *it = arr.rw_begin() + pos;
		do {
			*it++ = *s_begin++;
		} while (s_begin!=s_end);
	}

	return *this;
}



/********** Complement **********/
template<class T, class cmp>
Set<T, cmp> &Set<T, cmp>::sub (const Set<T, cmp> &set) {
	if (isEmpty() || set.isEmpty()) {
		return *this;
	}

	const T *s_begin = set.arr.begin(), *const s_end = set.arr.end();
	T *it = arr.rw_begin();
	const T *data = arr.begin(), *const data_end = arr.end();

	do {
		if (cmp::gt(*data, *s_begin)) {
			++s_begin;
		} else if (cmp::eq(*data, *s_begin)) {
			++s_begin;
			++data;
		} else {
			if (it!=data) *it = *data;
			++it;
			++data;
		}
	} while (s_begin!=s_end && data!=data_end);

	if (it==data) return *this;

	while (data!=data_end) {
		*it++ = *data++;
	}

	arr.shrinkBy(data_end - it);
	return *this;
}



/********** Intersection **********/
template<class T, class cmp>
Set<T, cmp> &Set<T, cmp>::mul (const Set<T, cmp> &set) {
	if (isEmpty() || set.isEmpty()) {
		arr.clear();
		return *this;
	}

	const T *s_begin = set.arr.begin(), *const s_end = set.arr.end();
	T *it = arr.rw_begin();
	const T *data = arr.begin(), *const data_end = arr.end();

	do {
		if (cmp::gt(*data, *s_begin)) {
			++s_begin;
		} else if (cmp::eq(*data, *s_begin)) {
			if (it!=data) *it = *data;
			++it;
			++s_begin;
			++data;
		} else {
			++data;
		}
	} while (s_begin!=s_end && data!=data_end);

	arr.shrinkBy(data_end - it);
	return *this;
}



/********** Symetric difference **********/
template<class T, class cmp>
Set<T, cmp> &Set<T, cmp>::diff(const Set<T, cmp> &set) {
	if (set.isEmpty()) {
		return *this;
	} else if (isEmpty()) {
		return *this = set;
	}

	Array<T> tmp;

	const T *s_begin = set.arr.begin(), *const s_end = set.arr.end();
	const T *data = arr.begin(), *const data_end = arr.end();

	do {
		if (cmp::gt(*data, *s_begin)) {
			tmp.push(*s_begin++);
		} else if (cmp::eq(*data, *s_begin)) {
			++s_begin;
			++data;
		} else {
			tmp.push(*data++);
		}
	} while (s_begin!=s_end && data!=data_end);

	if (s_begin!=s_end) {
		unsigned _size = tmp.size();
		tmp.enlargeBy(s_end - s_begin);
		T *it = tmp.rw_begin() + _size;
		do {
			*it++ = *s_begin++;
		} while (s_begin!=s_end);
	} else if (data!=data_end) {
		unsigned _size = tmp.size();
		tmp.enlargeBy(data_end - data);
		T *it = tmp.rw_begin() + _size;
		do {
			*it++ = *data++;
		} while (data!=data_end);
	}

	arr = tmp;
	return *this;
}



/********** Comparision - is subset **********/
template<class T, class cmp>
bool    Set<T, cmp>::isSubsetOf        (const Set<T, cmp> &set) const {
	if (isEmpty()) {
		return true;
	} else if (set.size() < size()) {
		return false;
	}

	const T *s = set.arr.begin(), *d = arr.begin(), *const limit = arr.end();
	if (set.size() == size()) {
		while (d!=limit && cmp::eq(*s, *d)) ++s, ++d;
		return d==limit;
	}

	const T *const s_limit = set.arr.end();
	do {
		while (s!=s_limit && cmp::gt(*d, *s)) ++s;
		if (s==s_limit || !cmp::eq(*d, *s)) {
			return false;
		}
		++s;
		++d;
	} while (d!=limit);

	return true;
}



}

#endif
