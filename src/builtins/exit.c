//
// Created by mohamed on 2/3/22.
//

#include "builtins/exit.h"
#include "core/shell-attrs.h"

void exit_(void)
{
        job_table.killall(&job_table);
}
