#include "oslabs.h"
#include <stdlib.h>
#include <stdio.h>

struct MEMORY_BLOCK NULLBLOCK = {0, 0, 0, 0};

struct MEMORY_BLOCK insert_memory(struct MEMORY_BLOCK memory_map[MAPMAX], int target_index, int *memory_map_size, int target_memory_size, int process_id);

struct MEMORY_BLOCK best_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int process_id)
{
    int memory_size = *map_cnt;
    if (memory_size == 0)
        return NULLBLOCK;

    int *min_gap_size;
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

    return insert_memory(memory_map, min_gap_index, memory_size, request_size, process_id);
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