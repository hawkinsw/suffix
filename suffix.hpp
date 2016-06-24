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

std::ostream &operator<<(std::ostream &os, Locus &locus)
{
	os << "(" << locus.Start() << "," << locus.Stop() << ")";
	return os;
}

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
	private:
		void AddSuffix(unsigned int suffi);
		void DoInsert(unsigned int offset, Locus *locus);
		void DoPrint(std::ostream &os, Locus *locus, int ws=0);

		Locus *m_root;
		T entire;
		bool m_verbose;
};


template <class T>
void SuffixTree<T>::DoPrint(std::ostream &os, Locus *locus, int ws)
{
	for (auto c = locus->ChildrenBegin();
	          c != locus->ChildrenEnd();
						c++)
	{
		std::stringstream locus_output_string;
		for (int i = 0; i<ws; i++) os << " ";

		locus_output_string << **c << ": ";
		/*
		 * We would like to be able to do substr()
		 * here, but we can't guarantee that we have
		 * that. So, we'll do it the slow way.
		 */
		if ((*c)->Start() >= 0 && (*c)->Stop() >= 0)
		{
			for (int i = (*c)->Start(); i<(*c)->Stop(); i++)
				locus_output_string << entire[i];
		}
		locus_output_string << "->";
		os << locus_output_string.str() << std::endl;

		DoPrint(os, *c, ws + locus_output_string.str().size());
		os << std::endl;
	}
}

template <class T>
void SuffixTree<T>::Build(T obj)
{
	entire = obj;
	m_root = new Locus();

	unsigned int length = entire.size();
	unsigned int i = 0;
	for (i = 0; i<length; i++)
	{
		AddSuffix(i);
	}
}

template <class T>
void SuffixTree<T>::AddSuffix(unsigned int suffi)
{
	DoInsert(suffi, m_root);
}

template <class T>
void SuffixTree<T>::DoInsert(unsigned int offset, Locus *locus)
{
	bool acted = false;

	if (m_verbose)
		std::cout << "DoInsert(" << offset << ", " << *locus << ")" << std::endl;

	for (auto c = locus->ChildrenBegin();
	          c != locus->ChildrenEnd() && !acted;
						c++)
	{
		Locus *child = *c;
		unsigned int o = offset;
		if (m_verbose)
			std::cout << "Inspecting child: " << *child << std::endl;
		if (entire[child->Start()] == entire[o])
		{
			bool matches_entirely = false;
			bool suffix_exhausted = false;
			unsigned int i = child->Start();
			
			acted = true;

			if (m_verbose)
				std::cout << "Acting on child: " << *child << std::endl;

			while (true)
			{
				if (m_verbose)
					std::cout << "i: " << i << std::endl
					          << "o: " << o << std::endl
					          << entire[i] << " == " << entire[o] << std::endl;

				if (i>=child->Stop())
				{
					/*
					 * We have exhausted this arc's string.
					 */
					if (m_verbose)
						std::cout << "Setting matches_entirely." << std::endl;
					matches_entirely = true;
					break;
				}
				if (o>=entire.size())
				{
					/*
					 * We have exhausted the suffix. This is
					 * a workaround for when the input
					 * does not end with a special sentinel.
					 */
					if (m_verbose)
						std::cout << "Suffix exhausted; declaring mismatch." << std::endl;
					break;
				}

				if (entire[i] != entire[o])
				{
					/*
				 	* We found a point of difference.
				 	*/
					if (m_verbose)
						std::cout << "Found a point of difference." << std::endl;
					break;
				}

				i++;
				o++;
			}

			if (matches_entirely)
			{
				/*
				 * Descend to the child locus based on the arc
				 * on which we just completed a match.
				 */
				DoInsert(o, child);
			}
			else if (suffix_exhausted)
			{
				/* 
				 * Add a "terminal" link.
				 */
				Locus *terminal = new Locus();
				terminal->Start() = -1;
				terminal->Stop() = -1;
				child->AddChild(terminal);
				if (m_verbose)
					std::cout << "Adding a new terminal locus." << std::endl;
			}
			else
			{
				if (m_verbose)
					std::cout << "Breaking apart an arc." << std::endl;

				if (m_verbose)
					std::cout << "Was " << *child << std::endl;

				Locus *new_child = new Locus();
				Locus *left = child;
				Locus *right = new Locus();

				new_child->Start() = child->Start();
				new_child->Stop() = i;

				left->Start() = i;
				left->Stop() = child->Stop();

				right->Start() = o;
				right->Stop() = entire.size();

				new_child->AddChild(right);
				new_child->AddChild(left);

				locus->RemoveChild(child);
				locus->AddChild(new_child);

				if (m_verbose)
					std::cout << "Now is " << *new_child << std::endl
					          << "Left: " << *left << std::endl
					          << "Right: " << *right << std::endl;
			}
		}
		else
		{
			if (m_verbose)
				std::cout << "Skipping child inspection entirely." << std::endl;
		}
	}

	if (!acted)
	{
		/*
		 * insert obj[obj_i:] as a new locus off locus.
		 */
		Locus *new_child = new Locus();
		new_child->Start() = offset;
		new_child->Stop() = entire.size();
		locus->AddChild(new_child);

		if (m_verbose)
			std::cout << "New locus: " << *new_child << std::endl;
	}
}

#endif
