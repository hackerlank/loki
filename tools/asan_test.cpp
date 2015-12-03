#include <iostream>
using namespace std;

int main()
{
	int *ptr = new int(10);
	delete ptr;
	*ptr = 4;
	return 0;
}
