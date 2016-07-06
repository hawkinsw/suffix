#include "suffix.hpp"

#include <vector>
#include <iostream>
using namespace std;

ostream &operator<<(ostream &os, const std::vector<string*> &v)
{
	for (auto s : v)
		os << *s;
	return os;
}

int main() {
	std::vector<string*> list_of_strings;

	list_of_strings.push_back(new string("b"));
	list_of_strings.push_back(new string("a"));
	list_of_strings.push_back(new string("n"));
	list_of_strings.push_back(new string("a"));
	list_of_strings.push_back(new string("n"));
	list_of_strings.push_back(new string("a"));
	SuffixTree<std::vector, string*> st_list(true);
	st_list.Build(list_of_strings);
	cout << list_of_strings << " substrings: " << endl;
	st_list.PrintSubstrings(cout, 0);
	cout << st_list << endl;
	return 1;
}
