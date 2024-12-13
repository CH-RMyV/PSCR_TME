#include "Socket.h"
namespace pr
{
    class ServerSocket
    {
        int socket_fd;

    public:
        ServerSocket(int port);

        int getFD() { return socket_fd; }
        bool isOpen() const { return socket_fd != -1; }

        Socket accept();

        void close();
    };
}
