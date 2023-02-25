#include "colony.h"
#include <stdexcept>   

Colony::Colony()
{
	size_of_last_chunk = 10;
	number_of_chunks = 1;
	number_of_elements = 0;
	back_index = 0;

	number_of_empty_chunks = 0;

	map.push_back(std::vector<int>(size_of_last_chunk));

}

int Colony::size() const
{
	return this->number_of_elements;
}

void Colony::insert(T data)
{
	if (skipfield.empty())		//Kdy� nen� ��dn� prvek smazan�, tak p�id�v�me data na konec
	{
		if (back_index == map[number_of_chunks-1].size())		//Pokud je i posledn� chunk pln� zapln�n, vytvo��me nov�
		{
			create_new_chunk();
		}
		
		map[number_of_chunks-1][back_index] = data;
		back_index++;

		number_of_elements++;
	}
	else	//Vybereme nejmen�� index ze skipfieldu a ten zapln�me
	{
		int i = *skipfield.begin();
		skipfield.erase(i);

		std::pair<int, int> position = count_indexes(i);

		map[position.first][position.second] = data;

		number_of_elements++;
	}
}

void Colony::erase(int index)
{
	if (index >= number_of_elements)		//Kdy� chceme smazat prvek na indexu v�t��m, ne� je po�et prvk�, tak vyhod�me v�jimku
	{
		throw std::out_of_range("Out of range");
	}

	int index_ = index;

	int real_index = 0;

	for (std::set<int>::iterator it = skipfield.begin(); it != skipfield.end(); ++it)		//P�epo�te skute�n� index v na�ich datech map, proto�e n�kter� data u� mohou b�t "smazan�" a existovat ve skipfieldu, tak je mus�me p�esko�ikt
	{
		if (*it <= index)
		{
			real_index++;
		}
		if (*it > index)
		{
			break;
		}
	}
	index_ += real_index;

	skipfield.insert(index_);
	number_of_elements--;


	if (index == number_of_elements)  //Jestlize mazeme posledni prvek, sn�� se back_index, nav�c tady u� m��eme ps�t v podm�nce number_of_elements, proto�e u� number_of_elements o 1 zmen�ili
	{
		back_index--;

		if (back_index == -1)
		{
			number_of_chunks--;
			size_of_last_chunk /= 2;
			back_index = size_of_last_chunk;

			number_of_empty_chunks++;
		}
	}
}

T& Colony::operator[](int i)
{
	try 
	{
		if (i >= number_of_elements)
		{
			throw std::out_of_range("Out of range");
		}


		int real_index = 0;
		for (std::set<int>::iterator it = skipfield.begin(); it != skipfield.end(); ++it)
		{
			if (*it <= i)
			{
				real_index++;
			}
			if (*it > i)
			{
				break;
			}
		}


		std::pair<int, int> position = count_indexes(i + real_index);

		return map[position.first][position.second];
	}
	catch (std::out_of_range& ex)
	{
		throw std::out_of_range("Out of range");
	}
	
}



std::pair<int, int> Colony::count_indexes(int index)		//Vr�t� p�r index� ve vektoru map
{
	std::pair<int, int> indexes;

	int index_ = index;

	int help = 0;
	for (int i = 0; i < number_of_chunks; i++)		//Po��t�me, ve kter�m chunku le�� dan� index
	{
		help += map[i].size();
		if (index_ <= help - 1)			//Kdy� je index men��, ne� sou�et velikost� v�ech chunk� do indexu i, tak zastav�me, proto�e index le�� v i-t�m chunku
		{
			indexes.first = i;
			index_ -= (help - map[i].size());
			indexes.second = index_;

			return indexes;
		}
	}
	
}

void Colony::create_new_chunk()
{
	size_of_last_chunk *= 2;
	
	number_of_chunks++;
	back_index = 0;

	if (number_of_empty_chunks > 0)		//Jestli�e m�me n�jak� pr�zdn� chunky na konci, tak nebudeme alokovat nov� chunk, ale zapln�me tento postupn�
	{
		number_of_empty_chunks--;
	}
	else
	{
		map.push_back(std::vector<T>(size_of_last_chunk));
	}
	
}

bool Colony::full()
{
	if (skipfield.empty())
	{
		if (back_index == map[number_of_chunks - 1].size())
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}

Colony::Iterator::Iterator(Colony* c, int m, int n)
	: c(c), m(m), n(n)
{

}


Colony::Iterator Colony::begin()		//Vr�t� iter�tor s ukazatelem na prvn� prvek, kter� nebyl smazan�
{
	int i = 0;
	while (skipfield.count(i))
	{
		i++;
	}

	std::pair<int,int> pos = count_indexes(i);

	return Iterator(this, pos.first, pos.second);
}

Colony::Iterator Colony::end()
{
	return Iterator(this, number_of_chunks-1, back_index);
}

T& Colony::Iterator::operator*() const
{
	return this->c->map[m][n];
}

Colony::Iterator& Colony::Iterator::operator++()
{
	int index = 0;
	for (int i = 0; i < m; i++)
	{
		index += c->map[i].size();
	}
	index += n;

	/*n++;
	index++;*/

	do	//Budeme zv�t�ovat index do t� doby, ne� p�esk��eme v�echny smazan� prvky ulo�en� ve skipfieldu
	{
		n++;
		index++;
		if (n >= c->map[m].size())
		{
			m++;
			n = 0;
			if (m >= c->number_of_chunks)
			{
				throw std::out_of_range("Out of range");
			}
		}

		if (!(*this != c->end()))		//Jestli�e jsme u� na konci u back_indexu, tak vr�t�me iter�tor na konec
		{
			return *this;
		}
	} while (c->skipfield.count(index));

	

	return *this;
}
