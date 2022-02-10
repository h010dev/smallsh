/**
 * @file dto.h
 * @author Mohamed Al-Hussein
 * @date 09 Feb 2022
 * @brief Contains data structures containing signal-generated data that
 * will transferred over a dedicated channel.
 */
#include <sys/types.h>

/**
 * @brief A @c SigchldDTO contains relevant data about a SIGCHLD event that
 * the global job table will use to update its state.
 */
typedef struct {
        pid_t chld_pid; /**< PID of child that sent the signal */
        int chld_status; /**< exit status of child */
} SigchldDTO;
