//
// Created by mohamed on 1/31/22.
//

#ifndef SMALLSH_SIGNAL_HANDLERS_H
#define SMALLSH_SIGNAL_HANDLERS_H

#include <signal.h>

void handle_SIGINT(int sig);
void handle_SIGTSTP(int sig);

#endif //SMALLSH_SIGNAL_HANDLERS_H
