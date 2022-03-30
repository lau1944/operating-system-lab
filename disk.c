#include "oslabs.h"
#include <stdlib.h>
#include <stdio.h>

struct RCB NULLRCB = {
    0, 0, 0, 0, 0};

int main()
{
    int size = 3;
    struct RCB rcb0 = {1, 52, 57, 58, 1};
    struct RCB rcb1 = {2, 51, 58, 58, 2};
    struct RCB rcb2 = {3, 53, 59, 58, 3};
    struct RCB request_queue[QUEUEMAX];
    request_queue[0] = rcb0;
    request_queue[1] = rcb1;
    request_queue[2] = rcb2;
    struct RCB result = handle_request_completion_look(request_queue, &size, 56, 0);
    printf("%d", result.request_id);
}

void addToArray(int array[], int element, int *size);

int find_smallest_cylinder(int array[], int array_size);

int find_smallest_cylinder_from_index(int array[], int index[], int index_size);

struct RCB remove_memory_from_queue(struct RCB request_queue[MAPMAX], int target_index, int *queue_size);

struct RCB handle_request_arrival_fcfs(struct RCB request_queue[QUEUEMAX], int *queue_cnt, struct RCB current_request, struct RCB new_request, int timestamp)
{
    if (current_request.process_id == 0)
    {
        return new_request;
    }

    request_queue[(*queue_cnt)++] = new_request;
    return current_request;
}

struct RCB handle_request_completion_fcfs(struct RCB request_queue[QUEUEMAX], int *queue_cnt)
{
    if (*queue_cnt == 0)
    {
        return NULLRCB;
    }

    struct RCB min_arrive_rcb = request_queue[0];
    int target_index = 0;
    for (int i = 1; i < *queue_cnt; ++i)
    {
        if (request_queue[i].arrival_timestamp < min_arrive_rcb.arrival_timestamp)
        {
            min_arrive_rcb = request_queue[i];
            target_index = i;
        }
    }

    if (target_index == -1)
        return NULLRCB;

    remove_memory_from_queue(request_queue, target_index, queue_cnt);
    return min_arrive_rcb;
}

struct RCB handle_request_arrival_sstf(struct RCB request_queue[QUEUEMAX], int *queue_cnt, struct RCB current_request, struct RCB new_request, int timestamp)
{
    if (current_request.process_id == 0)
    {
        return new_request;
    }

    request_queue[(*queue_cnt)++] = new_request;
    return current_request;
}

struct RCB handle_request_completion_sstf(struct RCB request_queue[QUEUEMAX], int *queue_cnt, int current_cylinder)
{
    if (*queue_cnt == 0)
    {
        return NULLRCB;
    }

    int min_cylinder_dis = abs(current_cylinder - request_queue[0].cylinder);
    int target_index = 0;
    struct RCB rcb = request_queue[0];
    for (int i = 1; i < *queue_cnt; ++i)
    {
        int distance = abs(current_cylinder - request_queue[i].cylinder);
        if (distance < min_cylinder_dis)
        {
            min_cylinder_dis = distance;
            rcb = request_queue[i];
            target_index = i;
        }
        if (distance == min_cylinder_dis && rcb.arrival_timestamp > request_queue[i].arrival_timestamp)
        {
            min_cylinder_dis = distance;
            rcb = request_queue[i];
            target_index = i;
        }
    }

    remove_memory_from_queue(request_queue, target_index, queue_cnt);
    return rcb;
}

struct RCB handle_request_arrival_look(struct RCB request_queue[QUEUEMAX], int *queue_cnt, struct RCB current_request, struct RCB new_request, int timestamp)
{
    if (current_request.process_id == 0)
    {
        return new_request;
    }

    request_queue[(*queue_cnt)++] = new_request;
    return current_request;
}

struct RCB handle_request_completion_look(struct RCB request_queue[QUEUEMAX], int *queue_cnt, int current_cylinder, int scan_direction)
{
    if (*queue_cnt == 0)
    {
        return NULLRCB;
    }

    struct RCB target_rcb;

    int cylinder_dif[*queue_cnt];

    for (int i = 0; i < *queue_cnt; ++i)
    {
        cylinder_dif[i] = abs(request_queue[i].cylinder - current_cylinder);
    }

    int target_index_size = 0;
    int* rcb_index_pointer = malloc(sizeof(int) * target_index_size);

    int buf_size = 0;
    int* buf_index_pointer = malloc(sizeof(int) * buf_size);

    // find same cylinder
    for (int i = 0; i < *queue_cnt; ++i)
    {
        if (current_cylinder == request_queue[i].cylinder)
        {
            addToArray(rcb_index_pointer, i, &target_index_size);
        }
    }

    if (target_index_size != 0)
    {
        int index = find_smallest_cylinder_from_index(cylinder_dif, rcb_index_pointer, target_index_size);
        return remove_memory_from_queue(request_queue, index, queue_cnt);
    }

    for (int i = 0; i < *queue_cnt; ++i)
    {
        if (scan_direction == 1)
        {
            if (current_cylinder < request_queue[i].cylinder)
            {
                addToArray(rcb_index_pointer, i, &target_index_size);
            }
        }

        else if (scan_direction == 0)
        {
            if (current_cylinder > request_queue[i].cylinder)
            {
                addToArray(rcb_index_pointer, i, &target_index_size);
            }
            else
            {
                addToArray(buf_index_pointer, i, &buf_size);
            }
        }
    }

    if (target_index_size == 0 && buf_size != 0)
    {
        int index = find_smallest_cylinder_from_index(cylinder_dif, buf_index_pointer, buf_size);
        return remove_memory_from_queue(request_queue, index, queue_cnt);
    }

    else if (target_index_size != 0)
    {
        int index = find_smallest_cylinder_from_index(cylinder_dif, rcb_index_pointer, target_index_size);
        return remove_memory_from_queue(request_queue, index, queue_cnt);
    }

    // No require found, find the smallest cylinder
    int index = find_smallest_cylinder(cylinder_dif, *queue_cnt);
    return remove_memory_from_queue(request_queue, index, queue_cnt);
}

int find_smallest_cylinder(int array[], int array_size)
{
    int target_index = 0;
    int smallest_cylinder = array[0];
    for (int i = 1; i < array_size; ++i)
    {
        if (array[i] < smallest_cylinder)
        {
            target_index = i;
        }
    }
    return target_index;
}

int find_smallest_cylinder_from_index(int array[], int index[], int index_size)
{
    int target_index = index[0];
    int smallest_cylinder = array[0];
    for (int i = 1; i < index_size; ++i)
    {
        if (array[i] < smallest_cylinder)
        {
            target_index = i;
        }
    }
    return target_index;
}

void addToArray(int array[], int element, int *size)
{
    array = realloc(array, sizeof(int) * (++(*size)));
    array[*size - 1] = element;
    // int new_array[++(*size)];
    // for (int i = 0; i < *size - 1; ++i)
    // {
    //     new_array[i] = (*array)[i];
    // }
    // new_array[*size - 1] = element;
    // int (*pointer)[*size];
    // pointer = &new_array;
    // return pointer;
}

struct RCB remove_memory_from_queue(struct RCB request_queue[MAPMAX], int target_index, int *queue_size)
{
    struct RCB removed_rcb = request_queue[target_index];
    for (int i = target_index; i < *queue_size - 1; ++i)
    {
        request_queue[i] = request_queue[i + 1];
    }
    (*queue_size) -= 1;
    return removed_rcb;
}