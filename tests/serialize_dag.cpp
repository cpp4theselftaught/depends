#include "../dag.hpp"
#include <sstream>
#include <iostream>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

void test1()
{
	// create a DAG with a few entries
	std::vector< int > data;
	for (int i = 0; i < 10; ++i)
		data.push_back(i);
	Depends::DAG< int > dag(data.begin(), data.end());
	// create two paths through the DAG
	for (int i = 0; i < 2; ++i)
	{
		std::random_shuffle(data.begin(), data.end());
		for (std::vector< int >::const_iterator where(data.begin()); (where + 1) != data.end(); ++where)
		{
			try
			{
				dag.link(*where, *(where + 1));
			}
			catch (const Depends::DAG< int >::circular_reference_exception & )
			{ /* no-op */ }
		}
	}
#ifdef DEPENDS_SUPPORT_SERIALIZATION
	// now try to serialize the bastard
	std::stringstream os;
	{
		boost::archive::xml_oarchive oa(os);
		oa << boost::serialization::make_nvp("dag", dag);
	}
	std::cout << os.str() << std::endl;
	// now, re-create the DAG
	std::stringstream is(os.str());
	boost::archive::xml_iarchive ia(is);
	Depends::DAG< int > dag2;
	ia >> boost::serialization::make_nvp("dag", dag2);
	assert(dag == dag2);
#endif
}

int main()
{
	test1();

	return 0;
}
