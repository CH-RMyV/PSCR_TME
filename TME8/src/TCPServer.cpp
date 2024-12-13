#include "TCPServer.h"
#include <vector>
#include <thread>
#include <cstring>

using namespace pr;

void handleClient(Socket scom)
{
    char bloc[PAQUET_SIZE];
    int fd = scom.getFD();
    ssize_t lu;

    memset(bloc, 0, PAQUET_SIZE);

    while(1)
    {
        lu = read(fd, &bloc, PAQUET_SIZE);
        if(lu == 0)
        {

        }
    }
}

bool TCPServer::startServer(int port)   //on stocke le ServerSocket dans ss (attribut)
{
    if((ss = new ServerSocket(port))->getFD() == -1)
    {
        std::cerr << "Erreur de création du socket d'écoute" << std::endl;
        return false;
    }
    return true;
}