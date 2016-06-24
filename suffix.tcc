std::ostream &operator<<(std::ostream &os, Locus &locus)
{
	os << "(" << locus.Start() << "," << locus.Stop() << ")";
	return os;
}

template <class T>
void SuffixTree<T>::PrintSubstrings(std::ostream &os)
{
	for (auto c = m_root->ChildrenBegin();
	          c != m_root->ChildrenEnd();
						c++)
	{
		T new_base;
		if (((*c)->Start() >= 0 && (*c)->Stop() >= 0))
		{
			for (int i = (*c)->Start(); i<(*c)->Stop(); i++)
				new_base.push_back(entire[i]);
		}
		DoPrintSubstrings(os, *c, new_base);
	}
}

template <class T>
int SuffixTree<T>::DoPrintSubstrings(std::ostream &os, Locus *locus, T base)
{
	int child_counter = 0;

	bool has_children = false;
	for (auto c = locus->ChildrenBegin();
	          c != locus->ChildrenEnd();
						c++)
	{
		has_children = true;
		T new_base = base;

		/*
		 * We would like to be able to do substr()
		 * here, but we can't guarantee that we have
		 * that. So, we'll do it the slow way.
		 */
		if (((*c)->Start() >= 0 && (*c)->Stop() >= 0))
		{
			for (int i = (*c)->Start(); i<(*c)->Stop(); i++)
				new_base.push_back(entire[i]);
		}
		child_counter += DoPrintSubstrings(os, *c, new_base);
	}

	if (!has_children)
		child_counter++;

	os << "# " << child_counter << "," << base << std::endl;

	return child_counter;
}

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
		if (m_verbose && ((*c)->Start() >= 0 && (*c)->Stop() >= 0))
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
