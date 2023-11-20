
#include <iostream>
#include "person.pb.h"

using namespace std;

int main()
{
	std::cout << "Hello World!\n";

	Persion p;
	p.set_id(1001);
	p.set_name("路飞");
	p.set_age(12);
	p.set_sex("man");
	string str;
	p.SerializeToString(&str);
	cout << str << endl;

	Persion pp;
	pp.ParseFromString(str);
	cout << pp.name() << endl;
}
