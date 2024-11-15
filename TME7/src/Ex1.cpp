#include <iostream>
#include <unistd.h>
#include <cstring>


int main(int argc, char** argv)
{
    char* cmd[argc+1];
    memcpy(cmd, argv, sizeof(char*)*(argc+1));

    int pfd[2];
    if(pipe(pfd)!=0)
    {
        std::cerr << "Erreur de création du pipe" << std::endl;
        exit(1);
    }
        
    int i;

    for(i = 1; i<argc; ++i)
    {
        if(strcmp(argv[i], "|") == 0)
            break;
    }

    if(i == argc)
    {
        std::cerr << "Erreur : | non trouvé" << std::endl;
        exit(1);
    }

    cmd[i] = NULL;  //On remplace le | par 0x0 pour pouvoir donner directement les arguments sans autre traitement lors d'exec

    char* cmd1[i];
    char* cmd2[argc-i];

    memcpy(cmd1, cmd+1, sizeof(cmd1));
    memcpy(cmd2, cmd+i+1, sizeof(cmd2));

    if(fork() == 0)   //Première commande
    {
        execv(cmd1[0], cmd1);
    }

    if(fork() == 0) //Deuxième commande
    {
        execv(cmd2[0], cmd2);
    }

}