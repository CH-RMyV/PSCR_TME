#pragma once

#include <cstring> // size_t,memset
#include <semaphore.h>
#include <iostream>

namespace pr
{

#define STACKSIZE 100

	template <typename T>
	class Stack
	{
		T tab[STACKSIZE];
		size_t sz;
		sem_t give_permits, take_permits, general_mutex;
		bool blocked;

	public:
		Stack() : sz(0), blocked(true)
		{
			memset(tab, 0, sizeof tab);
			if(sem_init(&give_permits, 1, STACKSIZE)!=0)
			{
				std::cerr << "Erreur de création du sémaphore push" << std::endl;
			}
			if(sem_init(&take_permits, 1, 0)!=0)
			{
				std::cerr << "Erreur de création du sémaphore pop" << std::endl;
			}
			if(sem_init(&general_mutex, 1, 1)!=0)
			{
				std::cerr << "Erreur de création du mutex général" << std::endl;
			}
		}

		T pop()
		{
			// bloquer si vide
			sem_wait(&take_permits);
			sem_wait(&general_mutex);
			if(sz<=0)
			{
				return (T)NULL;
			}
			T toret = tab[--sz];
			sem_post(&general_mutex);
			sem_post(&give_permits);
			return toret;
		}

		void push(T elt)
		{
			if(!blocked)
			{
				return;
			}
			// bloquer si plein
			sem_wait(&give_permits);
			sem_wait(&general_mutex);
			tab[sz++] = elt;
			sem_post(&general_mutex);
			sem_post(&take_permits);
		}

		void unblock()
		{
			sem_wait(&general_mutex);
			blocked = false;
			sem_post(&take_permits);
			sem_post(&general_mutex);
		}

		bool getBlocking()
		{
			sem_wait(&general_mutex);
			bool ret = blocked;
			sem_post(&general_mutex);
			return ret;
		}

		size_t getSz()
		{
			sem_wait(&general_mutex);
			size_t ret = sz;
			sem_post(&general_mutex);
			return ret;
		}
	};

}
