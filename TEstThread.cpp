#include <thread>
#include <iostream>
#include <unistd.h>


void affiche()
{
    std::cout << "Thread PID : " << getpid() << std::endl;
}

int main(int argc, char** argv)
{
    std::thread t1 = std::thread(affiche);
    t1.detach();
    std::cout << "Main PID : " << getpid() << std::endl;
}
