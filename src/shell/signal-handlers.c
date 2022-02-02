//
// Created by mohamed on 1/31/22.
//

#include <unistd.h>
#include <string.h>

#include "shell/signal-handlers.h"


void handle_SIGINT(int sig)
{
        (void) sig;

        char *msg = "\n: ";
        size_t msg_len = 4;

        write(STDOUT_FILENO, msg, msg_len);
}

void handle_SIGTSTP(int sig)
{
        (void) sig;
        char *msg;

        msg = "\nCaught SIGTSTP\n: ";
        write(STDOUT_FILENO, msg, strlen(msg));
}
