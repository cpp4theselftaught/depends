#include <cassert>
#include <Depends/Depends.h>
#include <boost/tuple/tuple.hpp>

void test1()
{
	Depends::Depends< int > deps;
	assert(deps.empty());
	assert(deps.begin() == deps.end());
	assert(deps.rbegin() == deps.rend());
	assert(deps.size() == 0);
	const Depends::Depends< int > & deps2(deps);
	assert(deps2.empty());
	assert(deps2.begin() == deps2.end());
	assert(deps2.rbegin() == deps2.rend());
	assert(deps2.size() == 0);
}

void test2()
{
	int i[3] = {0, 1, 2};
	Depends::Depends< int > deps(i, i + 3);
	assert(!deps.empty());
	assert(std::distance(deps.begin(), deps.end()) == 3);
	assert(std::distance(deps.rbegin(), deps.rend()) == 3);
	assert(deps.size() == 3);
	const Depends::Depends< int > & deps2(deps);
	assert(std::distance(deps2.begin(), deps2.end()) == 3);
	assert(std::distance(deps2.rbegin(), deps2.rend()) == 3);
	assert(deps2.size() == 3);
}

void test3()
{
	int i1[3] = { 0, 1, 2 };
	Depends::Depends< int > deps1(i1, i1 + 3);
	int i2[3] = { 0, 1, 3 };
	Depends::Depends< int > deps2(i2, i2 + 3);
}

void test4()
{
	Depends::Depends< int > deps;
	Depends::Depends< int >::iterator where;
	bool inserted;
	boost::tie(where, inserted) = deps.insert(12);
	assert(where != deps.end());
	assert(inserted);
	boost::tie(where, inserted) = deps.insert(12);
	assert(where != deps.end());
	assert(!inserted);
	where = deps.insert(where, 13);
	assert(where != deps.end());
	Depends::Depends< int >::size_type count(0);
	count = deps.erase(12);
	assert(count == 1);
	deps.erase(where);
	assert(deps.empty());
}

void test5()
{
	int i1[3] = { 0, 1, 2 };
	Depends::Depends< int > deps1(i1, i1 + 3);
	deps1.clear();
	assert(deps1.empty());
}

void test6()
{
	int i1[3] = { 0, 1, 2 };
	Depends::Depends< int > deps1(i1, i1 + 3);
	deps1.erase(deps1.begin(), deps1.end());
	assert(deps1.empty());
}

void test7()
{
	int i1[3] = { 0, 1, 2 };
	Depends::Depends< int > deps(i1, i1 + 3);
	assert(deps.size() == 3);
	for (int i = 0; i < 10; ++i)
		deps.select(i);
	assert(deps.size() == 10);
}

void test8()
{
	int i1[3] = { 0, 1, 2 };
	Depends::Depends< int > deps(i1, i1 + 3);
	deps.select(1);
	deps.addPrerequisite(0);
	assert(deps.depends(1, 0));
	assert(deps.depends(deps.find(1), 0));
	assert(deps.depends(1, deps.find(0)));
	assert(deps.depends(deps.find(1), deps.find(0)));
	assert(!deps.depends(0, 1));
	assert(!deps.depends(deps.find(0), 1));
	assert(!deps.depends(0, deps.find(1)));
	assert(!deps.depends(deps.find(0), deps.find(1)));
}

void test9()
{
	int i1[3] = { 0, 1, 2 };
	Depends::Depends< int > deps(i1, i1 + 3);
	deps.select(0);
	deps.addDependant(1);
	assert(deps.depends(1, 0));
	assert(deps.depends(deps.find(1), 0));
	assert(deps.depends(1, deps.find(0)));
	assert(deps.depends(deps.find(1), deps.find(0)));
	assert(!deps.depends(0, 1));
	assert(!deps.depends(deps.find(0), 1));
	assert(!deps.depends(0, deps.find(1)));
	assert(!deps.depends(deps.find(0), deps.find(1)));
}

void test10()
{
	int i1[3] = { 0, 1, 2 };
	Depends::Depends< int > deps(i1, i1 + 3);
	deps.select(0);
	deps.addDependant(1);
	assert(deps.depends(1, 0));
	assert(deps.depends(deps.find(1), 0));
	assert(deps.depends(1, deps.find(0)));
	assert(deps.depends(deps.find(1), deps.find(0)));
	assert(!deps.depends(0, 1));
	assert(!deps.depends(deps.find(0), 1));
	assert(!deps.depends(0, deps.find(1)));
	assert(!deps.depends(deps.find(0), deps.find(1)));
	deps.removeDependant(1);
	assert(!deps.depends(1, 0));
	assert(!deps.depends(deps.find(1), 0));
	assert(!deps.depends(1, deps.find(0)));
	assert(!deps.depends(deps.find(1), deps.find(0)));
}

void test11()
{
	int i1[3] = { 0, 1, 2 };
	Depends::Depends< int > deps(i1, i1 + 3);
	deps.select(0);
	deps.addDependant(1);
	deps.select(1);
	deps.addDependant(2);
	std::set< int > preqs(deps.getPrerequisites());
	assert(preqs.size() == 1);
	assert(preqs.find(0) != preqs.end());
	assert(preqs.find(2) == preqs.end());
}

void test12()
{
	int i1[3] = { 0, 1, 2 };
	Depends::Depends< int > deps(i1, i1 + 3);
	deps.select(0);
	deps.addDependant(1);
	deps.select(1);
	deps.addDependant(2);
	deps.select(2);
	std::set< int > preqs(deps.getPrerequisites(true));
	assert(preqs.size() == 2);
	assert(preqs.find(0) != preqs.end());
	assert(preqs.find(1) != preqs.end());
	assert(preqs.find(2) == preqs.end());
}

void test13()
{
	int i1[3] = { 0, 1, 2 };
	Depends::Depends< int > deps(i1, i1 + 3);
	deps.select(0);
	deps.addPrerequisite(1);
	deps.select(1);
	deps.addPrerequisite(2);
	std::set< int > preqs(deps.getDependants());
	assert(preqs.size() == 1);
	assert(preqs.find(0) != preqs.end());
	assert(preqs.find(2) == preqs.end());
}

void test14()
{
	int i1[3] = { 0, 1, 2 };
	Depends::Depends< int > deps(i1, i1 + 3);
	deps.select(0);
	deps.addPrerequisite(1);
	deps.select(1);
	deps.addPrerequisite(2);
	deps.select(2);
	std::set< int > preqs(deps.getDependants(true));
	assert(preqs.size() == 2);
	assert(preqs.find(0) != preqs.end());
	assert(preqs.find(1) != preqs.end());
	assert(preqs.find(2) == preqs.end());
}

int main()
{
	test1();
	test2();
	test3();
	test4();
	test5();
	test6();
	test7();
	test8();
	test9();
	test10();
	test11();
	test12();
	test13();
	test14();
}
