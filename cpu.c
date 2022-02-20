#include "oslabs.h"
#include <stdlib.h>
#include <stdio.h>
struct PCB NULLPCB = {0, 0, 0, 0, 0, 0, 0};

void remove_pcb(struct PCB array[], int index, int array_length);

int is_empty_queue(struct PCB queue[], int length);

int is_null_pcb(struct PCB process);

// convert pointer to integer
// u_int64_t PointerToInt(void *ptr)
// {
//     u_int64_t *u = (void *)&ptr;
//     return *u;
// }

// int main(int argc, char *argv[]) {
//     int i = 3;
//     struct PCB current_process = {1, 1, 4, 1, 5, 4, 8};
//     struct PCB new_process = {2,2,3,0,0,3,6};
//     struct PCB old_process = {3,2,3,0,0,3,6};
//     struct PCB ready_queue[3];
//     ready_queue[0] = current_process;
//     ready_queue[1] = new_process;
//     ready_queue[2] = old_process;
//     remove_pcb(ready_queue, 1, 3);
//     //handle_process_completion_pp(ready_queue, &i, 5);
//     for (int i = 0; i < 3; i++) {
//         printf("%d", ready_queue[i].process_id);
//     }
// }

// Priority-based Preemptive Scheduler
struct PCB handle_process_arrival_pp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, struct PCB new_process, int timestamp)
{
    int size = *queue_cnt;
    // No current process
    if (is_null_pcb(current_process))
    {
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;
        return new_process;
    }

    // current process is running
    if (current_process.process_priority <= new_process.process_priority)
    {
        new_process.execution_endtime = 0;
        new_process.execution_starttime = 0;
        new_process.remaining_bursttime = new_process.total_bursttime;
        ready_queue[size] = new_process;
        ++(*queue_cnt);
        return current_process;
    }
    else
    {
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
        current_process.execution_endtime = 0;
        current_process.remaining_bursttime = current_process.total_bursttime - timestamp + current_process.execution_starttime;
        ready_queue[size] = current_process;
        ++(*queue_cnt);
        return new_process;
    }
}

//  the completion of execution of a process in a Priority-based Preemptive Scheduler
struct PCB handle_process_completion_pp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp)
{
    if (is_empty_queue(ready_queue, *queue_cnt))
        return NULLPCB;

    // find the largest priority PCB
    int size = *queue_cnt;
    struct PCB high_priority_process = ready_queue[0];
    int high_process_index = 0;
    for (int i = 0; i < size; ++i)
    {
        if (ready_queue[i].process_priority < high_priority_process.process_priority)
        {
            high_priority_process = ready_queue[i];
            high_process_index = i;
        }
    }
    // remove from ready queue
    remove_pcb(ready_queue, high_process_index, *queue_cnt);
    *queue_cnt = size - 1;
    high_priority_process.execution_starttime = timestamp;
    high_priority_process.execution_endtime = timestamp + high_priority_process.remaining_bursttime;
    return high_priority_process;
}

//  the arrival of a new process in a Shortest-Remaining-Time-Next Preemptive Scheduler
struct PCB handle_process_arrival_srtp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, struct PCB new_process, int time_stamp)
{
    int size = *queue_cnt;
    if (is_null_pcb(current_process))
    {
        new_process.execution_starttime = time_stamp;
        new_process.execution_endtime = time_stamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;
        return new_process;
    }

    if (current_process.remaining_bursttime <= new_process.total_bursttime)
    {
        new_process.execution_starttime = 0;
        new_process.execution_endtime = 0;
        new_process.remaining_bursttime = new_process.total_bursttime;
        ready_queue[size] = new_process;
        ++(*queue_cnt);
        return current_process;
    }
    else
    {
        new_process.execution_starttime = time_stamp;
        new_process.execution_endtime = time_stamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;
        current_process.remaining_bursttime = current_process.total_bursttime - time_stamp + current_process.execution_starttime;
        current_process.execution_starttime = 0;
        current_process.execution_endtime = 0;
        ready_queue[size] = current_process;
        ++(*queue_cnt);
        return new_process;
    }
}

//  the completion of execution of a process in a Shortest-Remaining-Time Preemptive Scheduler
struct PCB handle_process_completion_srtp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp)
{
    if (is_empty_queue(ready_queue, (*queue_cnt)))
        return NULLPCB;

    // find the shortest remain time process
    int size = *queue_cnt;
    struct PCB short_remain_time_process = ready_queue[0];
    int short_remain_time_process_index = 0;
    for (int i = 0; i < size; ++i)
    {
        if (ready_queue[i].remaining_bursttime < short_remain_time_process.remaining_bursttime)
        {
            short_remain_time_process = ready_queue[i];
            short_remain_time_process_index = i;
        }
    }
    // remove from ready queue
    remove_pcb(ready_queue, short_remain_time_process_index, *queue_cnt);
    *queue_cnt = size - 1;
    short_remain_time_process.execution_starttime = timestamp;
    short_remain_time_process.execution_endtime = timestamp + short_remain_time_process.remaining_bursttime;
    return short_remain_time_process;
}

//  the arrival of a new process in a Round-Robin Scheduler
struct PCB handle_process_arrival_rr(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, struct PCB new_process, int timestamp, int time_quantum)
{
    int size = *queue_cnt;
    if (is_null_pcb(current_process))
    {
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;
        return new_process;
    }

    ready_queue[size] = new_process;
    ++(*queue_cnt);
    new_process.execution_starttime = 0;
    new_process.execution_endtime = 0;
    new_process.remaining_bursttime = current_process.total_bursttime;
    return current_process;
}

// the completion of execution of a process in a Round-Robin Scheduler
struct PCB handle_process_completion_rr(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp, int time_quantum)
{
    if (is_empty_queue(ready_queue, *queue_cnt))
        return NULLPCB;

    // find the early arrival time process
    int size = *queue_cnt;
    struct PCB early_arrive_time_process = ready_queue[0];
    int early_arrive_time_process_index = 0;
    for (int i = 0; i < size; ++i)
    {
        if (ready_queue[i].arrival_timestamp < early_arrive_time_process.arrival_timestamp)
        {
            early_arrive_time_process = ready_queue[i];
            early_arrive_time_process_index = i;
        }
    }
    // remove from ready queue
    remove_pcb(ready_queue, early_arrive_time_process_index, *queue_cnt);
    *queue_cnt -= 1;
    early_arrive_time_process.execution_starttime = timestamp;
    early_arrive_time_process.execution_endtime = timestamp + early_arrive_time_process.remaining_bursttime;
    return early_arrive_time_process;
}

int is_empty_queue(struct PCB queue[], int length)
{
    for (int i = 0; i < length; ++i)
    {
        if ((&queue)[i] == NULL || is_null_pcb(queue[i]))
        {
            return 0;
        }
    }
    return 1;
}

int is_null_pcb(struct PCB process)
{
    return process.arrival_timestamp == NULLPCB.arrival_timestamp && process.execution_endtime == NULLPCB.execution_endtime && process.execution_starttime == NULLPCB.execution_starttime && process.process_id == NULLPCB.process_id && process.process_priority == NULLPCB.process_priority && process.remaining_bursttime == NULLPCB.remaining_bursttime && process.total_bursttime == NULLPCB.total_bursttime;
}

// remove pcb from ready queue
void remove_pcb(struct PCB array[], int index, int array_length)
{
    for (int i = index; i < array_length - 1; i++)
    {
        array[i] = array[i+1];
    }
    (&array)[array_length - 1] = NULL;
}