#include "oslabs.h"
#include <stdlib.h>
#include <stdio.h>

struct RCB NULLRCB = {
    0, 0, 0, 0, 0
};

struct RCB handle_request_arrival_fcfs(struct RCB request_queue[QUEUEMAX], int *queue_cnt, struct RCB current_request, struct RCB new_request, int timestamp) {
    if (current_request.process_id == 0) {
        return new_request;
    } 

    request_queue[++*queue_cnt] = new_request;
    return current_request;
}

struct RCB handle_request_completion_fcfs(struct RCB request_queue[QUEUEMAX],int *queue_cnt) {
    if (*queue_cnt == 0) {
        return NULLRCB;
    }

    struct RCB min_arrive_rcb = NULLRCB;
    int target_index = -1;
    for (int i = 0; i < *queue_cnt; ++i) {
        if (request_queue[i].arrival_timestamp < min_arrive_rcb.arrival_timestamp) {
            min_arrive_rcb = request_queue[i];
            target_index = i;
        }
    }

    if (target_index == -1) return NULLRCB;

    remove_memory_from_queue(request_queue, target_index, *queue_cnt);
    return min_arrive_rcb;
}

void remove_memory_from_queue(struct RCB request_queue[MAPMAX], int target_index, int *queue_size)
{
    for (int i = target_index; i < *queue_size - 1; i++)
    {
        request_queue[i] = request_queue[i + 1];
    }
    (*queue_size) -= 1;
}