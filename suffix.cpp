#include "suffix.hpp"

using namespace std;

std::ostream &operator<<(std::ostream &os, Locus &locus)
{
	os << "|" << std::hex << &locus << "|";
	os << std::hex << locus.ContractedLocus() << "<-";
	os << "(" << std::dec << locus.Start()<<","<< std::dec << locus.Stop() << ")";
	os << "[";
	for (auto t : locus.Terminii())
	{
		os << t << ",";
	}
	os << "]";
	os << "d: " << std::dec << locus.Descendents();
	return os;
}
