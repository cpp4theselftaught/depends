#include "../depends.hpp"
#include <sstream>
#include <iostream>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

struct S
{
	S()
		: i_(-1)
	{}
	S(int i)
		: i_(i)
	{}

	int i_;
	template < typename A >
	void serialize(A & ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("i_", i_);
	}
};

bool operator<(const S & lhs, const S & rhs)
{
	return lhs.i_ < rhs.i_;
}

void test1()
{
	int i1[3] = { 0, 1, 2 };
	Depends::Depends< S > deps(i1, i1 + 3);
	deps.select(S(0));
	deps.addPrerequisite(S(1));
	deps.select(S(1));
	deps.addPrerequisite(S(2));
	deps.select(S(2));
#ifdef DEPENDS_SUPPORT_SERIALIZATION
	std::stringstream os;
	{
		boost::archive::xml_oarchive oa(os);
		oa << boost::serialization::make_nvp("deps", deps);
	}
	std::cout << os.str() << std::endl;
	std::stringstream is(os.str());
	boost::archive::xml_iarchive ia(is);
	Depends::Depends< S > deps2;
	ia >> boost::serialization::make_nvp("deps", deps2);
	deps2.select(S(2));
	std::set< S > preqs(deps2.getDependants(true));
	assert(preqs.size() == 2);
	assert(preqs.find(S(0)) != preqs.end());
	assert(preqs.find(S(1)) != preqs.end());
	assert(preqs.find(S(2)) == preqs.end());
#endif
}

int main()
{
	test1();
	return 0;
}
