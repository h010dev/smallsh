//
// Created by mohamed on 2/6/22.
//

#ifndef SMALLSH_DTO_H
#define SMALLSH_DTO_H


#include <sys/types.h>

typedef struct {
        pid_t chld_pid;
        int chld_status;
} SigchldDTO;


#endif //SMALLSH_DTO_H
