#pragma once
#include <iostream>
#include <fstream>
#include <regex>
#include <chrono>
#include <vector>
#include <string>
#include "hashmap.hpp"



int isPresent(const std::vector<std::pair<std::string, uint32_t>> &vec, const std::string &str)
/*Renvoie -1 si non trouvé, renvoie l'indice du mot si trouvé*/
{
	int i;
	for(i = 0; i<vec.size(); ++i)
	{
		if(str == std::get<0>(vec[i])) return i;
	}
	return -1;
}

int main () {
	using namespace std;
	using namespace std::chrono;
	using namespace tme2;

	Hashmap<int, int> test(10);
	int* testget;
	test.put(638746, 50);
	testget = test.get(638746);
	testget = test.get(0);
	test.put(28000, 45);
	test.put(76452, 40);
	test.put(4736958, 612);
	test.grow();


	ifstream input = ifstream("WarAndPeace.txt");
	int rank;
	vector<pair<string, uint32_t>> mots;

	auto start = steady_clock::now();
	cout << "Parsing War and Peace" << endl;

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

		rank = isPresent(mots, word);
		if(rank==-1)
		{
			pair<string, uint32_t> newWord = make_pair(word, 1);
			mots.push_back(newWord);
		}
		else
		{
			mots[rank] = make_pair(word, get<1>(mots[rank])+1);
		}

		// word est maintenant "tout propre"
		if (nombre_lu % 100 == 0)
			// on affiche un mot "propre" sur 100
			cout << nombre_lu << ": "<< word << endl;
		nombre_lu++;
	}
	input.close();

	cout << "Finished Parsing War and Peace" << endl;

	auto end = steady_clock::now();
    cout << "Parsing took "
              << duration_cast<milliseconds>(end - start).count()
              << "ms.\n";

    cout << "Found a total of " << nombre_lu << " words." << endl;

	cout << "found a total of " << mots.size() << " different words." << endl;

    return 0;
}


