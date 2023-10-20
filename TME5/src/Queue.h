#ifndef SRC_QUEUE_H_
#define SRC_QUEUE_H_

#include <cstdlib>
#include <mutex>
#include <condition_variable>

namespace pr {

// MT safe version of the Queue, non blocking.
template <typename T>
class Queue {
	T ** tab;
	const size_t allocsize;
	size_t begin;
	size_t sz;
	mutable std::mutex m;
    std::condition_variable cv;                 //pour recevoir un recursive mutex
    bool isBlocking;

	// fonctions private, sans protection mutex car le seul moyen de l'appeler c'est dans push/pop donc on a deja le lock de la classe
	bool empty() const {
		return sz == 0;
	}
	bool full() const {
		return sz == allocsize;
	}
public:
	Queue(size_t size) :allocsize(size), begin(0), sz(0) , isBlocking(true){
		tab = new T*[size];
		memset(tab, 0, size * sizeof(T*));
	}
	size_t size() const {
		std::unique_lock<std::mutex> lg(m);
		return sz;
	}
	T* pop() {
		std::unique_lock<std::mutex> lg(m);
		while (empty() && isBlocking) {
            cv.wait(m);
		}
        
        if(!isBlocking && empty()) return nullptr;
        
		auto ret = tab[begin];
		tab[begin] = nullptr;
		sz--;
		begin = (begin + 1) % allocsize;
        cv.notify_one();
		return ret;
	}
	bool push(T* elt) {
		std::unique_lock<std::mutex> lg(m);
		while (full()) {
            cv.wait(m);
		}
		tab[(begin + sz) % allocsize] = elt;
		sz++;
        cv.notify_one();
		return true;
	}
    
    void setisBlocking(bool b){
        std::unique_lock<std::mutex> lg(m);
        isBlocking = b;
    }
	~Queue() {
		// ?? lock a priori inutile, ne pas detruire si on travaille encore avec
		for (size_t i = 0; i < sz; i++) {
			auto ind = (begin + i) % allocsize;
			delete tab[ind];
		}
		delete[] tab;
	}
};

}

#endif /* SRC_QUEUE_H_ */
