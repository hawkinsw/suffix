#ifndef SUFFIX_H
#define SUFFIX_H

#include <unistd.h>
#include <assert.h>
#include <list>
#include <iostream>
#include <sstream>

class Locus
{
	public:
		Locus() : m_start(0), m_stop(0), m_contracted_locus(NULL) {}

		int &Start() { return m_start; }
		int &Stop() { return m_stop; }

		Locus *&ContractedLocus() { return m_contracted_locus; }

		std::list<Locus*>::iterator ChildrenBegin() { return children.begin(); }
		std::list<Locus*>::iterator ChildrenEnd() { return children.end(); }
		void AddChild(Locus *new_child) { children.push_back(new_child); }
		void RemoveChild(Locus *child) { children.remove(child); }

		friend std::ostream &operator<<(std::ostream &os, Locus &locus);

	private:
		int m_start, m_stop;
		std::list<Locus*> children;
		Locus *m_contracted_locus;
};

template <template <typename...> class Container, typename Element>
class SuffixTreeBase
{
	public:
		SuffixTreeBase(bool verbose = true) : m_root(NULL), m_verbose(verbose) {}
		void Build(Container<Element> obj);
	
		friend std::ostream &operator<<(std::ostream &os, SuffixTreeBase<Container, Element> &st)
		{
			//os << st.entire << ":" << std::endl;
			st.DoPrint(os, st.m_root);
			return os;
		}
		void PrintSubstrings(std::ostream &, int occurence_filter = 0);
	protected:
		void AddSuffix(unsigned int suffi);
		void DoInsert(unsigned int offset, Locus *locus);
		virtual bool not_equal(Element a, Element b)=0;
		virtual std::string StringifyElement(Element a)=0;
		void DoPrint(std::ostream &os, Locus *locus, int ws=0);
		int DoPrintSubstrings(std::ostream &os, Locus *locus, Container<Element> base, int occurence_filter);

		Locus *m_root;
		Container<Element> entire;
		bool m_verbose;
};

template <template <typename...> class Container, typename Element>
class SuffixTree : public SuffixTreeBase<Container, Element>
{
	public:
		SuffixTree(bool verbose): SuffixTreeBase<Container, Element>(verbose) {};
	protected:
		bool not_equal(Element a, Element b);
		std::string StringifyElement(Element a);
};

template <template <typename...> class Container, typename Element>
class SuffixTree<Container, Element*> : public SuffixTreeBase<Container, Element*>
{
	public:
		SuffixTree(bool verbose): SuffixTreeBase<Container, Element*>(verbose) { std::cout << "Constructing specialized ST" << std::endl; };
	protected:
		bool not_equal(Element *a, Element *b);
		std::string StringifyElement(Element *a);
};

#include "suffix.tcc"
#endif
