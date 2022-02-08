//
// Created by mohamed on 2/3/22.
//
#include <stdio.h>

//https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html#tag_18_08_02
int status(int status)
{
        fprintf(stdout, "exit value %d\n", status);
        return 0;
}
