#include "colony.h"
#include <iostream>
#include <chrono>

using namespace std;

int main()
{
	Colony c;

	for (int i = 0; i < 20; i++)
	{
		c.insert(i*10);
	}

	cout << c[15] << endl;

	auto start = chrono::high_resolution_clock::now();
	c.erase(19);
	c.erase(15);
	
	cout << c[10] << endl;

	auto stop = chrono::high_resolution_clock::now();

	auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);

	cout << "Time taken by function: "
		<< duration.count() << " microseconds" << endl;

	cout << c[16] << endl;

	for (auto i : c)
	{
		cout << i << " ";
	}
	/*for(Colony::Iterator it = c.begin(); it != c.end(); ++it)
	{
		cout << *it << " ";
	}*/
}