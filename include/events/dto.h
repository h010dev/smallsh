/**
 * @file dto.h
 * @author Mohamed Al-Hussein
 * @date 06 Feb 2022
 * @brief Contains data structures containing signal-generated data that
 * will transferred over a dedicated channel.
 */
#ifndef SMALLSH_DTO_H
#define SMALLSH_DTO_H

#include <sys/types.h>

/**
 * @brief A @c SigchldDTO contains relevant data about a SIGCHLD event that
 * the global job table will use to update its state.
 */
typedef struct {
        pid_t pid; /**< PID of child that sent the signal */
        int status; /**< exit status of child */
} SH_SigchldDTO;

#endif //SMALLSH_DTO_H
