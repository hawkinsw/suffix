#include "suffix.hpp"

#include <vector>
#include <iostream>
using namespace std;


ostream &operator<<(ostream &os, const std::vector<string> &v)
{
	for (auto s : v)
		os << s;
	return os;
}

int main() {
	std::vector<string> list_of_strings;

	list_of_strings.push_back("b");
	list_of_strings.push_back("a");
	list_of_strings.push_back("n");
	list_of_strings.push_back("a");
	list_of_strings.push_back("n");
	list_of_strings.push_back("a");

	SuffixTree<string> st(true);
	SuffixTree<std::vector<string>> st_list(false);
	st.Build("bananaz");
	st_list.Build(list_of_strings);

	cout << st << endl;
	cout << st_list << endl;

	st.PrintSubstrings(cout);
	st_list.PrintSubstrings(cout);
	return 1;
}
