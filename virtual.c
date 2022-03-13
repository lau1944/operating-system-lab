#include "oslabs.h"
#include <stdlib.h>
#include <stdio.h>

int poll(int frame_pool[POOLMAX], int *frame_cnt);

int process_page_access_fifo(struct PTE page_table[TABLEMAX],int *table_cnt, int page_number, int frame_pool[POOLMAX],int *frame_cnt, int current_timestamp) {
    if (page_table[page_number].is_valid == 1) {
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count += 1;
        return page_table[page_number].frame_number;
    }

    if (*frame_cnt != 0) {
        int frame_number = poll(frame_pool, frame_cnt);
        
        page_table[page_number].is_valid = 1;
        page_table[page_number].frame_number = frame_number;
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].arrival_timestamp = current_timestamp;
        page_table[page_number].reference_count += 1;
        return frame_number;
    } else {
        int smallest_timestamp = -1;
        int target_page_index;
        int first_invalid_index = -1;
        for (int i = 0; i < TABLEMAX; ++i) {
            if (page_table[i].is_valid == 0 && first_invalid_index == -1) {
                first_invalid_index = i;
            }

            if (page_table[i].is_valid == 1 && smallest_timestamp > page_table[i].arrival_timestamp) {
                smallest_timestamp = MIN(smallest_timestamp, page_table[i].arrival_timestamp);
                target_page_index = i;
            }
        }
        int target_frame_number = page_table[target_page_index].frame_number;
        // mark invalid
        page_table[target_page_index].is_valid = 0;
        page_table[target_page_index].frame_number = -1;
        page_table[target_page_index].arrival_timestamp = -1;
        page_table[target_page_index].last_access_timestamp = -1;
        page_table[target_page_index].reference_count = -1;

        struct PTE new_PTE = {
            .is_valid = 1,
            .arrival_timestamp = current_timestamp,
            .last_access_timestamp = current_timestamp,
            .reference_count = 1,
            .frame_number = target_frame_number
        };
        page_table[first_invalid_index] = new_PTE;
        return target_frame_number;
    }
}

int poll(int frame_pool[POOLMAX], int *frame_cnt) {
    if (*frame_cnt == 0) return -1;

    int poll_frame = frame_pool[0];
    for (int i = 0; i < *frame_cnt; ++i) {
        frame_pool[i] = frame_pool[i + 1];
    }
    *frame_cnt--;
    return poll_frame;
}