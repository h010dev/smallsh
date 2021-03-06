/**
 * @file SH_cd.h
 * @author Mohamed Al-Hussein
 * @date 04 Feb 2022
 * @brief SH_cd builtin command.
 */
#ifndef SMALLSH_CD_H
#define SMALLSH_CD_H

/**
 * @brief Changes directories into specified dirname, or $HOME if @p dirname
 * is @c NULL.
 * @pre $HOME env variable is set before call when @p dirname is @c NULL.
 * @param dirname directory name to SH_cd into
 */
void SH_cd(char const *dirname);

#endif //SMALLSH_CD_H
