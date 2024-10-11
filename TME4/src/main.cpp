#include "Banque.h"
#include <cstdlib>
#include <ctime>
#include <random>
#include <iostream>

using namespace std;

const int NB_THREAD = 10;
const int MAX = 100;
const int MAX_SOLDE = 200;

void start_thread(pr::Banque& bq)
{
	std::random_device rd;
	static thread_local std::mt19937 gen(rd());
	std::uniform_int_distribution<int> id(0, bq.size()-1);
	std::uniform_int_distribution<int> mt(0, MAX_SOLDE-1);
	mutex affichage;
	
	for(int i = 0; i < 100000; ++i)
	{
		int id_deb = id(gen);
		int id_cred = id(gen);
		affichage.lock();
		std::cout << "Transferring from debitor " << id_deb << " to creditor " << id_cred << std::endl;
		affichage.unlock();
		bq.transfert(id(gen), id(gen), mt(gen));
	}
}

void thread_compta(pr::Banque& bq, bool& stop_tk)
{
	int attendu = bq.size()*MAX_SOLDE;
	while(!stop_tk)
	{
		bq.comptabiliser(attendu);
		std::this_thread::sleep_for(10ms);
	}
	return;
}

int main () {
	std::srand(std::time(nullptr));
	int N = std::rand() % MAX + 2;
	pr::Banque bq((size_t)N, (size_t)MAX_SOLDE);
	bool stop_token = false;

	thread comptabilite(&thread_compta, std::ref(bq), std::ref(stop_token));
	comptabilite.detach();

	vector<thread> threads;
	for(int i  = 0; i<10; ++i)
	{
		threads.emplace_back(&start_thread, std::ref(bq));
	}

	for (auto & t : threads) {
		t.join();
	}

	stop_token = true;	//arrête l'exécution du thread de comptabilité

	return 0;
}
