#include "oslabs.h"
#include <stdlib.h>
#include <stdio.h>
#define DUMB_TIMESTAMP -100

// int main()
// {
//     struct PTE p1 = {
//         -1, -1, -1, -1, -1};
//     struct PTE p2 = {
//         -1, -1, -1, -1, -1};
//     struct PTE p3 = {
//         -1, -1, -1, -1, -1};
//     struct PTE p4 = {
//         -1, -1, -1, -1, -1};
//     struct PTE p5 = {
//         -1, -1, -1, -1, -1};
//     struct PTE p6 = {
//         -1, -1, -1, -1, -1};
//     struct PTE p7 = {
//         -1, -1, -1, -1, -1};
//     struct PTE p8 = {
//         -1, -1, -1, -1, -1};
//     int table_cnt = 8;
//     int frame_cnt = 3;
//     int ref_cnt = 11;
//     struct PTE ptes[TABLEMAX] = {p1, p2, p3, p4, p5, p6, p7, p8};
//     int ref_str[REFERENCEMAX] = {0, 3, 2, 6, 3, 4, 5, 2, 4, 5, 6};
//     int frame_pool[POOLMAX] = {0, 1, 2};
//     int faults = count_page_faults_lfu(ptes, table_cnt, ref_str, ref_cnt, frame_pool, frame_cnt);
//     printf("%d", faults);
// }

int poll(int frame_pool[POOLMAX], int frame_cnt);

int process_page_access_fifo(struct PTE page_table[TABLEMAX], int *table_cnt, int page_number, int frame_pool[POOLMAX], int *frame_cnt, int current_timestamp)
{
    if (page_table[page_number].is_valid == 1)
    {
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count += 1;
        return page_table[page_number].frame_number;
    }

    if (*frame_cnt != 0)
    {
        int frame_number = poll(frame_pool, *frame_cnt);
        (*frame_cnt) -= 1;

        page_table[page_number].is_valid = 1;
        page_table[page_number].frame_number = frame_number;
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].arrival_timestamp = current_timestamp;
        page_table[page_number].reference_count = 1;
        return frame_number;
    }
    else
    {
        int target_page_index = -1;
        for (int i = 0; i < *table_cnt; ++i)
        {
            if (page_table[i].is_valid == 1)
            {
                if (target_page_index == -1 || page_table[target_page_index].arrival_timestamp > page_table[i].arrival_timestamp)
                {
                    target_page_index = i;
                }
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
            .frame_number = target_frame_number};
        page_table[page_number] = new_PTE;
        return target_frame_number;
    }
}

int count_page_faults_fifo(struct PTE page_table[TABLEMAX], int table_cnt, int refrence_string[REFERENCEMAX], int reference_cnt, int frame_pool[POOLMAX], int frame_cnt)
{
    int total_page_faults = 0;
    int current_timestamp = 1;
    for (int i = 0; i < reference_cnt; ++i)
    {
        int current_ref = refrence_string[i];
        if (page_table[current_ref].is_valid == 1)
        {
            page_table[current_ref].last_access_timestamp = current_timestamp;
            page_table[current_ref].reference_count++;
        }
        else
        {
            // page fault occurs
            total_page_faults++;
            if (frame_cnt != 0)
            {
                int frame_number = poll(frame_pool, frame_cnt);
                frame_cnt--;
                page_table[current_ref].is_valid = 1;
                page_table[current_ref].frame_number = frame_number;
                page_table[current_ref].last_access_timestamp = current_timestamp;
                page_table[current_ref].arrival_timestamp = current_timestamp;
                page_table[current_ref].reference_count = 1;
            }
            else
            {
                int target_page_index = -1;
                for (int j = 0; j < table_cnt; ++j)
                {
                    if (page_table[j].is_valid == 1)
                    {
                        if (target_page_index == -1)
                        {
                            target_page_index = j;
                            continue;
                        }
                        else if (page_table[target_page_index].arrival_timestamp >= page_table[j].arrival_timestamp)
                        {
                            target_page_index = j;
                        }
                    }
                }
                int target_frame_number = page_table[target_page_index].frame_number;
                page_table[target_page_index].is_valid = 0;
                page_table[target_page_index].arrival_timestamp = 0;
                page_table[target_page_index].last_access_timestamp = 0;
                page_table[target_page_index].reference_count = 0;

                struct PTE new_PTE = {
                    .is_valid = 1,
                    .arrival_timestamp = current_timestamp,
                    .last_access_timestamp = current_timestamp,
                    .reference_count = 1,
                    .frame_number = target_frame_number};
                page_table[current_ref] = new_PTE;
            }
        }
        current_timestamp++;
    }
    return total_page_faults;
}

int process_page_access_lru(struct PTE page_table[TABLEMAX], int *table_cnt, int page_number, int frame_pool[POOLMAX], int *frame_cnt, int current_timestamp)
{
    if (page_table[page_number].is_valid == 1)
    {
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count += 1;
        return page_table[page_number].frame_number;
    }

    if (*frame_cnt != 0)
    {
        int frame_number = poll(frame_pool, *frame_cnt);
        (*frame_cnt) -= 1;

        page_table[page_number].is_valid = 1;
        page_table[page_number].frame_number = frame_number;
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].arrival_timestamp = current_timestamp;
        page_table[page_number].reference_count = 1;
        return frame_number;
    }
    else
    {
        int target_page_index = -1;
        for (int i = 0; i < *table_cnt; ++i)
        {
            if (page_table[i].is_valid == 1)
            {
                if (target_page_index == -1 || page_table[target_page_index].last_access_timestamp > page_table[i].last_access_timestamp)
                {
                    target_page_index = i;
                }
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
            .frame_number = target_frame_number};
        page_table[page_number] = new_PTE;
        return target_frame_number;
    }
}

int count_page_faults_lru(struct PTE page_table[TABLEMAX], int table_cnt, int refrence_string[REFERENCEMAX], int reference_cnt, int frame_pool[POOLMAX], int frame_cnt)
{
    int total_page_faults = 0;
    int current_timestamp = 1;
    for (int i = 0; i < reference_cnt; ++i)
    {
        int current_ref = refrence_string[i];
        if (page_table[current_ref].is_valid == 1)
        {
            page_table[current_ref].last_access_timestamp = current_timestamp;
            page_table[current_ref].reference_count++;
        }
        else
        {
            // page fault occurs
            total_page_faults++;
            if (frame_cnt != 0)
            {
                int frame_number = poll(frame_pool, frame_cnt);
                frame_cnt--;
                page_table[current_ref].is_valid = 1;
                page_table[current_ref].frame_number = frame_number;
                page_table[current_ref].last_access_timestamp = current_timestamp;
                page_table[current_ref].arrival_timestamp = current_timestamp;
                page_table[current_ref].reference_count++;
            }
            else
            {
                int target_page_index = -1;
                for (int j = 0; j < table_cnt; ++j)
                {
                    if (page_table[j].is_valid == 1)
                    {
                        if (target_page_index == -1 || page_table[target_page_index].last_access_timestamp >= page_table[j].last_access_timestamp)
                        {
                            target_page_index = j;
                        }
                    }
                }
                int target_frame_number = page_table[target_page_index].frame_number;
                page_table[target_page_index].is_valid = 0;
                page_table[target_page_index].arrival_timestamp = 0;
                page_table[target_page_index].last_access_timestamp = 0;
                page_table[target_page_index].reference_count = 0;

                struct PTE new_PTE = {
                    .is_valid = 1,
                    .arrival_timestamp = current_timestamp,
                    .last_access_timestamp = current_timestamp,
                    .reference_count = 1,
                    .frame_number = target_frame_number};
                page_table[current_ref] = new_PTE;
            }
        }
        current_timestamp++;
    }
    return total_page_faults;
}

int process_page_access_lfu(struct PTE page_table[TABLEMAX], int *table_cnt, int page_number, int frame_pool[POOLMAX], int *frame_cnt, int current_timestamp)
{
    if (page_table[page_number].is_valid == 1)
    {
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count += 1;
        return page_table[page_number].frame_number;
    }

    if (*frame_cnt != 0)
    {
        int frame_number = poll(frame_pool, *frame_cnt);
        (*frame_cnt) -= 1;

        page_table[page_number].is_valid = 1;
        page_table[page_number].frame_number = frame_number;
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].arrival_timestamp = current_timestamp;
        page_table[page_number].reference_count = 1;
        return frame_number;
    }
    else
    {
        int smallest_ref_count;
        int target_page_index = -1;
        for (int i = 0; i < *table_cnt; ++i)
        {
            if (page_table[i].is_valid == 1)
            {
                if (target_page_index == -1)
                {
                    target_page_index = i;
                    smallest_ref_count = page_table[i].reference_count;
                    continue;
                }
                if (smallest_ref_count > page_table[i].reference_count)
                {
                    smallest_ref_count = page_table[i].reference_count;
                    target_page_index = i;
                }
                else if (smallest_ref_count == page_table[i].reference_count && page_table[target_page_index].arrival_timestamp > page_table[i].arrival_timestamp)
                {
                    smallest_ref_count = page_table[i].reference_count;
                    target_page_index = i;
                }
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
            .frame_number = target_frame_number};
        page_table[page_number] = new_PTE;
        return target_frame_number;
    }
}

int count_page_faults_lfu(struct PTE page_table[TABLEMAX], int table_cnt, int refrence_string[REFERENCEMAX], int reference_cnt, int frame_pool[POOLMAX], int frame_cnt)
{
    int total_page_faults = 0;
    int current_timestamp = 1;
    for (int i = 0; i < reference_cnt; ++i)
    {
        int current_ref = refrence_string[i];
        if (page_table[i].is_valid == 1)
        {
            page_table[current_ref].last_access_timestamp = current_timestamp;
            page_table[current_ref].reference_count++;
        }
        else
        {
            // page fault occurs
            total_page_faults++;
            if (frame_cnt != 0)
            {
                int frame_number = poll(frame_pool, frame_cnt);
                frame_cnt--;
                page_table[current_ref].is_valid = 1;
                page_table[current_ref].frame_number = frame_number;
                page_table[current_ref].last_access_timestamp = current_timestamp;
                page_table[current_ref].arrival_timestamp = current_timestamp;
                page_table[current_ref].reference_count = 1;
            }
            else
            {
                int target_page_index = -1;
                for (int j = 0; j < table_cnt; ++j)
                {
                    if (page_table[j].is_valid == 1)
                    {
                        if (target_page_index == -1)
                        {
                            target_page_index = j;
                        }
                        else if (page_table[target_page_index].reference_count > page_table[j].reference_count) {
                            target_page_index = j;
                        }
                        else if ((page_table[target_page_index].reference_count == page_table[j].reference_count && page_table[target_page_index].arrival_timestamp >= page_table[j].arrival_timestamp)) {
                            target_page_index = j;
                        }
                    }
                }
                int target_frame_number = page_table[target_page_index].frame_number;
                page_table[target_page_index].is_valid = 0;
                page_table[target_page_index].arrival_timestamp = 0;
                page_table[target_page_index].last_access_timestamp = 0;
                page_table[target_page_index].reference_count = 0;

                struct PTE new_PTE = {
                    .is_valid = 1,
                    .arrival_timestamp = current_timestamp,
                    .last_access_timestamp = current_timestamp,
                    .reference_count = 1,
                    .frame_number = target_frame_number};
                page_table[current_ref] = new_PTE;
            }
        }
        current_timestamp++;
    }
    return total_page_faults;
}

int poll(int frame_pool[POOLMAX], int frame_cnt)
{
    if (frame_cnt == 0)
        return -1;

    int poll_frame = frame_pool[0];

    if (frame_cnt == 1)
        return poll_frame;

    for (int i = 0; i < frame_cnt; ++i)
    {
        frame_pool[i] = frame_pool[i + 1];
    }
    return poll_frame;
}