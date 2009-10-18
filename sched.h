#ifndef _SCHED_H
#define _SCHED_H


/* Scheduler
 * This contains the function headers
 * for the scheduler module, including
 * the ready queue, and the wait queue
 */

u_int8_t sched(PCB* ControlBlock, int io);
void BuildQueue(NPID);

#endif
