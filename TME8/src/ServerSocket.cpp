#include "ServerSocket.h"

using namespace pr;

ServerSocket::ServerSocket(int port) : socket_fd(-1)
{
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = port;
    addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sock_fd, (struct sockaddr*) &addr, sizeof(addr)) < 0)
    {
        std::cerr << "Erreur de bind" << std::endl;
        ::close(sock_fd);
        exit(1);
    }

    if(listen(sock_fd, 10) < 0)
    {
        std::cerr << "Erreur de listen" << std::endl;
        ::close(sock_fd);
        exit(1);
    }

    this->socket_fd = sock_fd;  //On est prêt à accepter les connexions
}

Socket ServerSocket::accept()
{
    struct sockaddr_in exped;
    unsigned int exp_len = sizeof(exped);
    int fdcom = ::accept(this->socket_fd, (struct sockaddr*) &exped, &exp_len);
    if(fdcom < 0)
    {
        std::cerr << "Erreur d'accept" << std::endl;
        return Socket(-1);
    }

    std::cout << "Connexion de " << &exped <<std::endl;
    return Socket(fdcom);
}

void ServerSocket::close()
{
    if(isOpen)
    {
        ::close(socket_fd);
        socket_fd = -1;
    }
}