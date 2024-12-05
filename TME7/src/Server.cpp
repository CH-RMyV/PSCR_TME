#include "chat_common.h"
#include <iostream>
#include <string>
#include <unistd.h>

bool terminate = false;

void end_handler(int sig)
{
    if(sig == SIGINT || sig == SIGTERM)
    {
        terminate = true;
    }
}

int main(int argc, char **argv)
{
    int shm_fd;
    struct myshm *shm;
    void *shm_ptr;
    int i, end;
    struct message cur_mess;

    signal(SIGINT, end_handler);

    if (argc < 2)
    {
        std::cerr << "Argument manquant : identifiant du serveur" << std::endl;
        exit(1);
    }

    //============ Allocation de la shm serveur ============
    if ((shm_fd = shm_open(argv[1], O_RDWR | O_CREAT | O_EXCL, 0666)) == -1) //Montage de la shm
    {
        std::cout << "FD mal fermé dans une précédente run. Unlink puis rappel." << std::endl;
        shm_unlink(argv[1]);
        shm_fd = shm_open(argv[1], O_RDWR | O_CREAT | O_EXCL, 0666);
    }

    ftruncate(shm_fd, sizeof(myshm));    //Déclaration de la taille de la shm
    shm_ptr = (mmap(NULL, sizeof(myshm), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));    //récupération du pointeur de la shm
    shm = (myshm*) shm_ptr;

    //============ Initialisation de la shm serveur ============
    shm->read = 0;
    shm->write = 0;
    shm->nb = 0;
    if (sem_init(&(shm->requests), 1, MAX_MESS) != 0)   //Un client prend un permis request, puis le mutex
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
        //Possibilité de dormir ici sur un signal si write = 0 pour réduire l'utilisation CPU ==> à implémenter plus tard
        sem_wait(&(shm->general_mutex));    //Section critique : une requête a été reçue

        for(i=0, end=shm->write; i<end; i++)         //Les clients incrémentent write à l'envoi, read à la réception
        {                                   //Le serveur décrémente write et read au traitement ==> write indique le rang courant dans le tableaux de messages
            cur_mess = shm->messages[i];    //Read peut devenir négatif ==> On traite toutes les requêtes d'un coup quand le mutex est libre

            switch(cur_mess.type)
            {
                case 0:     //Connexion ==> ID dans le message
                    std::cout << "Connexion du client " << cur_mess.content << std::endl;
                    
                    sem_post(&(shm->requests));
                    break;

                case -1:    //Déconnexion ==> ID dans le message
                    std::cout << "Déconnexion du client " << cur_mess.content << std::endl;
                    sem_post(&(shm->requests));
                    break;

                default:    //Message ==> Destinataire dans le type de requête (0 et -1 ne peuvent pas être des destinataires)
                    std::cout << "Envoi d'un message vers " << cur_mess.type << std::endl;
                    sem_post(&(shm->requests));
                    break;
            }
        }
        sem_post(&(shm->general_mutex));    //Fin de section critique, requête traitée
    }

    std::cout << "Fermeture du serveur..." << std::endl;

    //======== Désallocation du segment partagé ========
    sem_destroy(&(shm->requests));
    sem_destroy(&(shm->general_mutex));
    munmap(shm_ptr, sizeof(shm));
    close(shm_fd);
    shm_unlink(argv[1]);
}