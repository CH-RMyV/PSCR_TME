#include "chat_common.h"
#include <iostream>
#include <string>
#include <unistd.h>
#include <thread>
#include <condition_variable>
#include <mutex>

bool terminate = false;
struct message *Tx, *Rx;
std::condition_variable received;

void sig_handler(int sig)
{

    if (sig == SIGTERM || sig == SIGINT)
    {
        terminate = true;
        received.notify_one();
        std::cout << "Fermeture demandée par le serveur" << std::endl;
    }
    else if (sig == SIGUSR1)
    {
        received.notify_one();
        signal(SIGUSR1, sig_handler);
    }
}

void send(struct myshm *server_shm, struct message *Tx)
{
    while (!terminate) // Boucle des messages standards
    {
        std::string mess;
        long dest;

        std::cout << "PID du destinataire : ";
        std::cin >> dest;
        std::cout << "\nMessage : " << std::endl;
        std::cin >> mess;

        if (mess.size() > 1023)
        {
            std::cout << "Message trop long. Troncature aux premiers 1023 caractères." << std::endl;
            mess = mess.substr(0, 1022);
        }

        Tx->type = dest;
        strcpy(Tx->content, mess.c_str());

        sem_wait(&(server_shm->requests));
        sem_wait(&(server_shm->general_mutex));

        server_shm->messages[(server_shm->nb) % MAX_MESS].type = 0;
        strcpy(server_shm->messages[(server_shm->nb) % MAX_MESS].content, Tx->content);
        (server_shm->nb)++;
        (server_shm->write)++;

        sem_post(&(server_shm->general_mutex));
    }
}

void receive(struct myshm *server_shm, struct message *Rx)
{
    std::mutex m;
    while (!terminate)
    {
        std::unique_lock<std::mutex> lg(m);
        received.wait(lg, [Rx]
                      { return Rx->type != 0 || terminate; }); // On ne peut pas demander la connexion à un client
        std::cout << Rx->content << std::endl;
        memset(Rx->content, 0, 1024); // vidage de Rx
        Rx->type = 0;                 // reset de type pour réattendre sur received
    }
}

int main(int argc, char **argv)
{
    int server_fd, tx_fd, rx_fd;

    struct myshm *server_shm;
    void *shm_ptr;

    std::cout << "Identifiant client : " << getpid() << std::endl;

    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);
    signal(SIGUSR1, sig_handler);

    if (argc < 2)
    {
        std::cerr << "Argument manquant : identifiant du serveur" << std::endl;
        exit(1);
    }

    //================= shm du serveur  : déjà existant =================
    if ((server_fd = shm_open(argv[1], O_RDWR, 0666)) == -1) // Montage de la shm serveur
    {
        std::cerr << "Serveur inexistant. " << std::endl;
        exit(1);
    }

    shm_ptr = (mmap(NULL, sizeof(myshm), PROT_READ | PROT_WRITE, MAP_SHARED, server_fd, 0));
    server_shm = (myshm *)shm_ptr;

    //================= Création du FD tx =================
    if ((tx_fd = shm_open(("/" + std::to_string(getpid()) + "TX").c_str(), O_RDWR | O_CREAT | O_EXCL, 0666)) == -1) // Montage de la shm tx
    {
        std::cout << "FD mal fermé dans une précédente run. Unlink puis rappel." << std::endl;
        shm_unlink(("/" + std::to_string(getpid()) + "TX").c_str());
        tx_fd = shm_open(("/" + std::to_string(getpid()) + "TX").c_str(), O_RDWR | O_CREAT | O_EXCL, 0666);
    }

    //================= Création du FD rx =================
    if ((rx_fd = shm_open(("/" + std::to_string(getpid()) + "RX").c_str(), O_RDWR | O_CREAT | O_EXCL, 0666)) == -1) // Montage de la shm tx
    {
        std::cout << "FD mal fermé dans une précédente run. Unlink puis rappel." << std::endl;
        shm_unlink(("/" + std::to_string(getpid()) + "RX").c_str());
        tx_fd = shm_open(("/" + std::to_string(getpid()) + "RX").c_str(), O_RDWR | O_CREAT | O_EXCL, 0666);
    }

    //================= Déclaration de la taille en shm de TX et RX =================
    ftruncate(tx_fd, sizeof(message));
    ftruncate(rx_fd, sizeof(message));

    //================= Récupération des pointeurs de TX et RX =================
    shm_ptr = (mmap(NULL, sizeof(message), PROT_READ | PROT_WRITE, MAP_SHARED, tx_fd, 0));
    Tx = (message *)shm_ptr;

    shm_ptr = (mmap(NULL, sizeof(message), PROT_READ | PROT_WRITE, MAP_SHARED, rx_fd, 0));
    Rx = (message *)shm_ptr;

    //================= Connexion du client au serveur + vidage Rx =================
    memset(Rx->content, 0, 1024);
    Rx->type = 0;

    Tx->type = 0;
    strcpy(Tx->content, std::to_string(getpid()).c_str());
    sem_wait(&(server_shm->requests));
    sem_wait(&(server_shm->general_mutex));

    server_shm->messages[(server_shm->nb) % MAX_MESS].type = 0;
    strcpy(server_shm->messages[(server_shm->nb) % MAX_MESS].content, Tx->content);
    (server_shm->nb)++;
    (server_shm->write)++;

    sem_post(&(server_shm->general_mutex));

    //================= Threads de lecture + écriture =================
    std::thread sender = std::thread(send, server_shm, Tx);
    std::thread receiver = std::thread(receive, server_shm, Rx);

    sender.join();
    receiver.join();

    //================= Terminaison du client via client (ctrl+c) ou via serveur (SIGTERM) =================

    
}