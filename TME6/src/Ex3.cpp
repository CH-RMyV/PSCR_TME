#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include "rsleep.h"

volatile int PV = 5;

struct sigaction act;

void handler(int n)
{
    if(PV<=0)
    {
        printf("Mort du processus %d\n", getpid());
        exit(1);
    }
    else
    {
        PV--;
    }
    printf("PV restants : %d pour process %d\n", PV, getpid());
}

void attaque(pid_t adversaire)
{
    int killsig;
    act.sa_handler = &handler;
    sigaction(SIGINT, &act, NULL);
    randsleep();
    killsig = kill(adversaire, SIGINT);
    if(killsig != 0)
    {
        printf("victoire du processus %d\n", getpid());
        exit(0);
    }
}

void defense()
{
    act.sa_handler = SIG_IGN;
    sigaction(SIGINT, &act, NULL);
    randsleep();
}

int main(int argc, char* argv[])
{
    pid_t adversaire;

    srand(time(NULL));

    memset(&act, 0, sizeof act);
    act.sa_handler = SIG_IGN;
    sigfillset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);

    if((adversaire = fork()) == 0)
    {
        srand(rand());
        adversaire = getppid();
    }

    while(true)
    {
        printf("%d attaque %d\n", getpid(), adversaire);
        attaque(adversaire);
        printf("%d se défend\n", getpid());
        defense();
    }
}