/**
 * \file
 * Relation Header File.
 * $Id: relation.hpp,v 1.5 2006/12/18 19:59:30 mina86 Exp $
 * Copyright 2006 by Michal Nazarewicz (mina86/AT/mina86.com)
 * Licensed under the Academic Free License version 2.1
 */

#ifndef MN_RELATION_HPP
#define MN_RELATION_HPP

#include <stdexcept>

#include "set.hpp"
#include "pair.hpp"


namespace mina86 {


/**
 * Class holding relation flags used by Relation template as base
 * class.
 */
class RelationFlags {
protected:
	/**
	 * Default constructor made protected to make class abstract.
	 */
	RelationFlags() { }


	/**
	 * Flags of properties relation is in.
	 *
	 * \sa #checked
	 */
	mutable unsigned short flags;

	/**
	 * Flags specifing which properties were tested and therefore
	 * which bits of #flags are meaningful.  This property is zeroed
	 * whenever Relation::X, Relation::Y or Relation::G is changed.
	 *
	 * \sa #flags
	 */
	mutable unsigned short checked;

	/**
	 * Values of flags.
	 *
	 * \sa #flags
	 */
	enum Flag {
		FL_SAME_SETS     = 1 <<  0,  //!< Domain equals codomain.

		FL_LEFT_TOTAL    = 1 <<  1,  //!< Relation is left total.
		FL_RIGHT_TOTAL   = 1 <<  2,  //!< Relation is right total.

		FL_FUNCTIONAL    = 1 <<  3,  //!< Relation is functional.
		FL_INJECTIVE     = 1 <<  4,  //!< Relation is injective.
		FL_BIJECTIVE     = 1 <<  5,  //!< Relation is bijective

		FL_REFLEXIVE     = 1 <<  6,  //!< Relation is reflexive.
		FL_IRREFLEXIVE   = 1 <<  7,  //!< Relation is irreflexive.
		FL_COREFLEXIVE   = 1 <<  8,  //!< Relation is coreflexive.
		FL_SYMMETRIC     = 1 <<  9,  //!< Relation is symmetric.
		FL_ANTISYMMETRIC = 1 << 10,  //!< Relation is antisymmetric.
		FL_ASYMMETRIC    = 1 << 11,  //!< Relation is asymmetric.
		FL_TRANSITIVE    = 1 << 12,  //!< Relation is transitive.
		FL_LINEAR        = 1 << 13,  //!< Relation is linear.
		FL_TRICHOTOMOUS  = 1 << 14,  //!< Relation is trichotomous.
		FL_EUCLIDEAN     = 1 << 15   //!< Relation is euclidean.
	};


	/**
	 * Array with saved implication rules.  Element
	 * <code>flags_impl_array[n][s][0]</code> talls which flags are
	 * implied to be set if flag <code>1 << n</code> is in state \a s
	 * (\c 1 for set, \c 0 for unset).  Element
	 * <code>flags_impl_array[n][s][1]</code> talls which flags are
	 * implied to be unset if flag <code>1 << n</code> is in state \a
	 * s (\c 1 for set, \c 0 for unset).
	 *
	 * \warning This array is only valid if Relation::X, Relation::Y
	 *          and Relation::G are all non empty.
	 */
	static const unsigned short flags_impl_array[16][2][2];


	/**
	 * Sets or unsets given flag.  Sets given bit (or bits) in
	 * #checked and depending on \a state sets or zeros it (them) in
	 * #flags.
	 *
	 * This mathod makes use of #flags_impl_array which specifies
	 * which properties are implied by given flag and sets or unsets
	 * them as well.
	 *
	 * \param[in] flag flag to set or unset (this has to be single flag)
	 * \param[in] state whether to set flag
	 * \return \a state
	 */
	bool setFlag(Flag flag, bool state = true) const {
		unsigned num = 0;
		for (unsigned f = flag; f != 1; f >>= 1) ++num;
		flags |=  flags_impl_array[num][state][0];
		flags &= ~flags_impl_array[num][state][1];
		checked |= flags_impl_array[num][state][0] |
		           flags_impl_array[num][state][1];
		return state;
	}


	/**
	 * Unsets given flag.  Sets given bit (or bits) in #checked and
	 * zeros it (them) in #flags.  This method calls \a setFlag() so
	 * it in fact changes other flags as well.
	 *
	 * \param[in] flag flag to zero
	 * \sa setFlag()
	 */
	void unsetFlag(Flag flag) const { setFlag(flag, 0); }
};


const unsigned short RelationFlags::flags_impl_array[16][2][2] = {
	/* FL_SAME_SETS */
	{ { 0, FL_SAME_SETS    }, { FL_SAME_SETS    , 0 } },

	/* FL_LEFT_TOTAL    */
	{
		{ 0, FL_LEFT_TOTAL | FL_REFLEXIVE },
		{ FL_LEFT_TOTAL, 0 }
	},
	/* FL_RIGHT_TOTAL   */
	{
		{ 0, FL_RIGHT_TOTAL | FL_REFLEXIVE },
		{ FL_RIGHT_TOTAL, 0 }
	},

	/* FL_FUNCTIONAL    */
	{
		{ 0, FL_FUNCTIONAL | FL_COREFLEXIVE },
		{ FL_FUNCTIONAL, 0 }
	},
	/* FL_INJECTIVE     */
	{
		{ 0, FL_INJECTIVE | FL_COREFLEXIVE },
		{ FL_INJECTIVE, 0 }
	},
	/* FL_BIJECTIVE     */
	{ { 0, FL_BIJECTIVE    }, { FL_BIJECTIVE    , 0 } },

	/* FL_REFLEXIVE     */
	{
		{ 0, FL_REFLEXIVE },
		{ FL_REFLEXIVE | FL_LEFT_TOTAL | FL_RIGHT_TOTAL,
		  FL_IRREFLEXIVE | FL_ASYMMETRIC | FL_TRICHOTOMOUS }
	},
	/* FL_IRREFLEXIVE   */
	{
		{ 0, FL_IRREFLEXIVE  },
		{ FL_IRREFLEXIVE, FL_REFLEXIVE | FL_COREFLEXIVE }
	},
	/* FL_COREFLEXIVE   */
	{
		{ 0, FL_COREFLEXIVE },
		{ FL_COREFLEXIVE | FL_FUNCTIONAL | FL_INJECTIVE, 0 }
	},
	/* FL_SYMMETRIC     */
	{
		{ 0, FL_SYMMETRIC },
		{ FL_SYMMETRIC, FL_ASYMMETRIC }
	},
	/* FL_ANTISYMMETRIC */
	{ { 0, FL_ANTISYMMETRIC}, { FL_ANTISYMMETRIC, 0 } },
	/* FL_ASYMMETRIC    */
	{
		{ 0, FL_ASYMMETRIC },
		{ FL_ASYMMETRIC, FL_REFLEXIVE | FL_COREFLEXIVE | FL_SYMMETRIC }
	},
	/* FL_TRANSITIVE    */
	{ { 0, FL_TRANSITIVE   }, { FL_TRANSITIVE   , 0 } },
	/* FL_LINEAR        */
	{
		{ 0, FL_LINEAR },
		{ FL_LINEAR, FL_TRANSITIVE }
	},
	/* FL_TRICHOTOMOUS  */
	{
		{ 0, FL_TRICHOTOMOUS },
		{ FL_TRICHOTOMOUS, FL_REFLEXIVE | FL_COREFLEXIVE | FL_LINEAR }
	},
	/* FL_EUCLIDEAN     */
	{ { 0, FL_EUCLIDEAN    }, { FL_EUCLIDEAN    , 0 } },
};



/**
 * Class template representing a binary relation.  \a T1 and \a T2
 * template arguments specify the types of elements of the first and
 * the second set on which the relation is to be defined.  \a cmp1 and
 * \a cmp2 specifies policies for compering those elements and havee
 * to define a linear order.
 *
 * Quoting Wikipedia:
 *
 * A binary relation R is usually defined as an ordered triple (X, Y,
 * G) where X and Y are arbitrary sets, and G is a subset of the
 * Cartesian product X x Y.  The sets X and Y are called the domain
 * and codomain, respectively, of the relation, and G is called its
 * graph.
 *
 * The statement (x,y) exists in G is read "x is R-related to y", and
 * is denoted by xRy or R(x,y).  The latter notation corresponds to
 * viewing R as the characteristic function of the set of pairs G.
 *
 * The order of the elements in each pair of G is important: if a != b,
 * then aRb and bRa can be true or false, independently of each other.
 */
template<class T1, class T2 = T1,
         class cmp1 = default_cmp_policy<T1>,
         class cmp2 = default_cmp_policy<T2> >
class Relation : public RelationFlags {
	/**
	 * Just an alias.
	 */
	typedef Relation<T1, T2, cmp1, cmp2> Myself;


public:
	/**
	 * Type of elements kept in the graph of the relation.  Those are
	 * ordered pairs which left element is of type \a T1, and right of
	 * type \a T2.
	 */
	typedef Pair<T1, T2> ThePair;

	/**
	 * The type of the graph.
	 */
	typedef Set<ThePair, typename ThePair::template cmp_policy<cmp1, cmp2> > Graph;

	/**
	 * The type of the domain.
	 */
	typedef Set<T1, cmp1> Domain;

	/**
	 * The type of the codomain.
	 */
	typedef Set<T2, cmp2> Codomain;



	/**
	 * The base class of exceptions thrown by Relation's methods.
	 */
	class Exception : public std::exception {
		/**
		 * Reference to the instance of Relation which caused the
		 * exception.
		 */
		const Myself &relation;

		/**
		 * The default constructor.  It's private to disallow creating
		 * "default" exception class without reference set.
		 */
		Exception() { }

	protected:
		/**
		 * Constructs Exception class and sets the reference.  It's
		 * protected to disallow creating instances of this class.
		 * Only instances of derived class are allowed.
		 */
		Exception(const Myself &rel) : relation(rel) { }

	public:
		/**
		 * Returns Relation object which has thrown the exception.
		 *
		 * \return reference to Relation object
		 */
		const Myself &getRelation() const { return relation; }
	};


	/**
	 * Exception class representing invalid value of element or set.
	 * This exception means that user tried to add a pair to the graph
	 * such that it's left (right) element does not exist in domain
	 * (codomain) or set domain (codomain) to a value such that there
	 * is a pair in graph which left (right) element does not exist in
	 * given new doamin (codomain).
	 *
	 * This is an abstract object which is only the base of two other
	 * possible BadValue objects (ie. one with template argument given
	 * as \a T1 and another as \a T2).
	 */
	class BadPairBase : public Exception {
		ThePair pair;

	protected:
		/**
		 * Constructs exception.  It's protected to disallow creating
		 * instances of this class.  Only instances of derived class
		 * are allowed.
		 */
		BadPairBase(const Myself &rel, const ThePair &p)
			: Exception(rel), pair(p) { }

	public:
		/**
		 * Returns pair which caused problems.
		 *
		 * \return pair which caused problems
		 */
		const ThePair &getPair() const { return pair; }

		~BadPairBase() throw() { }
	};


	/**
	 * Exception class representing invalid value of element.  This
	 * exception means that user tried to add a pair to the graph such
	 * that it's left (right) element does not exist in domain
	 * (codomain).
	 */
	class BadPair : public BadPairBase {
	public:
		/**
		 * Constructs exception.
		 */
		BadPair(const Myself &rel, const ThePair &p) : BadPairBase(rel, p) { }
	};


	/**
	 * Exception class representing invalid set.  This happens when
	 * user tried to set domain (codomain) such that there is a pair
	 * in graph such that it's left (right) element does not exist in
	 * given domain (codomain).
	 */
	template<class T, class cmp>
	class BadSet : public BadPairBase {
		/**
		 * Set which caused exception.
		 */
		const Set<T, cmp> &set;

	public:
		/**
		 * Constructs exception.
		 */
		BadSet(const Myself &rel, const ThePair &p, const Set<T, cmp> &s)
			: BadPairBase(rel, p), set(s) { }

		/**
		 * Returns set which cause exception.
		 *
		 * \return reference to \a T2 object
		 */
		const T &getSet() const { return set; }
	};


	/**
	 * Exception class meaning operation which required domain and
	 * codomain to be equal was requested but the sets were not equal.
	 */
	class BadSets : public Exception {
	public:
		BadSets(const Myself &rel) : Exception(rel) { }
	};



private:
	/**
	 * The domain of the relation.
	 */
	Domain X;

	/**
	 * The codomain of the relation.
	 */
	Codomain Y;

	/**
	 * The graph of the relation.
	 */
	Graph G;



protected:
	/**
	 * Checks whether given pair is valid.  Pair is invalid if either
	 * it's left element does not exist in domain, or it's right
	 * element does not exist in codomain.
	 *
	 * \throw BadPair pair was found invalid.
	 *
	 * \param[in] pair pair to check
	 */
	void checkPair(const ThePair &pair) const {
		if (!X.exists(pair.left()) || !Y.exists(pair.right())) {
			throw BadPair(*this, pair);
		}
	}

	/**
	 * Checks all given pairs.  Simply calls checkPair() for each
	 * element from given set.
	 *
	 * \throw BadPair any of the pairs in graph were found invalid.
	 *
	 * \param[in] graph set of pairs to check.
	 */
	void checkGraph(const Graph &graph) const {
		const ThePair *p = graph.begin(), *const limit = graph.end();
		for (; p!=limit; ++p) {
			checkPair(*p);
		}
	}

	/**
	 * Compares the domain and codomain of the relation.
	 *
	 * \throw BadSets domain does not equal codomain.
	 */
	void checkSetsAreEqual() const {
		if (checked & FL_SAME_SETS) {
			if (!(flags & FL_SAME_SETS)) throw BadSets(*this);
		} else if (X!=Y) {
			throw BadSets(*this);
		} else {
			checked |= FL_SAME_SETS;
			flags   |= FL_SAME_SETS;
		}
	}


public:
	/**
	 * Constructs empty relation with empty domain and codomain.
	 */
	Relation() : X(), Y(), G() { }

	/**
	 * Copy constructor.
	 *
	 * \param[in] rel object to copy
	 */
	Relation(const Myself &rel)
		: RelationFlags(), X(rel.X), Y(rel.Y), G(rel.G) {}

	/**
	 * Constructs new relation.
	 *
	 * \throw BadPair \a g is not subset of Cartesian product
	 *                \a x * \a y.
	 *
	 * \param[in] x domain of the relation
	 * \param[in] y codomain of the relation
	 * \param[in] g graph of the relation
	 */
	Relation(const Domain &x, const Codomain &y, const Graph &g)
		: X(x), Y(y), G(g) {
		checkGraph();
	}


	/**
	 * Returns domain of the relation.
	 *
	 * \return domain of the relation
	 * \sa setDomain(), getCodomain()
	 */
	const Domain &getDomain() const { return X; }

	/**
	 * Sets domain of the relation.
	 *
	 * \throw BadSet there is a pair in graph such that it's left
	 *               element does not exist in \a x.
	 *
	 * \param[in] x the new domain
	 * \sa getDomain(), cutToDomain(), setCodomain()
	 */
	void setDomain(const Domain &x);

	/**
	 * Sets domain of the relation.  All pairs in graph which left
	 * element does not exist in \a x are removed.
	 *
	 * \param[in] x the new domain
	 * \sa getDomain(), setDomain(), cutToCodomain()
	 */
	void cutToDomain(const Domain &x);



	/**
	 * Returns codomain of the relation.
	 *
	 * \return codomain of the relation
	 * \sa setCodomain(), getDomain()
	 */
	const Codomain &getCodomain() const { return Y; }

	/**
	 * Sets codomain of the relation.
	 *
	 * \throw BadSet there is a pair in graph such that it's right
	 *               element oes not exist in \a y.
	 *
	 * \param[in] x the new codomain
	 * \sa getCodomain(), cutToCodomain(), setDomain()
	 */
	void setCodomain(const Codomain &x);

	/**
	 * Sets domain of the relation.  All pairs in graph which right
	 * element does not exist in \a x are removed.
	 *
	 * \param[in] x the new codomain
	 * \sa getCodomain(), setCodomain(), cutToDomain()
	 */
	void cutToCodomain(const Codomain &x);



	/**
	 * Returns graph of the relation.
	 *
	 * \return grapg of the relation
	 * \sa setGraph(), begin(), end(), getArray()
	 */
	const Graph &getGraph() const { return G; }

	/**
	 * Sets graph of the relation.
	 *
	 * \throw BadPair \a graph contains pair such that it's left
	 *                element does not exist in domain, or right in
	 *                codomain.
	 *
	 * \param[in] graph the new graph
	 * \sa getGraph()
	 */
	void setGraph(const Graph &graph) {
		checkGraph(graph);
		checked = 0;
		G = graph;
	}



	/**
	 * Removes all element from graph of the relation.
	 */
	void clear() { G.clear(); checked = 0; }

	/**
	 * Checks if relation's graph is empty.
	 *
	 * \return \c true if relation's graph is empty, \c false
	 *         otherwise
	 */
	bool isEmpty() const { return G.isEmpty(); }

	/**
	 * Returns number of elements in relation's graph.
	 *
	 * \return number of elements in relation's graph
	 */
	unsigned size() const { return G.size(); }



	/**
	 * Checks if given pair exists in relation's graph.  If pair
	 * exists the it's left and right element are in relation.
	 *
	 * \param[in] pair pair to search for
	 * \return \c true if given pair is in graph, \c false othrwise
	 * \sa exists(const T1&, const T2&), Set::exists()
	 */
	bool exists(const ThePair &pair) const { return G.exists(pair); }

	/**
	 * Checks if given pair exists in relation's graph.  If pair (\a
	 * left, \a right) exists in relations'graph then objects \a left
	 * and \a right are in relation.
	 *
	 * \param[in] left left element of the pair
	 * \param[in] right right element of the pair
	 * \return \c true if pair (\a left, \a right) exists in graph,
	 *         \c false otherwise
	 * \sa exists(const ThePair&), Set::exists()
	 */
	bool exists(const T1 &left, const T2 &right) const {
		return G.exists(ThePair(left, right));
	}



	/**
	 * Adds given pair to graph.  In other words makes \a pair's left
	 * and right element be in relation.
	 *
	 * \throw BadPair \a pair's left element does not exist in domain
	 *                or it's right element does not exist in
	 *                codomain.
	 *
	 * \param[in] pair pair to add
	 * \return reference to this object
	 * \sa add(const T1&, const T2&), Set::add(const T&)
	 */
	Myself &add (const ThePair &pair) {
		checkPair(pair);
		G.add(pair);
		checked = 0;
		return *this;
	}

	/**
	 * Adds given pair to graph.  In other words makes \a left and \a
	 * right element be in relation.
	 *
	 * \throw BadPair \a left does not exist in domain or \a right
	 *                does not exist in codomain.
	 *
	 * \param[in] left left element of pair
	 * \param[in] right right element of pair
	 * \return reference to this object
	 * \sa add(const ThePair&), Set::add(const T&)
	 */
	Myself &add (const T1 &left, const T2 &right) {
		return add(ThePair(left, right));
	}


	/**
	 * Removes given pair from graph.  In other words makes \a pair's
	 * left and right element no longer be in relation.
	 *
	 * \param[in] pair pair to remove
	 * \return reference to this object
	 * \sa sub(const T1&, const T2&), Set::sub(const T&)
	 */
	Myself &sub (const ThePair &pair) {
		G.sub(pair);
		checked = 0;
		return *this;
	}

	/**
	 * Removes given pair from graph.  In other words makes \a left
	 * and \a right element no longer be in relation.
	 *
	 * \param[in] left left element of pair
	 * \param[in] right right element of pair
	 * \return reference to this object
	 * \sa sub(const ThePair&), Set::sub(const T&)
	 */
	Myself &sub (const T1 &left, const T2 &right) {
		return sub(ThePair(left, right));
	}



	/**
	 * Calculates union of given graph and relation's graph.  Modifies
	 * this relation in such a way that elements x, y are in relation
	 * if they were in the original relation or pair (x, y) exists in
	 * \a graph.
	 *
	 * \note Relation class has a conversion to Graph so one can pass
	 *       directly Relation class as argument of this method.
	 *
	 * \throw BadPair \a graph contains pair which left element does
	 *                not exist in domain or right element does not
	 *                exist in codomain.
	 *
	 * \param[in] graph graph to add
	 * \return reference to this object
	 * \sa Set::add()
	 */
	Myself &add (const Graph &graph) {
		checkGraph(graph);
		G.add (graph);
		checked = 0;
		return *this;
	}

	/**
	 * Calculates complement of given graph and relation's graph.
	 * Modifies this relation in such a way that elements x, y are in
	 * relation if they were in the original relation and pair (x, y)
	 * does not exists in \a graph.
	 *
	 * \note Relation class has a conversion to Graph so one can pass
	 *       directly Relation class as argument of this method.
	 *
	 * \param[in] graph graph to substract
	 * \return reference to this object
	 * \sa Set::sub()
	 */
	Myself &sub (const Graph &graph) {
		G.sub (graph);
		checked = 0;
		return *this;
	}

	/**
	 * Calculates intersection of given graph and relation's graph.
	 * Modifies this relation in such a way that elements x, y are in
	 * relation if they were in the original relation and pair (x, y)
	 * does exists in \a graph.
	 *
	 * \note Relation class has a conversion to Graph so one can pass
	 *       directly Relation class as argument of this method.
	 *
	 * \param[in] graph graph to intersect with
	 * \return reference to this object
	 * \sa Set::mul()
	 */
	Myself &mul (const Graph &graph) {
		G.mul (graph);
		checked = 0;
		return *this;
	}

	/**
	 * Calculates symetric difference of given graph and relation's
	 * graph.  Modifies this relation in such a way that elements x, y
	 * are in relation if they were in the original relation or pair
	 * (x, y) exists in \a graph but not both at the same time.
	 *
	 * \note Relation class has a conversion to Graph so one can pass
	 *       directly Relation class as argument of this method.
	 *
	 * \throw BadPair \a graph contains pair which left element does
	 *                not exist in domain or right element does not
	 *                exist in codomain.
	 *
	 * \param[in] graph graph to add
	 * \return reference to this object
	 * \sa Set::diff()
	 */
	Myself &diff(const Graph &graph) {
		checkGraph(graph);
		G.diff(graph);
		checked = 0;
		return *this;
	}


	/**
	 * Returns image of given element.  Returns a set of elements that
	 * given element is in relation with, ie. set of elements \a y
	 * such that xRy.
	 *
	 * \param[in] x element to search image of
	 * \return image of element \a x
	 * \sa preimage()
	 */
	Set<T2, cmp2> image(const T1 &x) const;

	/**
	 * Returns preimage of given element.  Returns a set of elements that
	 * are in relation with given element, ie. set of elements \a x
	 * such that xRy.
	 *
	 * \param[in] y element to search preimage of
	 * \return preimage of element \a y
	 * \sa image()
	 */
	Set<T1, cmp1> preimage(const T2 &y) const;



	/**
	 * Adds given pair to graph.  In other words makes \a pair's left
	 * and right element be in relation.
	 *
	 * \throw BadPair \a pair's left element does not exist in domain
	 *                or it's right element does not exist in
	 *                codomain.
	 *
	 * \param[in] pair pair to add
	 * \return reference to this object
	 * \sa add(const ThePair&)
	 */
	Myself &operator+=(const ThePair &pair) { return this->add (pair); }

	/**
	 * Removes given pair from graph.  In other words makes \a pair's
	 * left and right element no longer be in relation.
	 *
	 * \param[in] pair pair to remove
	 * \return reference to this object
	 * \sa sub(const ThePair&)
	 */
	Myself &operator-=(const ThePair &pair) { return this->sub (pair); }


	/**
	 * Calculates union of given graph and relation's graph.  Modifies
	 * this relation in such a way that elements x, y are in relation
	 * if they were in the original relation or pair (x, y) exists in
	 * \a graph.
	 *
	 * \note Relation class has a conversion to Graph so one can pass
	 *       directly Relation class as argument of this method.
	 *
	 * \throw BadPair \a graph contains pair which left element does
	 *                not exist in domain or right element does not
	 *                exist in codomain.
	 *
	 * \param[in] graph graph to add
	 * \return reference to this object
	 * \sa add(const Graph&)
	 */
	Myself &operator+=(const Graph &graph) { return this->add (graph); }

	/**
	 * Calculates complement of given graph and relation's graph.
	 * Modifies this relation in such a way that elements x, y are in
	 * relation if they were in the original relation and pair (x, y)
	 * does not exists in \a graph.
	 *
	 * \note Relation class has a conversion to Graph so one can pass
	 *       directly Relation class as argument of this method.
	 *
	 * \param[in] graph graph to substract
	 * \return reference to this object
	 * \sa sub(const Graph&)
	 */
	Myself &operator-=(const Graph &graph) { return this->sub (graph); }

	/**
	 * Calculates intersection of given graph and relation's graph.
	 * Modifies this relation in such a way that elements x, y are in
	 * relation if they were in the original relation and pair (x, y)
	 * does exists in \a graph.
	 *
	 * \note Relation class has a conversion to Graph so one can pass
	 *       directly Relation class as argument of this method.
	 *
	 * \param[in] graph graph to intersect with
	 * \return reference to this object
	 * \sa mul()
	 */
	Myself &operator*=(const Graph &graph) { return this->mul (graph); }

	/**
	 * Calculates symetric difference of given graph and relation's
	 * graph.  Modifies this relation in such a way that elements x, y
	 * are in relation if they were in the original relation or pair
	 * (x, y) exists in \a graph but not both at the same time.
	 *
	 * \note Relation class has a conversion to Graph so one can pass
	 *       directly Relation class as argument of this method.
	 *
	 * \throw BadPair \a graph contains pair which left element does
	 *                not exist in domain or right element does not
	 *                exist in codomain.
	 *
	 * \param[in] graph graph to add
	 * \return reference to this object
	 * \sa diff()
	 */
	Myself &operator^=(const Graph &graph) { return this->diff(graph); }


	/**
	 * Returns image of given element.  Returns a set of elements that
	 * given element is in relation with, ie. set of elements \a y
	 * such that xRy.
	 *
	 * \param[in] x element to search image of
	 * \return image of element \a x
	 * \sa image()
	 */
	Set<T2, cmp1> operator[](const T1 &x) const { return image(x); }


	/**
	 * Checks if given pair exists in relation's graph.  If pair
	 * exists the it's left and right element are in relation.
	 *
	 * \param[in] pair pair to search for
	 * \return \c true if given pair is in graph, \c false othrwise
	 * \sa exists(const ThePair&)
	 */
	bool operator()(const ThePair &pair) const { return exists(pair); }

	/**
	 * Checks if given pair exists in relation's graph.  If pair (\a
	 * left, \a right) exists in relations'graph then objects \a left
	 * and \a right are in relation.
	 *
	 * \param[in] left left element of the pair
	 * \param[in] right right element of the pair
	 * \return \c true if pair (\a left, \a right) exists in graph,
	 *         \c false otherwise
	 * \sa exists(const ThePair&), Set::exists()
	 */
	bool operator()(const T1 &left, const T2 &right) const {
		return exists(left, right);
	}


	/**
	 * Returns graph of the relation.
	 *
	 * \return grapg of the relation
	 * \sa getGraph()
	 */
	operator const Graph&() const { return G; }


	/**
	 * Cehcks if graph is not empty.
	 *
	 * \return \c true if graph is not empty, \c false otherwise
	 */
	operator bool () const { return  G.size(); }

	/**
	 * Cehcks if graph is empty.
	 *
	 * \return \c true if graph is empty, \c false otherwise
	 */
	bool operator!() const { return !G.size(); }



	/**
	 * Checks if relation is left-total.  Relation is left-total if for
	 * all x in X there exists a y in Y such that xRy.
	 *
	 * \return \c true if relation is left-total, \c false otherwise
	 */
	bool isLeftTotal() const;

	/**
	 * Checks if relation is right-total.  Relation is right-total if
	 * for all y in Y there exists an x in X such that xRy.
	 *
	 * \return \c true if relation is right-total, \c false otherwise
	 */
	bool isRightTotal() const;


	/**
	 * Checks if relation is functional.  Relation is functional if
	 * for all x in X, and y and z in Y it holds that if xRy and xRz
	 * then y = z.
	 *
	 * \return \c true if relation is functional, \c false otherwise
	 */
	bool isFunctional() const;

	/**
	 * Checks if relation is surjective.  Relation is surjective if
	 * for all y in Y there exists an x in X such that xRy.
	 *
	 * \return \c true if relation is surjective, \c false otherwise
	 */
	bool isSurjective() const { return isRightTotal(); }

	/**
	 * Checks if relation is injective.  Relation is injective if for
	 * all x and z in X and y in Y it holds that if xRy and zRy then x
	 * = z.
	 *
	 * \return \c true if relation is injective, \c false otherwise
	 */
	bool isInjective() const;

	/**
	 * Checks if relation is bijective.  Relation is bijective if
	 * relation is left-total, right-total and functional.
	 *
	 * \return \c true if relation is bijective, \c false otherwise
	 */
	bool isBijective() const {
		return X.size()==Y.size() && X.size()==G.size() &&
			isLeftTotal() && isRightTotal() && isFunctional();
	}

	/**
	 * Checks if relation is reflexive.  Relation is reflexive if for
	 * all x in X it holds that xRx. For example, "greater than or
	 * equal to" is a reflexive relation but "greater than" is not.
	 *
	 * \return \c true if relation is reflexive, \c false otherwise
	 */
	bool isReflexive() const;

	/**
	 * Checks if relation is irreflexive.  Relation is irreflexive if
	 * for all x in X it holds that not xRx. "Greater than" is an
	 * example of an irreflexive relation.
	 *
	 * \return \c true if relation is irreflexive, \c false otherwise
	 */
	bool isIrreflexive() const;

	/**
	 * Checks if relation is coreflexive.  Relation is coreflexive if
	 * for all x and y in X it holds that if xRy then x = y.
	 *
	 * \return \c true if relation is coreflexive, \c false otherwise
	 */
	bool isCoreflexive() const;

	/**
	 * Checks if relation is symmetric.  Relation is symmetric if for
	 * all x and y in X it holds that if xRy then yRx. "Is a blood
	 * relative of" is a symmetric relation, because x is a blood
	 * relative of y if and only if y is a blood relative of x.
	 *
	 * \return \c true if relation is symmetric, \c false otherwise
	 */
	bool isSymmetric() const;

	/**
	 * Checks if relation is antisymmetric.  Relation is antisymmetric
	 * if for all x and y in X it holds that if xRy and yRx then x =
	 * y. "Greater than or equal to" is an antisymmetric relation,
	 * because if x>=y and y>=x, then x=y.
	 *
	 * \return \c true if relation is antisymmetric, \c false otherwise
	 */
	bool isAntisymmetric() const;

	/**
	 * Checks if relation is asymmetric.  Relation is asymmetric if
	 * for all x and y in X it holds that if xRy then not
	 * yRx. "Greater than" is an asymmetric relation, because if x>y
	 * then not y>x.
	 *
	 * \return \c true if relation is asymmetric, \c false otherwise
	*/
	bool isAsymmetric() const;

	/**
	 * Checks if relation is transitive.  Relation is transitive if
	 * for all x, y and z in X it holds that if xRy and yRz then
	 * xRz. "Is an ancestor of" is a transitive relation, because if x
	 * is an ancestor of y and y is an ancestor of z, then x is an
	 * ancestor of z.
	 *
	 * \return \c true if relation is transitive, \c false otherwise
	 */
	bool isTransitive() const;

	/**
	 * Checks if relation is total.  Relation is total if for all x
	 * and y in X it holds that xRy or yRx (or both). "Is greater than
	 * or equal to" is an example of a total relation (this definition
	 * for total is different from the one in the previous section).
	 *
	 * \return \c true if relation is total, \c false otherwise
	 */
	bool isTotal() const { return isLinear(); }

	/**
	 * Checks if relation is linear.  Relation is linear if for all x
	 * and y in X it holds that xRy or yRx (or both). "Is greater than
	 * or equal to" is an example of a total relation (this definition
	 * for total is different from the one in the previous section).
	 *
	 * \return \c true if relation is linear, \c false otherwise
	 */
	bool isLinear() const;

	/**
	 * Checks if relation is trichotomous.  Relation is trichotomous
	 * if for all x and y in X exactly one of xRy, yRx or x = y
	 * holds. "Is greater than" is an example of a trichotomous
	 * relation.
	 *
	 * \return \c true if relation is trichotomous, \c false otherwise
	 */
	bool isTrichotomous() const;

	/**
	 * Checks if relation is euclidean.  Relation is euclidean if for
	 * all x, y and z in X it holds that if xRy and xRz, then yRz.
	 *
	 * \return \c true if relation is euclidean, \c false otherwise
	 */
	bool isEuclidean() const;

	/**
	 * Checks if relation is extendable.  Relation is extendable if
	 * for all x in X, there exists y in X such that xRy. "Is greater
	 * than" is an extendable relation on the integers. But it is not
	 * an extendable relation on the positive integers, because there
	 * is no y in the positive integers such that 1>y.
	 *
	 * \return \c true if relation is extendable, \c false otherwise
	 */
	bool isExtendable() const { return isSerial(); }

	/**
	 * Checks if relation is serial.  Relation is serial if for all x
	 * in X, there exists y in X such that xRy. "Is greater than" is
	 * an extendable relation on the integers. But it is not an
	 * extendable relation on the positive integers, because there is
	 * no y in the positive integers such that 1>y.
	 *
	 * \return \c true if relation is serial, \c false otherwise
	 */
	bool isSerial() const { return isLeftTotal(); }



	/**
	 * Checks if relation is preorder.  Relation is preorder if
	 * relation is reflexive and transitive.
	 *
	 * \return \c true if relation is preorder, \c false otherwise
	 */
	bool isPreOrder() const { return isQuasiOrder(); }

	/**
	 * Checks if relation is quasiorder.  Relation is quasiorder if
	 * relation is reflexive and transitive.
	 *
	 * \return \c true if relation is quasiorder, \c false otherwise
	 */
	bool isQuasiOrder() const {
		return isReflexive() && isTransitive();
	}

	/**
	 * Checks if relation is equivalence.  Relation is equivalence if
	 * relation is reflexive, symmetric and transitive.
	 *
	 * \return \c true if relation is equivalence, \c false otherwise
	 */
	bool isEquivalence() const {
		return isQuasiOrder() && isSymmetric();
	}

	/**
	 * Checks if relation is partial order.  Relation is partial order
	 * if relation is reflexive, antisymmetric and transitive.
	 *
	 * \return \c true if relation is partial order, \c false otherwise
	 */
	bool isPartialOrder() const {
		return isQuasiOrder() && isAntisymmetric();
	}

	/**
	 * Checks if relation is total order.  Relation is total order if
	 * relation is partial order which is total.
	 *
	 * \return \c true if relation is total order, \c false otherwise
	 */
	bool isTotalOrder() const { return isLinearOrder(); }

	/**
	 * Checks if relation is linear order.  Relation is linear order
	 * if relation is partial order which is total.
	 *
	 * \return \c true if relation is linear order, \c false otherwise
	 */
	bool isLinearOrder() const { return isPartialOrder() && isTotal(); }



	/**
	 * Returns pointer to the first element of graph.
	 *
	 * \sa end(), getGraph(), getArray(), Set::begin()
	 */
	const ThePair *begin() const { return G.begin(); }

	/**
	 * Returns pointer to the "one past the last" element of graph.
	 *
	 * \sa begin(), getGraph(), getArray(), Set::end()
	 */
	const ThePair *end() const { return G.end(); }

	/**
	 * Returns reference to the graph's array.
	 *
	 * \sa begin(), end(), getGraph(), Set::getArray()
	 */
	const Array<ThePair> &getArray() const { return G.getArray(); }



	/**
	 * Helper class for <code>x&lt;R&gt;y</code> syntax.  Thanks to
	 * this class and proper operators one can type
	 * <code>X&lt;R&gt;y</code> to check if elements \a x and \a y are
	 * in relation \a R.
	 */
	class IsInRelationHelper {
		/**
		 * Reference to the relation.
		 */
		const Myself &relation;

		/**
		 * Reference to the left element.
		 */
		const T1 &left;

		/**
		 * Private constructor to disallow creating instances by not
		 * friends.
		 */
		IsInRelationHelper(const Myself &rel, const T1 &l)
			: relation(rel), left(l) { }

		/** \cond REALLY_ALL */
		IsInRelationHelper(const IsInRelationHelper &helper)
			: relation(helper.relation), left(helper.left) { }
		void operator=(const IsInRelationHelper &helper) { (void)helper; }
		/** \endcond */

	public:
		/**
		 * Checks if two given elements are in relation.  Left element
		 * is #left, relation is #relation and right element is given
		 * argument \a right.
		 *
		 * \param[in] right right element
		 * \return \c true if #left and \a right are in #relation
		 */
		bool operator>(const T2 &right) {
			return relation.exists(left, right);
		}

		template<class fT1, class fT2, class fc1, class fc2>
		friend
		typename Relation<fT1, fT2, fc1, fc2>::IsInRelationHelper
		operator<(const fT1 &left, const Relation<fT1, fT2, fc1, fc2> &rel);
	};
};



/**
 * Creates IsInRelationHelper object which can be used to check if
 * given element is in relation.  This is responsible for the first
 * part of syntax: <code>x&lt;R&gt;y</code>
 *
 * \param[in] left left element
 * \param[in] rel relation
 * \return IsInRelationHelper object
 */
template<class T1, class T2, class cmp1, class cmp2>
typename Relation<T1, T2, cmp1, cmp2>::IsInRelationHelper
operator<(const T1 &left, const Relation<T1, T2, cmp1, cmp2> &rel) {
	return typename Relation<T1, T2, cmp1, cmp2>::IsInRelationHelper(rel, left);
}



template<class T1, class T2, class cmp1, class cmp2>
void Relation<T1, T2, cmp1, cmp2>::setDomain(const Domain &x) {
	const ThePair *p = G.begin(), *const limit = G.end();
	for (; p!=limit; ++p) {
		if (!x.exists(p->left())) {
			throw BadSet<T1, cmp1>(*this, *p, x);
		}
	}
	X = x;
	checked = 0;
}


template<class T1, class T2, class cmp1, class cmp2>
void Relation<T1, T2, cmp1, cmp2>::cutToDomain(const Domain &x) {
	struct grep : public Graph::Grep {
		const Domain &X;
		grep(const Domain &x_) : X(x_) { }
		bool keep(const ThePair &p) {
			return X.exists(p->left());
		}
	} grp(x);
	G.grep(grp);
	X = x;
	checked = 0;
}



template<class T1, class T2, class cmp1, class cmp2>
void Relation<T1, T2, cmp1, cmp2>::setCodomain(const Codomain &y) {
	const ThePair *p = G.begin(), *const limit = G.end();
	for (; p!=limit; ++p) {
		if (!y.exists(p->right())) {
			throw BadSet<T2, cmp2>(*this, *p, y);
		}
	}
	Y = y;
	checked = 0;
}


template<class T1, class T2, class cmp1, class cmp2>
void Relation<T1, T2, cmp1, cmp2>::cutToCodomain(const Codomain &y) {
	struct grep : public Graph::Grep {
		const Codomain &Y;
		grep(const Domain &y_) : Y(y_) { }
		bool keep(const ThePair &p) {
			return Y.exists(p->right());
		}
	} grp(y);
	G.grep(grp);
	Y = y;
	checked = 0;
}




template<class T1, class T2, class cmp1, class cmp2>
Relation<T1, T2, cmp1, cmp2>
operator+(Relation<T1, T2, cmp1, cmp2> &rel, const Pair<T1, T2> &pair) {
	return Relation<T1, T2, cmp1, cmp2>(rel).add(pair);
}

template<class T1, class T2, class cmp1, class cmp2>
Relation<T1, T2, cmp1, cmp2>
operator-(Relation<T1, T2, cmp1, cmp2> &rel, const Pair<T1, T2> &pair) {
	return Relation<T1, T2, cmp1, cmp2>(rel).sub(pair);
}



template<class T1, class T2, class cmp1, class cmp2>
Relation<T1, T2, cmp1, cmp2>
operator+(Relation<T1, T2, cmp1, cmp2> &a,
          const Relation<T1, T2, cmp1, cmp2> &b) {
	return Relation<T1, T2, cmp1, cmp2>(a).add (b);
}


template<class T1, class T2, class cmp1, class cmp2>
Relation<T1, T2, cmp1, cmp2>
operator-(Relation<T1, T2, cmp1, cmp2> &a,
          const Relation<T1, T2, cmp1, cmp2> &b) {
	return Relation<T1, T2, cmp1, cmp2>(a).sub (b);
}


template<class T1, class T2, class cmp1, class cmp2>
Relation<T1, T2, cmp1, cmp2>
operator*(Relation<T1, T2, cmp1, cmp2> &a,
          const Relation<T1, T2, cmp1, cmp2> &b) {
	return Relation<T1, T2, cmp1, cmp2>(a).mul (b);
}


template<class T1, class T2, class cmp1, class cmp2>
Relation<T1, T2, cmp1, cmp2>
operator^(Relation<T1, T2, cmp1, cmp2> &a,
          const Relation<T1, T2, cmp1, cmp2> &b) {
	return Relation<T1, T2, cmp1, cmp2>(a).diff(b);
}



template<class T1, class T2, class cmp1, class cmp2>
Set<T2, cmp2> Relation<T1, T2, cmp1, cmp2>::image(const T1 &x) const {
	Set<T2, cmp2> elements;
	const ThePair *const pairs = G.begin(), *limit = G.end(), *p;

	unsigned found = 0, from = 0, to = limit - pairs;
	while (from<to) {
		const unsigned middle = (from + to) >> 1;
		if (cmp1::eq(pairs[middle].left(), x)) {
			to = middle;
			p = pairs + middle;
			found = 1;
		} else if (cmp1::gt(pairs[middle].left(), x)) {
			to = middle;
		} else {
			from = middle + 1;
		}
	}
	if (!found) {
		return elements;
	}

	while (p!=limit && cmp1::eq(p->left(), x)) {
		elements.add(p->right());
		++p;
	}
	return elements;
}


template<class T1, class T2, class cmp1, class cmp2>
Set<T1, cmp1>
Relation<T1, T2, cmp1, cmp2>::preimage(const T2 &y) const {
	Set<T1, cmp1> elements;
	const ThePair *p = G.begin(), *limit = G.end();
	for (; p!=limit; ++p) {
		if (cmp2::eq(p->right(), y)) {
			elements.add(p->left());
		}
	}
	return elements;
}



/* for all x in X there exists a y in Y such that xRy. */
template<class T1, class T2, class cmp1, class cmp2>
bool Relation<T1, T2, cmp1, cmp2>::isLeftTotal() const {
	if (!X.size()) return true;
	if (checked & FL_LEFT_TOTAL) return flags & FL_LEFT_TOTAL;
	if (G.size() < X.size()) {
		unsetFlag(FL_LEFT_TOTAL);
		return false;
	}

	const ThePair *p = G.begin(), *const limit = G.end();
	const T1 *x = X.begin(), *const x_end = X.end();
	for (; x != x_end && p!=limit; ++x) {
		while (p!=limit && !cmp1::ge(p->left(), *x)) ++p;
		p = cmp1::eq(p->left(), *x) ? p+1 : limit;
	}

	return setFlag(FL_LEFT_TOTAL, p != limit);
}


/* for all y in Y there exists an x in X such that xRy. */
template<class T1, class T2, class cmp1, class cmp2>
bool Relation<T1, T2, cmp1, cmp2>::isRightTotal() const {
	if (!Y.size()) return true;
	if (checked & FL_RIGHT_TOTAL) return flags & FL_RIGHT_TOTAL;

	if (G.size() < Y.size()) {
		unsetFlag(FL_LEFT_TOTAL);
		return false;
	}

	Set<T2, cmp2> y(Y);
	const ThePair *p = G.begin(), *limit = G.end();
	for (; y && p!=limit; ++p) {
		y.sub(p->right());
	}

	return setFlag(FL_RIGHT_TOTAL, !y.size());
}


/* for all x in X, and y and z in Y it holds that if xRy and xRz then
   y = z. */
template<class T1, class T2, class cmp1, class cmp2>
bool Relation<T1, T2, cmp1, cmp2>::isFunctional() const {
	if (!X.size() || !Y.size()) return true;
	if (checked & FL_FUNCTIONAL) return flags & FL_FUNCTIONAL;

	if (G.size() < 2) {
		setFlag(FL_FUNCTIONAL);
		return true;
	} else if (G.size()>X.size()) {
		unsetFlag(FL_FUNCTIONAL);
		return false;
	}

	const ThePair *p = G.begin(), *const limit = G.end();
	while (++p!=limit && !cmp1::eq(p[-1].left(), p[0].left()));

	return setFlag(FL_FUNCTIONAL, p == limit);
}


/* for all x and z in X and y in Y it holds that if xRy and zRy then x
   = z. */
template<class T1, class T2, class cmp1, class cmp2>
bool Relation<T1, T2, cmp1, cmp2>::isInjective() const {
	if (!X.size() || !Y.size()) return true;
	if (checked & FL_INJECTIVE) return flags & FL_INJECTIVE;

	if (G.size() < 2) {
		setFlag(FL_INJECTIVE);
		return true;
	} else if (G.size()>Y.size()) {
		unsetFlag(FL_INJECTIVE);
		return false;
	}

	Set<T2, cmp2> y;
	const ThePair *p = G.begin(), *limit = G.end();
	for (; p!=limit && !y.exists(p->right()); ++p) {
		y.add(p->right());
	}

	return setFlag(FL_INJECTIVE, p==limit);
}



/* for all x in X it holds that xRx. For example, "greater than or
   equal to" is a reflexive relation but "greater than" is not. */
template<class T1, class T2, class cmp1, class cmp2>
bool Relation<T1, T2, cmp1, cmp2>::isReflexive() const {
	if (!X.size() && !Y.size()) return true;
	if (checked & FL_REFLEXIVE) return flags & FL_REFLEXIVE;

	checkSetsAreEqual();
	const ThePair *p = G.begin(), *const limit = G.end();
	ThePair pair;
	const T1 *el = X.begin(), *const e_limit = X.end();
	for (; el!=e_limit; ++el) {
		pair.set(*el, *el);
		while (p!=limit &&
		       ThePair::template cmp_policy<cmp1, cmp2>::gt(pair, *p)) ++p;
		if (p==limit ||
		    !ThePair::template cmp_policy<cmp1, cmp2>::eq(pair, *p)) {
			break;
		}
	}

	return setFlag(FL_REFLEXIVE, el==e_limit);
}


/* for all x in X it holds that not xRx. "Greater than" is an
   example of an irreflexive relation. */
template<class T1, class T2, class cmp1, class cmp2>
bool Relation<T1, T2, cmp1, cmp2>::isIrreflexive() const {
	if (!X.size() && !Y.size()) return true;
	if (checked & FL_IRREFLEXIVE) return flags & FL_IRREFLEXIVE;

	checkSetsAreEqual();
	const ThePair *p = G.begin(), *const limit = G.end();
	while (p!=limit && !cmp1::eq(p->left(), p->right())) ++p;

	return setFlag(FL_IRREFLEXIVE, p==limit);
}


/* for all x and y in X it holds that if xRy then x = y. */
template<class T1, class T2, class cmp1, class cmp2>
bool Relation<T1, T2, cmp1, cmp2>::isCoreflexive() const {
	if (!X.size() && !Y.size()) return true;
	if (checked & FL_COREFLEXIVE) return flags & FL_COREFLEXIVE;

	checkSetsAreEqual();
	const ThePair *p = G.begin(), *limit = G.end();
	for (; p!=limit && cmp1::eq(p->left(), p->right()); ++p);

	return setFlag(FL_COREFLEXIVE, p==limit);
}


/* for all x and y in X it holds that if xRy then yRx. "Is a blood
   relative of" is a symmetric relation, because x is a blood
   relative of y if and only if y is a blood relative of x. */
template<class T1, class T2, class cmp1, class cmp2>
bool Relation<T1, T2, cmp1, cmp2>::isSymmetric() const {
	if (!X.size() && !Y.size()) return true;
	if (checked & FL_SYMMETRIC) return flags & FL_SYMMETRIC;

	checkSetsAreEqual();
	const ThePair *p = G.begin(), *limit = G.end();
	for (; p!=limit && exists(p->right(), p->left()); ++p);

	return setFlag(FL_SYMMETRIC, p==limit);
}


/* for all x and y in X it holds that if xRy and yRx then x =
   y. "Greater than or equal to" is an antisymmetric relation,
   because if x>=y and y>=x, then x=y. */
template<class T1, class T2, class cmp1, class cmp2>
bool Relation<T1, T2, cmp1, cmp2>::isAntisymmetric() const {
	if (!X.size() && !Y.size()) return true;
	if (checked & FL_ANTISYMMETRIC) return flags & FL_ANTISYMMETRIC;

	checkSetsAreEqual();
	const ThePair *p = G.begin(), *limit = G.end();
	while (p!=limit &&
	       (cmp1::eq(p->left(), p->right()) ||
	        !exists(p->right(), p->left()))) ++p;

	return setFlag(FL_ANTISYMMETRIC, p==limit);
}


/* for all x and y in X it holds that if xRy then not
   yRx. "Greater than" is an asymmetric relation, because if x>y
   then not y>x. */
template<class T1, class T2, class cmp1, class cmp2>
bool Relation<T1, T2, cmp1, cmp2>::isAsymmetric() const {
	if (!X.size() && !Y.size()) return true;
	if (checked & FL_ASYMMETRIC) return flags & FL_ASYMMETRIC;

	checkSetsAreEqual();
	const ThePair *p = G.begin(), *limit = G.end();
	for (; p!=limit && !exists(p->right(), p->left()); ++p);

	return setFlag(FL_ASYMMETRIC, p==limit);
}


/* for all x, y and z in X it holds that if xRy and yRz then
   xRz. "Is an ancestor of" is a transitive relation, because if x
   is an ancestor of y and y is an ancestor of z, then x is an
   ancestor of z. */
template<class T1, class T2, class cmp1, class cmp2>
bool Relation<T1, T2, cmp1, cmp2>::isTransitive() const {
	if (!X.size() && !Y.size()) return true;
	if (checked & FL_TRANSITIVE) return flags & FL_TRANSITIVE;

	checkSetsAreEqual();
	const ThePair *const pairs = G.begin(), *p = G.begin(),
		*limit = G.end();
	const unsigned number = limit - pairs;
	for (; p!=limit; ++p) {
		const T2 &y = p->right();

		/* Search for the first pair that left == p->right */
		unsigned found = 0, pos, from = 0, to = number;
		while (from<to) {
			const unsigned middle = (from + to) >> 1;
			if (cmp1::eq(pairs[middle].left(), y)) {
				to = pos = middle;
				found = 1;
			} else if (cmp1::gt(pairs[middle].left(), y)) {
				to = middle;
			} else {
				from = middle + 1;
			}
		}
		if (!found) {
			continue;
		}

		const T1 &x = p->left();
		do {
			if (exists(x, pairs[pos].right())) continue;
			unsetFlag(FL_TRANSITIVE);
			return false;
		} while (++pos<number && pairs[pos].left()==y);
	}

	setFlag(FL_TRANSITIVE);
	return true;
}


/* for all x and y in X it holds that xRy or yRx (or both). "Is
  greater than or equal to" is an example of a total relation
  (this definition for total is different from the one in the
  previous section). */
template<class T1, class T2, class cmp1, class cmp2>
bool Relation<T1, T2, cmp1, cmp2>::isLinear() const {
	if (!X.size() && !Y.size()) return true;
	if (checked & FL_LINEAR) return flags & FL_LINEAR;

	checkSetsAreEqual();
	const T1 *x = X.begin(), *y, *limit = X.end();
	for (; x!=limit; ++x) {
		for (y = x; y!=limit; ++y) {
			if (!exists(*x, *y) && !exists(*y, *x)) {
				unsetFlag(FL_LINEAR);
				return false;
			}
		}
	}

	setFlag(FL_LINEAR);
	return true;
}


/* for all x and y in X exactly one of xRy, yRx or x = y
   holds. "Is greater than" is an example of a trichotomous
   relation. */
template<class T1, class T2, class cmp1, class cmp2>
bool Relation<T1, T2, cmp1, cmp2>::isTrichotomous() const {
	if (!X.size() && !Y.size()) return true;
	if (checked & FL_TRICHOTOMOUS) return flags & FL_TRICHOTOMOUS;

	checkSetsAreEqual();
	const T1 *x = X.begin(), *y, *limit = X.end();
	for (; x!=limit; ++x) {
		for (y = x; y!=limit; ++y) {
			int counter = 0;
			if (cmp1::eq(*x, *y)) ++counter;
			if (exists(*x, *y)) ++counter;
			if (counter!=2 && exists(*y, *x)) ++counter;
			if (counter==1) continue;
			unsetFlag(FL_TRICHOTOMOUS);
			return false;
		}
	}

	setFlag(FL_TRICHOTOMOUS);
	return true;
}


/* for all x, y and z in X it holds that if xRy and xRz, then
   yRz. */
template<class T1, class T2, class cmp1, class cmp2>
bool Relation<T1, T2, cmp1, cmp2>::isEuclidean() const {
	if (!X.size() && !Y.size()) return true;
	if (checked & FL_EUCLIDEAN) return flags & FL_EUCLIDEAN;

	checkSetsAreEqual();
	const T1 *x = X.begin(), *limit = X.end();
	for (; x!=limit; ++x) {
		Set<T2, cmp2> elements = image(*x);
		const T2 *y = elements.begin(), *l = elements.end(), *z;
		for (; y!=l; ++y) {
			if (cmp1::eq(*x, *y)) continue;
			for (z = elements.begin(); z!=l && exists(*y, *z); ++z);
			if (z==l) continue;
			unsetFlag(FL_EUCLIDEAN);
			return false;
		}
	}

	setFlag(FL_EUCLIDEAN);
	return true;
}



}

#endif
