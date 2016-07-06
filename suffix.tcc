template <template <typename...> class Container, typename Element>
void SuffixTreeBase<Container, Element>::PrintSubstrings(std::ostream &os, int occurence_filter)
{
	for (auto c = m_root->ChildrenBegin();
	          c != m_root->ChildrenEnd();
						c++)
	{
		Container<Element> new_base;
		if (((*c)->Start() >= 0 && (*c)->Stop() >= 0))
		{
			for (int i = (*c)->Start(); i<=(*c)->Stop() && i<entire.size(); i++)
				new_base.push_back(entire[i]);
		}
		DoPrintSubstrings(os, *c, new_base, occurence_filter);
	}
}

template <template <typename...> class Container, typename Element>
int SuffixTreeBase<Container, Element>::DoPrintSubstrings(std::ostream &os, Locus *locus, Container<Element> base, int occurence_filter)
{
	int child_counter = 0;

	bool has_children = false;
	for (auto c = locus->ChildrenBegin();
	          c != locus->ChildrenEnd();
						c++)
	{
		has_children = true;
		Container<Element> new_base = base;

		/*
		 * We would like to be able to do substr()
		 * here, but we can't guarantee that we have
		 * that. So, we'll do it the slow way.
		 */
		if (((*c)->Start() >= 0 && (*c)->Stop() >= 0))
		{
			for (int i = (*c)->Start(); i<=(*c)->Stop() && i<entire.size(); i++)
				new_base.push_back(entire[i]);
		}
		child_counter += DoPrintSubstrings(os, *c, new_base, occurence_filter);
	}

	if (!has_children)
		child_counter++;

	if (child_counter>occurence_filter)
		os << "# " << child_counter << "," << base << std::endl;

	return child_counter;
}

template <template <typename...> class Container, typename Element>
void SuffixTreeBase<Container, Element>::DoPrint(std::ostream &os, Locus *locus, int ws)
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
		if (m_verbose && ((*c)->Start() >= 0 && (*c)->Stop() >= 0))
		{
			for (int i = (*c)->Start(); i<=(*c)->Stop() && i<entire.size(); i++)
			{
				locus_output_string << StringifyElement(entire[i]);
			}
		}
		locus_output_string << "->";
		os << locus_output_string.str() << std::endl;

		DoPrint(os, *c, ws + locus_output_string.str().size());
		os << std::endl;
	}
}

template <template <typename...> class Container, typename Element>
void SuffixTreeBase<Container, Element>::Build(Container<Element> obj)
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

template <template <typename...> class Container, typename Element>
void SuffixTreeBase<Container, Element>::AddSuffix(unsigned int suffi)
{
	DoInsert(suffi, m_root);
}

/*
 * Here's an example string with examples of various indexes:
 *
 * b a n a n a
 *
 * _ _ _ _ _ _ $
 *
 * 0 1 2 3 4 5 6
 */
template <template <typename...> class Container, typename Element>
void SuffixTreeBase<Container, Element>::DoInsert(
	unsigned int offset,
	Locus *locus)
{
	bool acted = false;

	if (m_verbose)
		std::cout << "DoInsert(" << offset << ", " << *locus << ")" << std::endl;

	if (offset >= entire.size())
	{
		/*
		 * Add a "sentinel" locus as a child.
		 */
		Locus *terminal = new Locus();
		terminal->Start() = -1;
		terminal->Stop() = -1;
		terminal->AddTerminus(offset);
		terminal->ContractedLocus() = locus;

		locus->AddChild(terminal);

		if (m_verbose)
			std::cout << "Adding a new terminal locus." << std::endl;
		return;
	}

	for (auto c = locus->ChildrenBegin();
	          c != locus->ChildrenEnd() && !acted;
						c++)
	{
		Locus *child = *c;
		unsigned int o = offset;
		if (m_verbose)
			std::cout << "Inspecting child: " << *child << std::endl;

		if (!not_equal(entire[child->Start()],entire[o]))
		{
			bool matches_entirely = false;
			unsigned int i = child->Start();
			
			acted = true;

			if (m_verbose)
				std::cout << "Acting on child: " << *child << std::endl;

			while (true)
			{
				if (m_verbose)
				{
					std::cout << "i: " << i << std::endl
					          << "o: " << o << std::endl;
					if (i<child->Stop())
						std::cout << entire[i];
					else
						std::cout << "-1";
					std::cout << " == ";
					if (o<entire.size())
						std::cout << entire[o];
					else
						std::cout << "-1";
					std::cout << std::endl;
				}
				if (i>child->Stop())
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

				if (not_equal(entire[i], entire[o]))
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
				child->AddTerminus(o);
			}
			else
			{
				/*
				 * The distance that we have to split.
				 */
				int new_child_terminii_delta = child->Stop() - (i - 1);
				std::list<int> new_child_terminii;

				if (m_verbose)
					std::cout << "Breaking apart an arc." << std::endl;

				if (m_verbose)
					std::cout << "Was " << *child << std::endl;

				Locus *new_child = new Locus();
				Locus *left = child;
				Locus *right = new Locus();

				new_child->Start() = child->Start();
				new_child->Stop() = i - 1;

				for (auto t : child->Terminii())
					new_child_terminii.push_back(t - new_child_terminii_delta);
				new_child_terminii.push_back(o-1);

				left->Start() = i;
				left->Stop() = child->Stop();

				right->Start() = o;
				right->Stop() = entire.size();

				new_child->AddChild(right);
				new_child->AddChild(left);
				new_child->Terminii(new_child_terminii);

				right->ContractedLocus() = new_child;
				left->ContractedLocus() = new_child;

				locus->RemoveChild(child);
				locus->AddChild(new_child);
				new_child->ContractedLocus() = locus;

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
		new_child->ContractedLocus() = locus;
		locus->AddChild(new_child);

		if (m_verbose)
			std::cout << "New locus: " << *new_child << std::endl;
	}
}

template <template <typename...> class Container, typename Element>
bool SuffixTree<Container, Element>::not_equal(Element a, Element b)
{
	return a != b;
}

template <template <typename...> class Container, typename Element>
bool SuffixTree<Container, Element*>::not_equal(Element *a, Element *b)
{
	return (*a) != (*b);
}

template <template <typename...> class Container, typename Element>
std::string SuffixTree<Container, Element>::StringifyElement(Element a)
{
	return std::string(a);
}

template <template <typename...> class Container, typename Element>
std::string SuffixTree<Container, Element*>::StringifyElement(Element *a)
{
	return std::string(*a);
}
