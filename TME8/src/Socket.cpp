#include "Socket.h"

using namespace pr;

void Socket::connect(const std::string& host, int port)
{
    /*DNS : getaddrinfo(char* host, char* service, struct addrinfo* hints, struct addrinfo** res)
    res peut contenir plusieurs réponses, par exemple une adresse en IPV4 et une en IPV6
    service : "http", autre protocole, ou nullptr
    hints : masque permettant d'indiquer quels champs on veut remplir*/
    struct addrinfo *res;
    struct in_addr ip;

    getaddrinfo(host.c_str(), nullptr, nullptr, &res);
    for(addrinfo* rp = res; rp; rp=rp->ai_next)
    {
        if(rp->ai_family==AF_INET)
        {
            ip = ((struct sockaddr_in*) rp->ai_addr) -> sin_addr;
            break;
        }
    }
    freeaddrinfo(res);
    connect(ip, port);  //connect de notre namespace, qu'on recrée
}

void Socket::connect(in_addr ip, int port)
{
    int fd_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);    //SOCK_STREAM pour TCP ou SOCK_DGRAM pour l'UDP; IPPROTO_TCP peut être remplacé par 0 pour laisser choisir automatiquement
    if(fd_sock <0)
    {
        std::cerr << "Erreur d'ouverture du socket" << std::endl;
        exit(1);
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET; 
    addr.sin_port = port;
    addr.sin_addr = ip;
    if(::connect(fd_sock, (struct sockaddr*) &addr, sizeof(struct sockaddr_in)) < 0) //connect de la std
    {
        std::cerr << "Erreur de connect" << std::endl;
        ::close(fd_sock);
        exit(1);
    }
    this->fd = fd_sock;
    return; 
}

void Socket::close()
{
    if(isOpen)
    {
        ::shutdown(fd, 2);  //0 : SHUT_RD; 1 : SHUT_WR; 2 : SHUT_RDRW
        ::close(fd);
    }
}