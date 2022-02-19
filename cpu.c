#include "oslab.h"
#include <stdlib.h>
struct PCB NULLPCB = {0, 0, 0, 0, 0, 0, 0};

void remove_pcb(struct PCB array[], int index, int array_length);

// Priority-based Preemptive Scheduler
struct PCB handle_process_arrival_pp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, struct PCB new_process, int timestamp)
{
    // No current process
    if (&current_process == &NULLPCB)
    {
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;
        return new_process;
    }

    // current process is running
    if (current_process.process_priority > new_process.process_priority)
    {
        new_process.execution_endtime = 0;
        new_process.execution_starttime = 0;
        new_process.remaining_bursttime = new_process.total_bursttime;
        ready_queue[*queue_cnt] = new_process;
        return current_process;
    }
    else
    {
        current_process.execution_endtime = 0;
        current_process.remaining_bursttime = timestamp - current_process.execution_starttime;
        ready_queue[*queue_cnt] = current_process;
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
    }
}

//  the completion of execution of a process in a Priority-based Preemptive Scheduler
struct PCB handle_process_completion_pp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp)
{
    if (&ready_queue[0] == NULL)
        return NULLPCB;

    // find the largest priority PCB
    struct PCB max_priority_process = ready_queue[0];
    int max_process_index = 0;
    for (int i = 1; i < *queue_cnt; ++i)
    {
        if (ready_queue[i].process_priority > max_priority_process.process_priority)
        {
            max_priority_process = ready_queue[i];
            max_process_index = i;
        }
    }
    // remove from ready queue
    remove_pcb(ready_queue, max_process_index, QUEUEMAX);
    max_priority_process.execution_starttime = timestamp;
    max_priority_process.execution_endtime = timestamp + max_priority_process.remaining_bursttime;
    return max_priority_process;
}

//  the arrival of a new process in a Shortest-Remaining-Time-Next Preemptive Scheduler
struct PCB handle_process_arrival_srtp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, struct PCB new_process, int time_stamp)
{
    if (&current_process == &NULLPCB)
    {
        new_process.execution_starttime = time_stamp;
        new_process.execution_endtime = time_stamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;
        return new_process;
    }

    if (new_process.total_bursttime >= current_process.remaining_bursttime)
    {
        new_process.execution_starttime = 0;
        new_process.execution_endtime = 0;
        new_process.remaining_bursttime = new_process.total_bursttime;
        ready_queue[*queue_cnt] = new_process;
        return new_process;
    }
    else
    {
        new_process.execution_starttime = time_stamp;
        new_process.execution_endtime = time_stamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;
        current_process.execution_starttime = 0;
        current_process.execution_endtime = 0;
        current_process.remaining_bursttime = current_process.total_bursttime - time_stamp;
        ready_queue[*queue_cnt] = current_process;
    }
}

//  the completion of execution of a process in a Shortest-Remaining-Time Preemptive Scheduler
struct PCB handle_process_completion_srtp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp) {
    if (&ready_queue[0] == NULL) return NULLPCB;

    // find the shortest remain time process
    struct PCB short_remain_time_process = ready_queue[0];
    int short_remain_time_process_index = 0;
    for (int i = 1; i < *queue_cnt; ++i)
    {
        if (ready_queue[i].remaining_bursttime < short_remain_time_process.remaining_bursttime)
        {
            short_remain_time_process = ready_queue[i];
            short_remain_time_process_index = i;
        }
    }
    // remove from ready queue
    remove_pcb(ready_queue, short_remain_time_process_index, QUEUEMAX);
    short_remain_time_process.execution_starttime = timestamp;
    short_remain_time_process.execution_endtime = timestamp + short_remain_time_process.remaining_bursttime;
    return short_remain_time_process;
}

//  the arrival of a new process in a Round-Robin Scheduler
struct PCB handle_process_arrival_rr(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, struct PCB new_process, int timestamp, int time_quantum) {
    if (&current_process == &NULLPCB)
    {
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;
        return new_process;
    }

    ready_queue[*queue_cnt] = current_process;
    current_process.execution_starttime = 0;
    current_process.execution_endtime = 0;
    current_process.remaining_bursttime = current_process.total_bursttime;
    return current_process;
}

// the completion of execution of a process in a Round-Robin Scheduler
struct PCB handle_process_completion_rr(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp, int time_quantum) {
    if (&ready_queue[0] == NULL) return NULLPCB;

    // find the early arrival time process
    struct PCB early_arrive_time_process = ready_queue[0];
    int early_arrive_time_process_index = 0;
    for (int i = 1; i < *queue_cnt; ++i)
    {
        if (ready_queue[i].arrival_timestamp < early_arrive_time_process.arrival_timestamp)
        {
            early_arrive_time_process = ready_queue[i];
            early_arrive_time_process_index = i;
        }
    }
    // remove from ready queue
    remove_pcb(ready_queue, early_arrive_time_process_index, QUEUEMAX);
    early_arrive_time_process.execution_starttime = timestamp;
    early_arrive_time_process.execution_endtime = timestamp + early_arrive_time_process.remaining_bursttime;
    return early_arrive_time_process;
}

// remove pcb from ready queue
void remove_pcb(struct PCB array[], int index, int array_length)
{
   int i;
   for(i = index; i < array_length - 1; i++) array[i] = array[i + 1];
}