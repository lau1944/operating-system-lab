#include "oslabs.h"
#include <stdlib.h>
#include <stdio.h>
struct PCB NULLPCB = {0, 0, 0, 0, 0, 0, 0};

void remove_pcb(struct PCB array[QUEUEMAX], int index, int array_length);

int is_null_pcb(struct PCB process);

// convert pointer to integer
u_int64_t PointerToInt(void* ptr){
    u_int64_t* u=(void*)&ptr;
    return *u;
}

// int main(int argc, char *argv[]) {
//     struct PCB current_process = {1, 1, 4, 1, 5, 4, 8};
//     struct PCB new_process = {2,2,3,0,0,3,6};
//     struct PCB ready_queue[QUEUEMAX];
//     //handle_process_arrival_pp(ready_queue, 0, current_process, new_process, 2);

//     ready_queue[0] = current_process;
//     ready_queue[1] = new_process;
//     remove_pcb(ready_queue, 0, 2);
//     int count = 0;
//     for (int i = 0; i < QUEUEMAX; i++) {
//         printf("process id: %d \n", ready_queue[i].process_id);
        
//     }
//     printf("%d", count);
// }

// [PID:1, AT:1, TBT:4, EST:1, EET:5, RBT:4, Priority:8]
// [PID:2, AT:2, TBT:3, EST:0, EET:0, RBT:3, Priority:6]
// Priority-based Preemptive Scheduler
struct PCB handle_process_arrival_pp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, struct PCB new_process, int timestamp)
{
    int size = PointerToInt(queue_cnt);
    // No current process
    if (is_null_pcb(current_process))
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
        ready_queue[size] = new_process;
        return current_process;
    }
    else
    {
        current_process.execution_endtime = 0;
        current_process.remaining_bursttime = timestamp - current_process.execution_starttime;
        ready_queue[size] = current_process;
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
        return new_process;
    }
}

//  the completion of execution of a process in a Priority-based Preemptive Scheduler
struct PCB handle_process_completion_pp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp)
{
    if (&ready_queue[0] == NULL)
        return NULLPCB;

    // find the largest priority PCB
    int size = PointerToInt(queue_cnt);
    struct PCB max_priority_process = ready_queue[0];
    int max_process_index = 0;
    for (int i = 1; i < size; ++i)
    {
        if (ready_queue[i].process_priority > max_priority_process.process_priority)
        {
            max_priority_process = ready_queue[i];
            max_process_index = i;
        }
    }
    // remove from ready queue
    remove_pcb(ready_queue, max_process_index, size);
    max_priority_process.execution_starttime = timestamp;
    max_priority_process.execution_endtime = timestamp + max_priority_process.remaining_bursttime;
    return max_priority_process;
}

//  the arrival of a new process in a Shortest-Remaining-Time-Next Preemptive Scheduler
struct PCB handle_process_arrival_srtp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, struct PCB new_process, int time_stamp)
{
    int size = PointerToInt(queue_cnt);
    if (is_null_pcb(current_process))
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
        ready_queue[size] = new_process;
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
        ready_queue[size] = current_process;
        return new_process;
    }
}

//  the completion of execution of a process in a Shortest-Remaining-Time Preemptive Scheduler
struct PCB handle_process_completion_srtp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp) {
    if (&ready_queue[0] == NULL) return NULLPCB;

    // find the shortest remain time process
    int size = PointerToInt(queue_cnt);
    struct PCB short_remain_time_process = ready_queue[0];
    int short_remain_time_process_index = 0;
    for (int i = 1; i < size; ++i)
    {
        if (ready_queue[i].remaining_bursttime < short_remain_time_process.remaining_bursttime)
        {
            short_remain_time_process = ready_queue[i];
            short_remain_time_process_index = i;
        }
    }
    // remove from ready queue
    remove_pcb(ready_queue, short_remain_time_process_index, size);
    short_remain_time_process.execution_starttime = timestamp;
    short_remain_time_process.execution_endtime = timestamp + short_remain_time_process.remaining_bursttime;
    return short_remain_time_process;
}

//  the arrival of a new process in a Round-Robin Scheduler
struct PCB handle_process_arrival_rr(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, struct PCB new_process, int timestamp, int time_quantum) {
    int size = PointerToInt(queue_cnt);
    if (is_null_pcb(current_process))
    {
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;
        return new_process;
    }

    ready_queue[size] = current_process;
    current_process.execution_starttime = 0;
    current_process.execution_endtime = 0;
    current_process.remaining_bursttime = current_process.total_bursttime;
    return current_process;
}

// the completion of execution of a process in a Round-Robin Scheduler
struct PCB handle_process_completion_rr(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp, int time_quantum) {
    if (&ready_queue[0] == NULL) return NULLPCB;

    // find the early arrival time process
    int size = PointerToInt(queue_cnt);
    struct PCB early_arrive_time_process = ready_queue[0];
    int early_arrive_time_process_index = 0;
    for (int i = 1; i < size; ++i)
    {
        if (ready_queue[i].arrival_timestamp < early_arrive_time_process.arrival_timestamp)
        {
            early_arrive_time_process = ready_queue[i];
            early_arrive_time_process_index = i;
        }
    }
    // remove from ready queue
    remove_pcb(ready_queue, early_arrive_time_process_index, size);
    early_arrive_time_process.execution_starttime = timestamp;
    early_arrive_time_process.execution_endtime = timestamp + early_arrive_time_process.remaining_bursttime;
    return early_arrive_time_process;
}

int is_null_pcb(struct PCB process) {
    return process.arrival_timestamp    == NULLPCB.arrival_timestamp 
        && process.execution_endtime    == NULLPCB.execution_endtime
        && process.execution_starttime  == NULLPCB.execution_starttime
        && process.process_id           == NULLPCB.process_id
        && process.process_priority     == NULLPCB.process_priority
        && process.remaining_bursttime  == NULLPCB.remaining_bursttime
        && process.total_bursttime      == NULLPCB.total_bursttime;
}

// remove pcb from ready queue
void remove_pcb(struct PCB array[QUEUEMAX], int index, int array_length)
{
   for (int i = index; i < QUEUEMAX; ++i) {
       array[i] = array[i + 1];
   }
}