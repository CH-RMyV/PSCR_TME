#include "ServerSocket.h"

int main(int argc, char* argv[])
{
    const char* host = argv[1];
    int port = atoi(argv[2]);

    Socket sock;
    sock.connect(host, port);
    if(!sock.isOpen())
    {
        std::cerr << "Erreur de connexion" << std::endl;
        exit(1);
    }

    int val = 42;
    write(sock.getFD(), &val, sizeof(val));
    //send et write font essentiellement la même chose. write n'existe pas sur windows, send est plus homogène
    send(sock.getFD(), &val, sizeof(val), 0);

    int res;
    recv(sock.getFD(), &res, sizeof(res), 0);

    std::cout << "Lu " << res << std::endl;
    sock.close();
}
