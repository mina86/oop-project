/**
 * \file
 * Array Header File.
 * $Id: array.hpp,v 1.12 2006/12/01 22:25:47 mina86 Exp $
 * Copyright 2006 by Michal Nazarewicz (mina86/AT/mina86.com)
 * Licensed under the Academic Free License version 2.1
 */

#ifndef MN_ARRAY_HPP
#define MN_ARRAY_HPP

#include <stdexcept>

#include "policy-compare.hpp"

namespace mina86 {


/**
 * Class representing a dynamic array which growns and shrinks when
 * elements arr added or removed.  This array is implemented in such a
 * way that tables with the same content may share memory.  This helps
 * minimizing memory usage but may cause some bugs, ie. in the
 * following code:
 *
 * \code
 * Array<int> foo;
 * foo.push(10);
 * int *first = foo.rw_begin();
 * Array<int> bar(foo);
 * *first = 20;
 * int baz = bar.at(0);
 * \endcode
 *
 * \a baz will most likelly have the value \c 20 instead of \c 10.  In
 * particular, iterators get invalidated when Array is modified but
 * also when another Array is assigned value of oryginal Array.
 */
template<class T>
class Array {

	/**
	 * Class which accualy stores the elements.
	 */
	class Data {
		/**
		 * Number of Array clases referring to this Data class.
		 */
		unsigned references;


		/**
		 * Destructor.
		 */
		~Data() {
			delete[] data;
		}

	public:

		/**
		 * Pointer to allocated memory.
		 */
		T *data;

		/**
		 * Number of elements.
		 */
		unsigned size;

		/**
		 * Number of slots allocates.
		 */
		unsigned capacity;


		/**
		 * Default constructor.
		 *
		 * \warning All constructors sets reference counter to zero.
		 *          You need to use inc() to increase it.
		 */
		Data() : references(0), data(0), size(0), capacity(0) { }

		/**
		 * Constructor allocating initial memory.
		 *
		 * \warning All constructors sets reference counter to zero.
		 *          You need to use inc() to increase it.
		 *
		 * \param[in] c initial capacity.
		 */
		Data(unsigned c) : references(0), data(0), size(0), capacity(c) {
			if (c) data = new T[c];
		}


		/**
		 * Copy constructor.
		 *
		 * \param[in] d data object to copy
		 */
		Data(const Data &d)
			: references(0), data(0), size(d.size), capacity(d.capacity) {
			if (!capacity) return;
			data = new T[capacity];
			copy(d.data, d.size);
		}


		/**
		 * Increments references counter.  This should be the first
		 * thing done after constructing a new Data object.
		 *
		 * \param[in] d pointer to data object
		 * \sa dec()
		 */
		static void inc(Data *d) {
			++d->references;
		}

		/**
		 * Decrements references counter.  Deletes data object if
		 * reference counter reaches zero.
		 *
		 * \param[in] d pointer to data object
		 * \sa inc()
		 */
		static void dec(Data *d) {
			if (!--d->references) delete d;
		}


		/**
		 * Copy elements and sets size.  This method does not check if
		 * there is enough allocated memory.  The new size is just \a s.
		 *
		 * \param[in] array pointer to first element of array
		 * \param[in] s number of elements to copy
		 * \sa copyAt()
		 */
		void copy(const T *array, unsigned s) {
			copyAt(0, array, s);
		}

		/**
		 * Copy elements at given location and sets size.  This method
		 * does not check if there is enough allocated memory.  The
		 * new size is just \a s + \a idx.
		 *
		 * \param[in] idx index at which elements should be copied
		 * \param[in] array pointer to first element of array
		 * \param[in] s number of elements to copy
		 * \sa copy()
		 */
		void copyAt(int idx, const T *array, unsigned s) {
			size = s + idx;
			for (T *d = data + idx; s; --s) *d++ = *array++;
		}

		/**
		 * Checks if only one Array object refers to this object.
		 *
		 * \return \c true if reference counter equals one
		 */
		bool isOwn() { return references == 1; }
	};


	/**
	 * Pointer to Data object holding Array's elements.
	 */
	Data *data;


	/**
	 * Creates new Data object if reference counter is more then one.
	 */
	inline void makeOwn() {
		if (!data->isOwn()) {
			Data::dec(data);
			Data::inc(data = new Data(*data));
		}
	}


public:
	/**
	 * Exception thrown by Array class in some situations when user
	 * specifies index which is out of bounds.
	 */
	class BadIndex : public std::exception {
		/**
		 * Reference to Array object
		 */
		const Array<T> &array;

		/**
		 * Requested index.
		 */
		unsigned index;

	public:
		/**
		 * Constructs exception class.
		 *
		 * \param[in] arr Array object
		 * \param[in] idx requested index
		 */
		BadIndex(const Array<T> &arr, unsigned idx)
			: array(arr), index(idx) { }

		/**
		 * Copy constructor.
		 *
		 * \param[in] e object to copy
		 */
		BadIndex(const BadIndex &e)
			: std::exception(), array(e.array), index(e.index) { }

		/**
		 * Returns reference to Array object which thrown exception.
		 *
		 * \return reference to Array object
		 */
		const Array<T> &getArray() const { return array; }


		/**
		 * Returns position which caused excetion.
		 */
		unsigned getIndex() const { return index; }
	};



	/**
	 * Default constructor.
	 *
	 * \param[in] capacity initial capacity of the Array
	 */
	Array(unsigned capacity = 0) {
		data = new Data(capacity);
		Data::inc(data);
	}

	/**
	 * Copy constructor.
	 *
	 * \param[in] array Array object to copy
	 */
	Array(const Array &array) {
		data = array.data;
		Data::inc(data);
	}

	/**
	 * Destructor.
	 */
	~Array() {
		Data::dec(data);
	}


	/**
	 * Returns size of the Array.
	 *
	 * \return number of elements in Array
	 */
	unsigned size() const { return data->size; }

	/**
	 * Returns capacity of the Array.
	 *
	 * \return number of allocated slots
	 */
	unsigned getCapacity() const { return data->capacity; }


	/**
	 * Copies data from another Array.
	 *
	 * \param[in] array Array to copy
	 * \sa set(const T*, unsigned, unsigned)
	 */
	void set(const Array<T> &array) {
		if (array.data!=data) {
			Data::dec(data);
			Data::inc(data = array.data);
		}
	}

	/**
	 * Copies data from array.
	 *
	 * \param[in] array pointer to the first element
	 * \param[in] _size numebr of elements
	 * \param[in] capacity desired capacity of Array
	 * \sa set_no_copy(const T*, unsigned, unsigned), set(const Array<T>&)
	 */
	void set(const T *array, unsigned _size, unsigned capacity = 0) {
		reserve(capacity < _size ? _size+8 : capacity);
		makeOwn();
		data->copy(array, _size);
	}

	/**
	 * Sets data.  This does not copy elements from array given as a
	 * pointer but rather uses memory pointed by given pointer to
	 * store data.
	 *
	 * Caller must ensure that this arrays capacity is at * least \a s
	 * and at least \a c.  Moreover, he or she must not * assume that
	 * the class will really use this memory as it cen ba * freed soon
	 * after calling this method and then memory pointed by * given
	 * pointer will be unusable.
	 *
	 * \param[in] arrays pointer to allocated memory
	 * \param[in] _size number of elements
	 * \param[in] capacity number of allocated slots
	 * \sa set(const T*, unsigned, unsigned)
	 */
	void set_no_copy(T *arrays, unsigned _size, unsigned capacity = 0) {
		if (data->isOwn()) {
			delete[] data->data;
		} else {
			Data::dec(data);
			Data::inc(data = new Data());
		}
		data->data = arrays;
		data->size = _size;
		data->capacity = capacity<_size ? _size+8 : capacity;
	}


	/**
	 * Adds element at the end of Array.
	 *
	 * \param[in] element element to add
	 * \sa pop(), unshift()
	 */
	void push(const T &element) {
		insert(data->size, element);
	}

	/**
	 * Returns and removes element at the end of Array.  If array is
	 * empty BadIndex is thrown.
	 *
	 * \return element from the end of array
	 * \sa pop(T&), peek(), push(), shift()
	 */
	T pop() {
		if (!data->size) throw BadIndex(*this, 0);
		T element(data->data[data->size-1]);
		shrinkBy(1);
		return element;
	}

	/**
	 * Returns and removes element at the end of Array.  If array is
	 * empty does not alter argument and returns \c false.
	 *
	 * \param[out] element object to save element from the end
	 * \return \c true if there was an element and \c false if array
	 *         was empty
	 * \sa pop(), peek(T&), push(), shift()
	 */
	bool pop(T &element) {
		if (!data->size) return false;
		element = data->data[data->size-1];
		shrinkBy(1);
		return true;
	}

	/**
	 * Returns element at the end of Array.  If array is empty
	 * BadIndex is thrown.
	 *
	 * \return element from the end of array
	 * \sa peek(T&), pop(), push()
	 */
	const T &peek() {
		if (!data->size) throw BadIndex(*this, 0);
		return data->data[data->size-1];
	}

	/**
	 * Returns element at the end of Array.  If array is empty does
	 * not alter argument and returns \c false.
	 *
	 * \param[out] element object to save element from the end
	 * \return \c true if there was an element and \c false if array
	 *         was empty
	 * \sa shift(), push()
	 */
	bool peek(T &element) {
		if (!data->size) return false;
		element = data->data[data->size-1];
		return true;
	}


	/**
	 * Inserts element at the beginning of Array.
	 *
	 * \param[in] element element to insert
	 * \sa unshift(), shift(), pop()
	 */
	void unshift(const T &element) {
		insert(0, element);
	}

	/**
	 * Returns and removed element from the beginning of Array.  If
	 * array is empty BadIndex is thrown.
	 *
	 * \return the frist element of the Array
	 * \sa unshift(T&), shift(), pop()
	 */
	T shift() {
		if (!data->size) throw BadIndex(*this, 0);
		T element(*data->data);
		remove(0);
		return element;
	}

	/**
	 * Returns and removes element from the beginning of Array.  If
	 * array is empty does not alter argument and returns \c false.
	 *
	 * \param[out] element object to save element from the beginning
	 * \return \c true if there was an element and \c false if array
	 *         was empty
	 */
	bool shift(T &element) {
		if (!data->size()) return false;
		element = *data->data;
		remove(0);
		return true;
	}



	/**
	 * Inserts element at given position.  If \a pos is greater then
	 * Array's size (ie. it isn't a valid index nor it points at the
	 * end of Array) BadIndex will be thrown.
	 *
	 * \param[in] pos position to insert at
	 * \param[in] element element to insert
	 * \sa remove()
	 */
	void insert(unsigned pos, const T &element);

	/**
	 * Removes element at given position.  If \a order is \c false,
	 * order of elements in Array may not be preserved after the
	 * element was removed.
	 *
	 * If \a pos is not a valid index BadIndex will be thrown.
	 * And yes - this method does not throw an exception if \a pos
	 * equals Array's size (ie. points to one past the last element).
	 *
	 * \param[in] pos position to remove
	 * \param[in] order whether to preserve order of elements
	 * \sa insert()
	 */
	void remove(unsigned pos, bool order = true);



	/**
	 * Resizes Array.  Changes Array's size to \a size.  If new size
	 * is larger then the old one elements with unspecified value are
	 * added at the end.  If it's smaller elements from the end are
	 * removed.  This function also ensures that Array's capacity is
	 * at least \a capacity but no more then \c capacity+256.
	 *
	 * If \a capacity is lower then \a size, current capacity is
	 * compared against \a size and if there are more then \c 256
	 * unused slots or there is not enough space array's capacity
	 * becomes \c size+8.
	 *
	 * \param[in] size new size of the Array
	 * \param[in] capacity desired capacity of the Array
	 */
	void resize  (unsigned size, unsigned capacity = 0);

	/**
	 * Changes Array's capacity.  Ensures that Array's capacity is at
	 * least \a capacity and no more then \c capacity+256.
	 *
	 * This is good if you know in advance how many elements there
	 * will be in the Array.  Note that removing elements will
	 * decrease capacity if there are more then 256 unused slots.
	 *
	 * If \a capacity is lower then current size, current capacity is
	 * compared against current size and if there are more then \c 256
	 * unused slots array's capacity becomes current size plus \c 8.
	 *
	 * \param[in] capacity desired capacity of the Array
	 * \sa resize()
	 */
	void reserve (unsigned capacity) {
		resize(data->size, capacity);
	}

	/**
	 * Removes all elements.
	 *
	 * \sa resize()
	 */
	void clear   () {
		resize(0);
	}

	/**
	 * Shrinks Array.  If \a _size is greater then or equal to size of
	 * the Array this method does nothing, otherwise it removes
	 * elements from the end of Array till it's size is \a _size.
	 *
	 * \param[in] _size new size of the Array
	 * \sa resize(), shrinkBy()
	 */
	void shrink  (unsigned _size) {
		if (_size<data->size) {
			resize(_size);
		}
	}

	/*
	 * Shrinks Array.  If \a by is greater then size of the Array this
	 * removes all elements, otherwise it removes \a by elements from
	 * the end.
	 *
	 * \param[in] by number of elements to remove
	 * \sa resize(), shrink()
	 */
	void shrinkBy(unsigned by) {
		if (by>=data->size) {
			clear();
		} else {
			resize(data->size - by);
		}
	}

	/**
	 * Enlarges Array.  If \a _size is less then or equal to size of
	 * the Array this method does nothing, otherwise it adds elements
	 * at the end of Array till it's size is \a _size.
	 *
	 * \param[in] _size new size of the Array
	 * \sa resize(), enlargeBy()
	 */
	void enlrage  (unsigned _size) {
		if (size>data->size) {
			resize(_size);
		}
	}

	/**
	 * Enlarges Array.  Adds \a by elements at the end of the Array.
	 *
	 * \param[in] by number of elements to add
	 * \sa resize(), enlarge()
	 */
	void enlargeBy(unsigned by) {
		resize(data->size + by);
	}


	/**
	 * Returns pointer to the begining of the Array.
	 *
	 * \return pointer to the first element of the Array
	 * \sa end(), rw_begin()
	 */
	const T *begin() const { return data->data; }

	/**
	 * Returns pointer to the end of the Array.
	 *
	 * \return pointer to "one past" the last element of the Array
	 * \sa begin(), rw_end()
	 */
	const T *end  () const { return data->data + data->size; }

	/**
	 * Returns pointer to the begining of the Array.  This returns a
	 * non-const pointer and thereofer you can use it to modify data
	 * in Array.  However, if you don't intend to use begin() instead
	 * since this method has to ensure that it doesn't share data with
	 * any other Array object which means that it may have to copy all
	 * elements to another location which may take some time.
	 *
	 * \return pointer to the first element of the Array
	 * \sa rw_end(), begin()
	 */
	T *rw_begin() {
		makeOwn();
		return data->data;
	}

	/**
	 * Returns pointer to the end of the Array.  See notes regarding
	 * rw_begin().
	 *
	 * \return pointer to "one past" the last element of the Array
	 * \sa rw_begin(), end()
	 */
	T *rw_end  () {
		makeOwn();
		return data->data;
	}


	/**
	 * Returns element at given potision.  If invalid index was given
	 * this method throws BadIndex.
	 *
	 * \param[in] pos index of an element
	 * \return element at index \a pos
	 */
	const T &at(unsigned pos) {
		if (pos>=data->size) throw BadIndex(*this, pos);
		return data->data[pos];
	}


	/**
	 * Returns element at given potision.  If invalid index was given
	 * this method throws BadIndex.
	 *
	 * \param[in] pos index of an element
	 * \return element at index \a pos
	 */
	T &rw_at(unsigned pos) {
		if (pos>=data->size) throw BadIndex(*this, pos);
		makeOwn();
		return data->data[pos];
	}


	/**
	 * Sets element at given potision.  If invalid index was given
	 * this method throws BadIndex.
	 *
	 * \param[in] pos index of an element
	 * \param[in] element new value
	 * \return reference to changed element
	 */
	T &at(unsigned pos, const T &element) {
		if (pos>=data->size) throw BadIndex(*this, pos);
		makeOwn();
		return data->data[pos] = element;
	}


	/**
	 * Swaps object at given positions.  If invalid index was given
	 * this method throws BadIndex.
	 *
	 * \param[in] pos1 index of first element
	 * \param[in] pos2 index of second element
	 */
	void swap(unsigned pos1, unsigned pos2) {
		if (pos1>=data->size) throw BadIndex(*this, pos1);
		if (pos2>=data->size) throw BadIndex(*this, pos2);
		makeOwn();
		T tmp(data->data[pos1]);
		data->data[pos1] = data->data[pos2];
		data->data[pos2] = tmp;
	}


	/**
	 * Reverses order of elements in array.  The first element becomes
	 * the last one and so on.
	 */
	void reverse() {
		if (!data->size) return;
		makeOwn();
		T *d = data->data;
		for (unsigned i = 0, j = data->size - 1; i<j; ++i, --j) {
			T tmp(d[i]);
			d[i] = d[j];
			d[j] = tmp;
		}
	}


	/**
	 * Abstract structure used for grepping values in array.
	 */
	struct Grep {
		/**
		 * Says whether given element should be kept in the array.  If
		 * it returns \c false it will be removed.
		 *
		 * \param[in] element element to test
		 * \return \c true if element should stay, \c false otherwise
		 */
		virtual bool keep(const T &element) = 0;

		/**
		 * A shorthand for keep() method.
		 *
		 * \param[in] element element to test
		 * \return \c true if element should stay, \c false otherwise
		 */
		bool operator()(const T &element) {
			return keep(element);
		}
	};


	/**
	 * Removes all elements for which \a grp says to.
	 *
	 * \param[in] grp instance of Grep class with overriden ()
	 *                operator which says which elements shall be
	 *                kept.
	 */
	void grep(const Grep *grp) {
		if (!data->size) return;
		makeOwn();
		T *it = data->data;
		const T *el = data-data, *const limit = data->data + data->size;
		do {
			if (!grp(*el)) continue;
			if (it!=el) *it = *el;
			++it;
		} while (++el!=data);
		resize(it - data->data);
	}


	/**
	 * Returns element at given position.
	 *
	 * \param[in] pos index of an element
	 * \return element at index \a pos
	 */
	const T &operator[](unsigned pos) const {
		return data->data[pos];
	}


	/**
	 * Assigns Array object.
	 *
	 * \param[in] array Array object to assing
	 */
	Array &operator=(const Array &array) {
		set(array);
		return *this;
	}



	/**
	 * Checks if Array is not empty.
	 *
	 * \return \c true if array is not empty, \c false otherwise
	 */
	operator bool() const { return size(); }

	/**
	 * Checks if Array is empty.
	 *
	 * \return \c true if array is empty, \c false otherwise
	 */
	bool operator !() const { return !size(); }


	/**
	 * Compare policy for Array objects.  Linear order of Array
	 * objects is defined as follows: one array is greater then or
	 * equal to another iff they are equal or the first on is longer
	 * then the other or they are of equal length and there exists
	 * such index \a i that all elements priori to this position are
	 * equal in both arraies and element at given index in the first
	 * position is greater then element at given index in the second
	 * array, ie:
	 *
	 * \code
	 * ( x >= y )  <=>
	 * ( ( x.size() > y.size() ) or
	 *   ( ( x.size() == y.size() ) and
	 *     ( ( for all i  x[i] == y[i] ) or
	 *       ( exists i such that
	 *         (x[i] > k[i]) and (for all k from 0 to i-1  x[k] == y[k])
	 *       )
	 *     )
	 *   )
	 * )
	 * \endcode
	 */
	template<class cmp = default_cmp_policy<T> >
	struct cmp_policy {
		static const bool ordered = cmp::ordered;

		/**
		 * Checks if both arguments are equal.
		 *
		 * \param[in] a first object
		 * \param[in] b second object
		 * \return \c true if both objects are equal
		 */
		static bool eq(const Array<T> &a, const Array<T> &b) {
			if (a.data == b.data) return true;
			if (a.size() != b.size()) return false;
			const T *e1 = a.begin(), *e2 = b.begin(), *const end = a.end();
			while (e1!=end && cmp::eq(*e1, *e2)) ++e1, ++e2;
			return e1==end;
		}

		/**
		 * Compares two objects.
		 *
		 * \param[in] a first object
		 * \param[in] b second object
		 * \return \c true if first object is greater then on equal to the
		 *         second
		 */
		static bool ge(const Array<T> &a, const Array<T> &b) {
			if (a.size()<b.size()) return false;
			if (a.size>b.size()) return true;
			if (a.data == b.data) return true;

			const T *e1 = a.begin(), *e2 = b.begin(), *const end = a.end();
			while (e1!=end && cmp::eq(*e1, *e2)) ++e1, ++e2;
			return e1==end || cmp::gt(*e1, *e1);
		}

		/**
		 * Compares two objects.
		 *
		 * \param[in] a first object
		 * \param[in] b second object
		 * \return \c true if first object is greater then the second
		 */
		static bool gt(const T &a, const T &b) {
			if (a.size()<b.size()) return false;
			if (a.size>b.size()) return true;
			if (a.data == b.data) return false;

			const T *e1 = a.begin(), *e2 = b.begin(), *const end = a.end();
			while (e1!=end && cmp::eq(*e1, *e1)) ++e1, ++e2;
			return e1!=end && cmp::gt(*e1, *e2);
		}
	};

};


/**
 * Sends elements of Array to output stream.  Separates elements by
 * single space character.
 *
 * \param[in] os output stream to send data to
 * \param[in] a  an array to send
 * \return \a os
 */
template<class T>
std::ostream &operator<<(std::ostream &os, const Array<T> &a) {
	const T *begin = a.begin(), *const end = a.end();
	if (begin!=end) {
		for (os << *begin; ++begin!=end; os << ' ' << *begin);
	}
	return os;
}



/**
 * Specialization of \a is_ordered struct saing \a Array<T> has a
 * linear order if \a T has a linear order.
 */
template<class T>
struct is_ordered< Array<T> > {
	/**
	 * Field says that \a Array<T> has linear order if \a T has a
	 * linear order.
	 */
	static const bool v = is_ordered<T>::v;
};



/**
 * Specialization of \a default_cmp_policy structure for \a Array<T>
 * type.
*/
template<class T>
struct default_cmp_policy< Array<T> >
	: public Array<T>::template cmp_policy<> { };



/**
 * Checks if arraies are equal.  Uses default_cmp_policy< Array<T> >
 * to compare elements.
 *
 * \param[in] a the first Array to test for equality
 * \param[in] b the second Array to test for equality
 * \return \c true if both arraies are equal, \c false otherwise
 */
template<class T>
bool operator==(const Array<T> &a, const Array<T> &b) {
	return default_cmp_policy<Array<T> >::eq(a, b);
}


/**
 * Checks if arraies are not equal.  Uses default_cmp_policy< Array<T> >
 * to compare elements.
 *
 * \param[in] a the first Array to test for equality
 * \param[in] b the second Array to test for equality
 * \return \c true if arraies are not equal, \c false otherwise
 */
template<class T>
bool operator!=(const Array<T> &a, const Array<T> &b) {
	return !default_cmp_policy<Array<T> >::eq(a, b);
}






/******************** Implementation ********************/
template<class T>
void Array<T>::insert(unsigned pos, const T &element) {
	if (pos>data->size) {
		throw BadIndex(data->size, pos);
	}

	unsigned capacity = data->capacity;
	if (data->size>=capacity) {
		capacity = capacity>30 ? capacity + capacity / 2 : 32;
	}

	if (!data->isOwn()) {
		Data *d = new Data(capacity);
		const unsigned _size = data->size;
		d->copyAt(0, data->data, pos);
		d->copyAt(pos, &element, 1);
		d->copyAt(pos + 1, data->data + pos, _size - pos);
		Data::dec(data);
		Data::inc(data = d);
	} else if (data->capacity>=capacity) {
		T *el = data->data + data->size;
		for (unsigned left = data->size - pos; left; --el, --left) {
			*el = *(el - 1);
		}
		*el = element;
		++data->size;
	} else {
		T *d = data->data;
		data->data = new T[capacity];
		data->capacity = capacity;
		const unsigned _size = data->size;
		data->copyAt(0, d, pos);
		data->copyAt(pos, &element, 1);
		data->copyAt(pos + 1, d + pos, _size - pos);
		delete[] d;
	}
}



template<class T>
void Array<T>::remove(unsigned pos, bool order) {
	if (pos>=data->size) {
		throw BadIndex(data->size, pos);
	} else if (!data->isOwn()) {
		Data *d = new Data(data->capacity + 31);
		const unsigned _size = data->size;
		d->copyAt(0, data->data, pos);
		d->copyAt(pos, data->data + pos + 1, _size - pos - 1);
		Data::dec(data);
		Data::inc(data = d);
	} else if (data->capacity - data->size >= 256) {
		T *d = data->data;
		const unsigned _size = data->size;
		data->data = new T[_size + 31];
		data->capacity = _size + 31;
		data->copyAt(0, d, pos);
		data->copyAt(pos, d + pos + 1, _size - pos - 1);
		delete[] d;
	} else if (pos == --data->size) {
		/* nothing */
	} else if (!order) {
		data->data[pos] = data->data[data->size];
	} else {
		T *el = data->data + pos, *const last = data->data + data->size;
		for (; el!=last; ++el) {
			el[0] = el[1];
		}
	}
}



template<class T>
void Array<T>::resize(unsigned _size, unsigned capacity) {
	if (_size==data->size && data->capacity >= capacity
	    && data->capacity <= capacity + 256
	    && data->capacity <= data->size + 256) {
		return;
	}

	if (capacity<_size) {
		capacity = _size+8;
	}

	if (!data->isOwn()) {
		Data *d = new Data(capacity);
		d->copy(data->data, _size <= data->size ? _size : data->size);
		d->size = _size;
		Data::dec(data);
		Data::inc(data = d);
	} else if (capacity>=data->capacity || capacity+256<=data->capacity) {
		T *d = data->data;
		data->data = new T[capacity];
		data->capacity = capacity;
		data->copy(d, _size <= data->size ? _size : data->size);
		data->size = _size;
		delete[] d;
	} else {
		data->size = _size;
	}
}



}
#endif
