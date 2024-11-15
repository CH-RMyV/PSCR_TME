#include <iostream>
#include <unistd.h>
#include <cstring>


int main(int argc, char** argv)
{
    char* cmd[argc+1];
    memcpy(cmd, argv, sizeof(char*)*(argc+1));

    //std::cout << cmd[1] << std::endl;
    //std::cout << cmd[2] << std::endl;

    int pfd[2];
    if(pipe(pfd)!=0)
    {
        std::cerr << "Erreur de création du pipe" << std::endl;
        exit(1);
    }
        
    int i;

    for(i = 1; i<argc; ++i)
    {
        if(strcmp(argv[i], "|"))
            break;
    }

    //cmd 1 va de argv[1] à argv[i-1]
    //cmd 2 va de argv[i+1] à argv[argc-1]

    if(i == argc)
    {
        std::cerr << "Erreur : | non trouvé" << std::endl;
        exit(1);
    }

    cmd[i] = NULL;

    char* cmd1[i+2];
    char* cmd2[argc-i+2];

    memcpy(cmd1, argv[1], sizeof(char*)*(i+2));
    memcpy(cmd2, argv[i+1], sizeof(char*)*(argc-i+2));

}