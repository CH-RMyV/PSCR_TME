#include "Stack.h"
//#include <iostream>	déjà dans Stack.h
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <sys/mman.h>
#include <fcntl.h>

using namespace std;
using namespace pr;

#define M 100
#define N 100

void main_handler(int sig)
{
	std::cout << "\nArrêt demandé..." << std::endl;
}

void producteur (Stack<char> * stack) {
	char c ;
	while (stack->getBlocking()) {
		cin.get(c);
		stack->push(c);
	}
}

void consomateur (Stack<char> * stack) {
	while (true) {
		char c = stack->pop();
		if(c == '\0')
		{
			break;
		}
		cout << c << std::flush;

	}
}

int main () {
	Stack<char> * s;
	int shared_memory;
	void* shm_ptr;
	pid_t fork_res;
	std::vector<pid_t> consumers, producers;

	sigset_t set;
    sigfillset(&set);
	sigdelset(&set, SIGINT);

	if((shared_memory = shm_open("/stack_mem", O_RDWR|O_CREAT|O_EXCL, 0666))==-1)
	{
		std::cout << "FD mal fermé dans une précédente run. Unlink puis rappel." << std::endl;
		shm_unlink("/stack_mem");
		shared_memory = shm_open("/stack_mem", O_RDWR|O_CREAT|O_EXCL, 0666);
	}
	ftruncate(shared_memory, sizeof(Stack<char>));
	shm_ptr = (mmap(NULL, sizeof(Stack<char>), PROT_READ|PROT_WRITE, MAP_SHARED, shared_memory, 0));	//Allocation du segement pour ranger le stack en shared memory
	s = new (shm_ptr) Stack<char>();	//Initialisation du stack en shared memory

	for(int i=0; i<M; ++i)
	{
		if((fork_res=fork())==0)
		{
			consomateur(s);
			close(shared_memory);
			exit(0);
		}
		consumers.push_back(fork_res);
	}

	for(int i=0; i<N; ++i)
	{
		if((fork_res=fork())==0)
		{
			producteur(s);
			close(shared_memory);
			exit(0);
		}
		producers.push_back(fork_res);
	}

	signal(SIGINT, main_handler);
	sigsuspend(&set);

	for(pid_t prod:producers)
	{
		std::cout << "Arrêt du producteur " << prod << "..." <<std::endl;
		kill(prod, SIGKILL);
		wait(NULL);
	}
	for(pid_t cons:consumers)
	{
		std::cout << "Arrêt du consommateur " << cons << "..." <<std::endl;
		s -> unblock();
		wait(NULL);
	}

	s -> ~Stack();
	munmap(shm_ptr, sizeof(Stack<char>));
	close(shared_memory);
	shm_unlink("/stack_mem");
	return 0;
}

