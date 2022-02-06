/**
 * @date 05 Feb 2022
 */
#ifndef SMALLSH_INSTALLER_H
#define SMALLSH_INSTALLER_H

/* Source: https://www.gnu.org/software/libc/manual/html_node/Initializing-the-Shell.html */
void installer_install_job_control_signals(void);

/* Source: https://www.gnu.org/software/libc/manual/html_node/Launching-Jobs.html */
void installer_uninstall_job_control_signals(void);

#endif //SMALLSH_INSTALLER_H
