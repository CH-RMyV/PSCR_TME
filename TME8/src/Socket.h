#pragma once
#include <sys/socket.h>
#include <string>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PAQUET_SIZE 65535

namespace pr
{
    std::ostream &operator<<(std::ostream &o, struct sockaddr_in *addr)
    {
        /*Résolution du nom d'hôte : getnameinfo(struct sockaddr* addr, socklen_t len,
                                                char* host, size_t hostlen, char* service,
                                                size_t servlen, int flags)*/

        char host[1024];
        if (getnameinfo((sockaddr *)addr, sizeof(struct sockaddr_in), host, 1024, nullptr, 0, 0) != 0)
        {
            std::cerr << "Erreur de requête" << std::endl;
        }

        o << host << std::endl;
        o << inet_ntoa(addr->sin_addr) << " : " << ntohs(addr->sin_port) << std::endl;
    }

    class Socket
    {
        int fd;

    public:
        Socket() : fd(-1) {}
        Socket(int fd) : fd(fd) {}

        void connect(const std::string &host, int port);
        void connect(in_addr ip, int port);

        bool isOpen() const { return fd != -1; }
        int getFD() const { return fd; }

        void close();
    };
}
