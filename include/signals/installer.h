/**
 * @file installer.h
 * @author Mohamed Al-Hussein
 * @date 05 Feb 2022
 * @brief Contains functions for installing signals.
 *
 * Ideas presented here were retrieved from the following sources:
 * https://www.gnu.org/software/libc/manual/html_node/Initializing-the-Shell.html
 * https://www.gnu.org/software/libc/manual/html_node/Launching-Jobs.html
 */
#ifndef SMALLSH_INSTALLER_H
#define SMALLSH_INSTALLER_H

/**
 * @brief Installs job control signals for the shell.
 */
void SH_InstallerInstallJobControlSignals(void);

/**
 * @brief Installs process signals for foreground/background child processes.
 * @param foreground whether or not process will run in foreground
 */
void SH_InstallerInstallChildProcessSignals(bool foreground);

/**
 * @brief Installs a SIGCHLD handler for the shell.
 */
void SH_InstallerInstallSigchldHandler(void);

/**
 * @brief Installs a SIGTSTP handler for the shell.
 */
void SH_InstallerInstallSigtstpHandler(void);

#endif //SMALLSH_INSTALLER_H
