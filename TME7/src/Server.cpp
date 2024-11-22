#include "chat_common.h"
#include <iostream>
#include <string>
#include <unordered_map>

bool terminate = false;

void end_handler(int sig)
{
    terminate = true;
}

int main(int argc, char **argv)
{
    int shared_memory;
    struct myshm *shm;
    void *shm_ptr;
    int i;
    message cur_mess;

    std::unordered_map<std::string, message*> send, receive;

    if (argc < 2)
    {
        std::cerr << "Argument manquant : identifiant du serveur" << std::endl;
        exit(1);
    }

    //============ Allocation de la shm serveur ============
    if ((shared_memory = shm_open(argv[1], O_RDWR | O_CREAT | O_EXCL, 0666)) == -1)
    {
        std::cout << "FD mal fermé dans une précédente run. Unlink puis rappel." << std::endl;
        shm_unlink(argv[1]);
        shared_memory = shm_open(argv[1], O_RDWR | O_CREAT | O_EXCL, 0666);
    }

    ftruncate(shared_memory, sizeof(myshm));
    shm_ptr = (mmap(NULL, sizeof(myshm), PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory, 0));

    //============ Initialisation de la shm serveur ============
    shm->read = 0;
    shm->write = 0;
    shm->nb = 0;
    if (sem_init(&(shm->requests), 1, 0) != 0)
    {
        std::cerr << "Erreur de création du sémaphore requests" << std::endl;
    }
    if (sem_init(&(shm->general_mutex), 1, 1) != 0)
    {
        std::cerr << "Erreur de création du sémaphore general_mutex" << std::endl;
    }

    //============ Lecture des requêtes ============
    while (!terminate) 
    {
        sem_wait(&(shm->requests));
        sem_wait(&(shm->general_mutex));    //Section critique : une requête a été reçue

        for(i=0; i<shm->write; i++)         //Les clients incrémentent write à l'envoi, read à la réception
        {                                   //Le serveur décrément write et read au traitement ==> write indique le rang courant dans le tableaux de messages
            cur_mess = shm->messages[i];    //Read peut devenir négatif ==> On traite toutes les requêtes d'un coup quand le mutex est libre

            if(cur_mess.type == 0)  // Requête de connexion
            {
                std::cout << "Connexion du client " << std::stoi(cur_mess.content) << std::endl;
                continue;
            }

            if(cur_mess.type == 1)
            {
                continue;
            }

        }

        sem_post(&(shm->general_mutex));    //Fin de section critique, requête traitée
    }

    //======== Désallocation du segment partagé ========
    munmap(shm_ptr, sizeof(shm));
    close(shared_memory);
    shm_unlink(argv[1]);
}