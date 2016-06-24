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

template <class T>
class SuffixTree
{
	public:
		SuffixTree(bool verbose = true) : m_root(NULL), m_verbose(verbose) {}
		void Build(T obj);
	
		friend std::ostream &operator<<(std::ostream &os, SuffixTree<T> &st)
		{
			//os << st.entire << ":" << std::endl;
			st.DoPrint(os, st.m_root);
			return os;
		}
		void PrintSubstrings(std::ostream &, int occurence_filter = 0);
	private:
		void AddSuffix(unsigned int suffi);
		void DoInsert(unsigned int offset, Locus *locus);
		void DoPrint(std::ostream &os, Locus *locus, int ws=0);
		int DoPrintSubstrings(std::ostream &os, Locus *locus, T base, int occurence_filter);

		Locus *m_root;
		T entire;
		bool m_verbose;
};

#include "suffix.tcc"


#endif
