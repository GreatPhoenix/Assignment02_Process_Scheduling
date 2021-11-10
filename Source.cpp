#include <queue>

int N = 10;


int main() 
{
        setJobQueues();           /* Set up the priority job queues with chosen file and data structure */

    if (pid = fork() > 0) {/* Parent, jobGenerator process */

        jobGenerator(…); /* generate random jobs and put them into the priority queues
                                             The priority queues must be protected in a critical region */
    }
    else {/* Child, job scheduler process */

        jobScheduler(…); /* schedule and execute the jobs. */

        exit(0);
    }
    return (1);
}

void jobScheduler(…) 
{
        while (i < N) 
        {           /* schedule and run maximum N jobs */

            n = selectJob(…)      /* pick a job from the job priority queues */

                if (n > 0) 
                { /* valid job id */

                    if (pid = fork() == 0) 
                    { /* child worker process */
                        executeJob(…);      /* execute the job */
                        exit(0);
                    }
                }
            i++;
        }
}

void executeJob(...)
{

}

void selectJob(...)
{

}