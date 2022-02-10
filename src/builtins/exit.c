//
// Created by mohamed on 2/3/22.
//

#include "builtins/exit.h"
#include "job-control/job-control.h"

void exit_(void)
{
        job_table.killall(&job_table);
}
