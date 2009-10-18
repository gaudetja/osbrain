#ifndef _SCHED_H
#define _SCHED_H

/* Scheduler
 * This contains the function headers
 * for the scheduler module, including
 * the ready queue, and the wait queue
 */

int sched(struct ProcessControl * PCB, int io, curlyqueue_t * rqueue);

#endif