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
	if (skipfield.empty())		//Když není žádný prvek smazaný, tak pøidáváme data na konec
	{
		if (back_index == map[number_of_chunks-1].size())		//Pokud je i poslední chunk plnì zaplnìn, vytvoøíme nový
		{
			create_new_chunk();
		}
		
		map[number_of_chunks-1][back_index] = data;
		back_index++;

		number_of_elements++;
	}
	else	//Vybereme nejmenší index ze skipfieldu a ten zaplníme
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
	if (index >= number_of_elements)		//Když chceme smazat prvek na indexu vìtším, než je poèet prvkù, tak vyhodíme výjimku
	{
		throw std::out_of_range("Out of range");
	}

	int index_ = index;

	int real_index = 0;

	for (std::set<int>::iterator it = skipfield.begin(); it != skipfield.end(); ++it)		//Pøepoète skuteèný index v našich datech map, protože nìkterá data už mohou být "smazaná" a existovat ve skipfieldu, tak je musíme pøeskoèikt
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


	if (index == number_of_elements)  //Jestlize mazeme posledni prvek, sníží se back_index, navíc tady už mùžeme psát v podmínce number_of_elements, protože už number_of_elements o 1 zmenšili
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



std::pair<int, int> Colony::count_indexes(int index)		//Vrátí pár indexù ve vektoru map
{
	std::pair<int, int> indexes;

	int index_ = index;

	int help = 0;
	for (int i = 0; i < number_of_chunks; i++)		//Poèítáme, ve kterém chunku leží daný index
	{
		help += map[i].size();
		if (index_ <= help - 1)			//Když je index menší, než souèet velikostí všech chunkù do indexu i, tak zastavíme, protože index leží v i-tém chunku
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

	if (number_of_empty_chunks > 0)		//Jestliže máme nìjaké prázdné chunky na konci, tak nebudeme alokovat nový chunk, ale zaplníme tento postupnì
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


Colony::Iterator Colony::begin()		//Vrátí iterátor s ukazatelem na první prvek, který nebyl smazaný
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

	do	//Budeme zvìtšovat index do té doby, než pøeskáèeme všechny smazané prvky uložené ve skipfieldu
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

		if (!(*this != c->end()))		//Jestliže jsme už na konci u back_indexu, tak vrátíme iterátor na konec
		{
			return *this;
		}
	} while (c->skipfield.count(index));

	

	return *this;
}
