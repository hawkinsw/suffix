#include "suffix.hpp"

#include <vector>
using namespace std;

int main() {
	std::vector<string> list_of_strings;

	list_of_strings.push_back("b");
	list_of_strings.push_back("a");
	list_of_strings.push_back("n");
	list_of_strings.push_back("a");
	list_of_strings.push_back("n");
	list_of_strings.push_back("a");
	list_of_strings.push_back("z");

	SuffixTree<string> st(false);
	SuffixTree<std::vector<string>> st_list(false);
	st.Build("bananaz");
	st_list.Build(list_of_strings);

	cout << st << endl;
	cout << st_list << endl;
	return 1;
}
