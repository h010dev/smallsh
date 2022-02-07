//
// Created by mohamed on 2/6/22.
//

#ifndef SMALLSH_DTOS_H
#define SMALLSH_DTOS_H

#include <sys/types.h>

typedef struct {
        pid_t chld_pid;
        int chld_status;
} SigchldDTO;

#endif //SMALLSH_DTOS_H
