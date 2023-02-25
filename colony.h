#pragma once

#include <vector>
#include <set>
#include <utility>

using T = int;

class Colony
{
private:
	std::vector<std::vector<T>> map;

	int number_of_chunks;
	int number_of_elements;
	int size_of_last_chunk;
	int back_index;

	std::set<int> skipfield;

	int number_of_empty_chunks;


public:

	Colony();


	int size() const;
	void insert(T data);
	void erase(int index);

	T& operator[](int i);


	class Iterator
	{
	private:
		Colony* c;
		int m; int n; 

	public:
		Iterator(Colony* c, int m, int n );

		bool operator!=(const Iterator& it) { return c != it.c || m != it.m || n != it.n; }
		T& operator*() const;
		Iterator& operator++();
	};


	Iterator begin();

	Iterator end();
private:

	std::pair<int, int> count_indexes(int index);

	void create_new_chunk();
	bool full();

};