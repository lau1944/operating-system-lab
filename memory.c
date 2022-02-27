#include "oslabs.h"
#include <stdlib.h>
#include <stdio.h>

struct MEMORY_BLOCK NULLBLOCK = {0, 0, 0, 0};

struct MEMORY_BLOCK insert_memory(struct MEMORY_BLOCK memory_map[MAPMAX], int target_index, int *memory_map_size, int target_memory_size, int process_id);

void remove_memory_from_map(struct MEMORY_BLOCK memory_map[MAPMAX], int target_index, int *memory_map_size, int target_size);

int main()
{
    int size = 3;
    struct MEMORY_BLOCK first = {0, 15, 15, 1};
    struct MEMORY_BLOCK second = {16, 22, 7, 0};
    struct MEMORY_BLOCK third = {23, 27, 5, 3};
    struct MEMORY_BLOCK memory_map[MAPMAX] = {first, second, third};
    struct MEMORY_BLOCK inserted = {28, 32, 5, 4};
    insert_memory(memory_map, 1, &size, 5, 4);
    for (int i = 0; i < 5; i++)
    {
        printf("%d \n", memory_map[i].process_id);
    }
}

struct MEMORY_BLOCK best_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int process_id)
{
    int memory_size = *map_cnt;
    if (memory_size == 0)
        return NULLBLOCK; 

    int *min_gap_size = NULL;
    int min_gap_index;
    // Find the smallest gap
    for (int i = 0; i < memory_size; ++i)
    {
        // It has been occupied
        if (memory_map[i].process_id == 0)
            continue;
        int current_gap = memory_map[i].segment_size - request_size;
        // NOT enough space
        if (current_gap < 0)
            continue;

        if (i == 0)
        {
            min_gap_index = 0;
            *min_gap_size = current_gap;
        }
        else if (current_gap < *min_gap_size)
        {
            min_gap_index = i;
            *min_gap_size = current_gap;
        }
    }

    // Cannot find space
    if (min_gap_size == NULL)
        return NULLBLOCK;

    if (*min_gap_size == 0)
    {
        memory_map[min_gap_index].process_id = process_id;
        return memory_map[min_gap_index];
    }

    return insert_memory(memory_map, min_gap_index, map_cnt, request_size, process_id);
}

struct MEMORY_BLOCK first_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int process_id)
{
    int memory_size = *map_cnt;
    if (memory_size == 0)
        return NULLBLOCK;

    int space_index = -1;
    for (int i = 0; i < memory_size; ++i) {
        // It has been occupied
        if (memory_map[i].process_id == 0)
            continue;
        int current_gap = memory_map[i].segment_size - request_size;
        if (current_gap >= 0) {
            space_index = i;
        }
    }

    if (space_index == -1) return NULLBLOCK;        

    if (memory_map[space_index].segment_size == request_size) {
        memory_map[space_index].process_id = process_id;
        return memory_map[space_index];
    }

    return insert_memory(memory_map, space_index, map_cnt, request_size, process_id);
}

struct MEMORY_BLOCK worst_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int process_id)
{
    int memory_size = *map_cnt;
    if (memory_size == 0)
        return NULLBLOCK;

    int memory_size = *map_cnt;
    if (memory_size == 0)
        return NULLBLOCK;

    int *max_gap_size;
    int max_gap_index;
    // Find the smallest gap
    for (int i = 0; i < memory_size; ++i)
    {
        // It has been occupied
        if (memory_map[i].process_id == 0)
            continue;
        int current_gap = memory_map[i].segment_size - request_size;
        // NOT enough space
        if (current_gap < 0)
            continue;

        if (i == 0)
        {
            max_gap_index = 0;
            *max_gap_size = current_gap;
        }
        else if (current_gap > *max_gap_size)
        {
            max_gap_index = i;
            *max_gap_size = current_gap;
        }
    }

    // Cannot find space
    if (max_gap_size == NULL)
        return NULLBLOCK;

    if (*max_gap_size == 0)
    {
        memory_map[max_gap_index].process_id = process_id;
        return memory_map[max_gap_index];
    }

    return insert_memory(memory_map, max_gap_index, map_cnt, request_size, process_id);    
}

struct MEMORY_BLOCK next_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int process_id, int last_address)
{
    int memory_size = *map_cnt;
    if (memory_size == 0)
        return NULLBLOCK;

    int space_index = -1;
    for (int i = 0; i < memory_size; ++i) {
        // It has been occupied
        if (memory_map[i].process_id == 0)
            continue;
        int current_gap = memory_map[i].segment_size - request_size;
        if (current_gap >= 0 && memory_map[i].start_address >= last_address) {
            space_index = i;
        }
    }       

    if (space_index == -1) return NULLBLOCK; 

    if (memory_map[space_index].segment_size == request_size) {
        memory_map[space_index].process_id = process_id;
        return memory_map[space_index];
    }

    return insert_memory(memory_map, space_index, map_cnt, request_size, process_id);
}

void release_memory(struct MEMORY_BLOCK freed_block, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt)
{
    freed_block.process_id = 0;
    int freed_index = 0;
    while (memory_map[freed_index].start_address != freed_block.start_address)
    {
        freed_index++;
    }

    if (freed_index > 0 && memory_map[freed_index - 1].process_id == 0 && memory_map[freed_index + 1].process_id == 0)
    {
        memory_map[freed_index - 1].end_address = memory_map[freed_index + 1].end_address;
        remove_memory_from_map(memory_map, freed_index, map_cnt, 2);
        return;
    }

    if (freed_index > 0 && memory_map[freed_index - 1].process_id == 0)
    {
        memory_map[freed_index - 1].end_address = freed_block.end_address;
        remove_memory_from_map(memory_map, freed_index, map_cnt, 1);
        return;
    }

    if (memory_map[freed_index + 1].process_id == 0)
    {
        memory_map[freed_index + 1].start_address = freed_block.start_address;
        remove_memory_from_map(memory_map, freed_index, map_cnt, 1);
        return;
    }
}

void remove_memory_from_map(struct MEMORY_BLOCK memory_map[MAPMAX], int target_index, int *memory_map_size, int target_size)
{
    for (int i = target_index; i < *memory_map_size - target_size; i++)
    {
        memory_map[i] = memory_map[i + target_size];
    }
    (*memory_map_size) -= target_size;
}

struct MEMORY_BLOCK insert_memory(struct MEMORY_BLOCK memory_map[MAPMAX], int target_index, int *memory_map_size, int target_memory_size, int process_id)
{
    int origin_end_address = memory_map[target_index].end_address;
    int origin_size = memory_map[target_index].segment_size;
    memory_map[target_index].end_address = memory_map[target_index].start_address + target_memory_size;
    memory_map[target_index].process_id = process_id;
    memory_map[target_index].segment_size = target_memory_size;

    struct MEMORY_BLOCK free_memory = {
        .start_address = memory_map[target_index].end_address + 1,
        .end_address = origin_end_address,
        .process_id = 0,
        .segment_size = origin_size - target_memory_size};

    (*memory_map_size)++;
    int insert_pos = target_index + 1;
    // Insert free_memory into memory_map
    for (int i = *memory_map_size; i > insert_pos; --i)
    {
        memory_map[i] = memory_map[i - 1];
    }
    memory_map[insert_pos] = free_memory;
    return memory_map[target_index];
}