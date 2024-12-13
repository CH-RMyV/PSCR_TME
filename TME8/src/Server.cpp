#include "ServerSocket.h"

int main(int argc, char* argv[])
{
    int port = atoi(argv[1]);

    ServerSocket sock(port);

    if(!sock.isOpen())
    {
        std::cerr << "Erreur d'ouverture" << std::endl;
        exit(1);
    }

    while(true)
    {
        Socket scom = sock.accept();
        if(!scom.isOpen()) break;

        int val;
        if(read(scom.getFD(), &val, sizeof(val)) == sizeof(val));
        {
            std::cout << "Lu " << val << std::endl;
        }
        val++;
        write(scom.getFD(), &val, sizeof(val));
        scom.close();
    }

    int val = 42;
    write(sock.getFD(), &val, sizeof(val));
    //send et write font essentiellement la même chose. write n'existe pas sur windows, send est plus homogène
    send(sock.getFD(), &val, sizeof(val), 0);

    int res;
    recv(sock.getFD(), &res, sizeof(res), 0);
    sock.close();
}
