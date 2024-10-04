#pragma once
#include <iostream>
#include <fstream>
#include <regex>
#include <chrono>
#include <vector>
#include <string>
#include "hashmap.hpp"

template <typename iterator>
size_t count(iterator begin, iterator end)
{
	size_t i = 0;
	for(;begin!=end;++begin)++i;
	return i;
}

template <typename iterator, typename T>
size_t count_if_equal(iterator begin, iterator end, const T &val)
{
	size_t i = 0;
	for(;begin!=end;++begin)i+=(*begin==val);
	return i;
}



int main () {
	using namespace std;
	using namespace std::chrono;
	using namespace tme2;

	ifstream input = ifstream("WarAndPeace.txt");

	Hashmap<string, int> mots(32768);
	int* found;

	auto start = steady_clock::now();
	std::cout << "Parsing War and Peace" << endl;

	size_t nombre_lu = 0;
	// prochain mot lu
	string word;
	// une regex qui reconnait les caractères anormaux (négation des lettres)
	regex re( R"([^a-zA-Z])");
	while (input >> word) {
		// élimine la ponctuation et les caractères spéciaux
		word = regex_replace ( word, re, "");
		// passe en lowercase
		transform(word.begin(),word.end(),word.begin(),::tolower);
		found = mots.get(word);
		if(!found)
		{
			mots.put(word, 1);
		}
		else
		{
			*found += 1;
		}
		

		/*rank = isPresent(mots, word);
		if(rank==-1)
		{
			pair<string, uint32_t> newWord = make_pair(word, 1);
			mots.push_back(newWord);
		}
		else
		{
			mots[rank] = make_pair(word, get<1>(mots[rank])+1);
		}*/

		// word est maintenant "tout propre"
		/*if (nombre_lu % 100 == 0)
			// on affiche un mot "propre" sur 100
			std::cout << nombre_lu << ": "<< word << endl;
		nombre_lu++;*/
	}
	input.close();

	std::cout << "Finished Parsing War and Peace" << endl;

	auto end = steady_clock::now();
    std::cout << "Parsing took "
              << duration_cast<milliseconds>(end - start).count()
              << "ms.\n";

	//std::vector<std::pair<std::string, int>> mots_freq;
	//mots_freq.reserve(mots.get_elements());
	// question 7 sautée

    cout << "Found a total of " << nombre_lu << " words." << endl;

	cout << "found a total of " << mots.get_elements() << " different words." << endl;

	cout << "war : " << (mots.get("war") ? *mots.get("war") : 0) << endl;
	cout << "peace : " << (mots.get("peace") ? *mots.get("peace") : 0) << endl;
	cout << "toto : " << (mots.get("toto") ? *mots.get("toto") : 0) << endl;

    return 0;
}


