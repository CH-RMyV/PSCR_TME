#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/wait.h>

int main(int argc, char **argv)
{
    char *cmd[argc + 1];
    memcpy(cmd, argv, sizeof(char *) * (argc + 1));
    pid_t p1, p2;

    int pfd[2];
    if (pipe(pfd) != 0)
    {
        std::cerr << "Erreur de création du pipe" << std::endl;
        exit(1);
    }

    int i;

    for (i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "|") == 0)
            break;
    }

    if (i == argc)
    {
        std::cerr << "Erreur : | non trouvé" << std::endl;
        exit(1);
    }

    cmd[i] = NULL; // On remplace le | par 0x0 pour pouvoir donner directement les arguments sans autre traitement lors d'exec

    char *cmd1[i];
    char *cmd2[argc - i];

    memcpy(cmd1, cmd + 1, sizeof(cmd1));
    memcpy(cmd2, cmd + i + 1, sizeof(cmd2));

    if ((p1 = fork()) == 0) // Première commande
    {
        dup2(pfd[1], STDOUT_FILENO); // écrasement du pipe stdout par notre pipe d'origine
        /*
        dup2(old_fd, new_fd) ==> duplique les FD de old_fd vers new_fd. si new_fd existait déjà, il est écrasé silencieusement
        */
        close(pfd[0]); // fermeture du pipe d'origine
        close(pfd[1]);
        execvp(cmd1[0], cmd1);
    }
    else
    {
        if ((p2 = fork()) == 0) // Deuxième commande
        {
            dup2(pfd[0], STDIN_FILENO); // écrasement du pipe STDIN par notre pipe d'origine
            close(pfd[0]);              // Fermeture des pipes d'origine du père
            close(pfd[1]);
            execvp(cmd2[0], cmd2);
        }
        else
        {
            close(pfd[0]);
            close(pfd[1]);
            waitpid(p1, NULL, 0);
            waitpid(p2, NULL, 0);
        }
    }
}