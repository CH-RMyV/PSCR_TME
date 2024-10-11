#include "Banque.h"

#include <iostream>

using namespace std;

namespace pr {

void Banque::transfert(size_t deb, size_t cred, unsigned int val) {
	Compte & debiteur = comptes[deb];
	Compte & crediteur = comptes[cred];
	recursive_mutex & mutex_d = debiteur.getMutex();
	recursive_mutex & mutex_c = crediteur.getMutex();
	lock(mutex_d, mutex_c);
	if (debiteur.debiter(val)) {
		crediteur.crediter(val);
	}
	mutex_d.unlock();
	mutex_c.unlock();
}
size_t Banque::size() const {
	return comptes.size();
}
bool Banque::comptabiliser (int attendu) const {
	int bilan = 0;
	int id = 0;
	for (auto & compte : comptes) {
		recursive_mutex &c = compte.getMutex();
		c.lock();
		if (compte.getSolde() < 0) {
			cout << "Compte " << id << " en négatif : " << compte.getSolde() << endl;
			getchar();
		}
		bilan += compte.getSolde();
		
		id++;
	}

	for(auto & compte:comptes)
	{
		recursive_mutex &c = compte.getMutex();
		c.unlock();
	}
	if (bilan != attendu) {
		cout << "Bilan comptable faux : attendu " << attendu << " obtenu : " << bilan << endl;
		getchar();
	}
	return bilan == attendu;
}
}
