 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <perf.h>
#include <rtthread.h>
#include "app_init.h"
#include <csi_core.h>
#include <string.h>

perf_stat_args_t* perf_stat_parse_cmd(char* input) {
    for (int i = 0; i < strlen(input); i++)
    {
        if(input[i] == '\r') input[i] = 0;
    }
    // Allocate memory for the perf_stat_args_t structure
    perf_stat_args_t* cmd = (perf_stat_args_t*)perf_malloc(sizeof(perf_stat_args_t));
    if (cmd == NULL) {
        rt_kprintf("Failed to allocate memory for perf_stat_args_t.\n");
        return NULL;
    }

    // Initialize the structure fields
    cmd->events = NULL;
    cmd->cpu_ids = NULL;
    cmd->tids = NULL;
    cmd->nr_threads = 0;
    cmd->nr_events = 0;
    cmd->nr_cpus = 0;

    // Copy the input string to avoid modifying the original
    char* input_copy = strdup(input);
    if (input_copy == NULL) {
        rt_kprintf("Failed to duplicate input string.\n");
        perf_free(cmd);
        return NULL;
    }

    // Tokenize the input string
    char* token;
    char* rest = input_copy;

    // Variables to hold the option values
    char* event_values = NULL;
    char* cpu_values = NULL;
    char* thread_values = NULL;

    // Skip the initial "perf stat" tokens
    token = strtok_r(rest, " ", &rest);
    if (token == NULL || strcmp(token, "perf") != 0) {
        rt_kprintf("Input does not start with 'perf'.\n");
        perf_free(input_copy);
        perf_free(cmd);
        return NULL;
    }
    token = strtok_r(rest, " ", &rest);
    if (token == NULL || strcmp(token, "stat") != 0) {
        // rt_kprintf("Input does not contain 'stat' after 'perf'.\n");
        rt_kprintf(token);
        perf_free(input_copy);
        perf_free(cmd);
        return NULL;
    }

    // Parse the command options and their values
    while ((token = strtok_r(rest, " ", &rest))) {
        if (strcmp(token, "-e") == 0) {
            event_values = strtok_r(rest, " ", &rest);
        } else if (strcmp(token, "-C") == 0) {
            cpu_values = strtok_r(rest, " ", &rest);
        } else if (strcmp(token, "-t") == 0) {
            thread_values = strtok_r(rest, " ", &rest);
        }
        // You can handle other options here if needed
    }

    // Process the event values (-e option)
    if (event_values) {
        // Duplicate the event values string to tokenize
        char* event_values_copy = strdup(event_values);
        if (event_values_copy == NULL) {
            rt_kprintf("Failed to duplicate event values string.\n");
            perf_free(input_copy);
            perf_free(cmd);
            return NULL;
        }

        // Count the number of events
        uint16_t event_count = 0;
        char* temp_token;
        char* temp_rest = event_values_copy;
        while ((temp_token = strtok_r(temp_rest, ",", &temp_rest))) {
            event_count++;
        }

        // Allocate memory for the events array
        cmd->events = (uint16_t*)malloc(event_count * sizeof(uint16_t));
        if (cmd->events == NULL) {
            rt_kprintf("Failed to allocate memory for events array.\n");
            perf_free(event_values_copy);
            perf_free(input_copy);
            perf_free(cmd);
            return NULL;
        }
        cmd->nr_events = event_count;

        // Tokenize again to fill the events array
        strcpy(event_values_copy, event_values); // Reset the copy
        temp_rest = event_values_copy;
        uint16_t i = 0;
        while ((temp_token = strtok_r(temp_rest, ",", &temp_rest))) {
            // Get event ID from event name
            uint16_t event_id = perf_get_event_id(temp_token);
            cmd->events[i++] = event_id;
        }

        perf_free(event_values_copy);
    }

    // Process the CPU values (-C option)
    if (cpu_values) {
        // Duplicate the CPU values string to tokenize
        char* cpu_values_copy = strdup(cpu_values);
        if (cpu_values_copy == NULL) {
            rt_kprintf("Failed to duplicate CPU values string.\n");
            perf_free(input_copy);
            perf_free(cmd->events);
            perf_free(cmd);
            return NULL;
        }

        // Count the number of CPUs
        uint8_t cpu_count = 0;
        char* temp_token;
        char* temp_rest = cpu_values_copy;
        while ((temp_token = strtok_r(temp_rest, ",", &temp_rest))) {
            cpu_count++;
        }

        // Allocate memory for the cpu_id array
        cmd->cpu_ids = (uint8_t*)perf_malloc(cpu_count * sizeof(uint8_t));
        if (cmd->cpu_ids == NULL) {
            rt_kprintf("Failed to allocate memory for cpu_id array.\n");
            perf_free(cpu_values_copy);
            perf_free(input_copy);
            perf_free(cmd->events);
            perf_free(cmd);
            return NULL;
        }
        cmd->nr_cpus = cpu_count;

        // Tokenize again to fill the cpu_id array
        strcpy(cpu_values_copy, cpu_values); // Reset the copy
        temp_rest = cpu_values_copy;
        uint8_t i = 0;
        while ((temp_token = strtok_r(temp_rest, ",", &temp_rest))) {
            uint8_t cpu_id = (uint8_t)atoi(temp_token);
            if (cpu_id < perf_num_cpus_get())
            {
                cmd->cpu_ids[i++] = cpu_id;
            }
            else
            {
                cmd->nr_cpus--;
            }
        }

        perf_free(cpu_values_copy);
    }

    // Process the thread values (-t option)
    if (thread_values) {
        // Duplicate the thread values string to tokenize
        char* thread_values_copy = strdup(thread_values);
        if (thread_values_copy == NULL) {
            rt_kprintf("Failed to duplicate thread values string.\n");
            perf_free(input_copy);
            perf_free(cmd->events);
            perf_free(cmd->cpu_ids);
            perf_free(cmd);
            return NULL;
        }

        // Count the number of threads
        uint32_t thread_count = 0;
        char* temp_token;
        char* temp_rest = thread_values_copy;
        while ((temp_token = strtok_r(temp_rest, ",", &temp_rest))) {
            thread_count++;
        }

        // Allocate memory for the tid array
        cmd->tids = (perf_thread_id*)perf_malloc(thread_count * sizeof(rt_thread_t));
        if (cmd->tids == NULL) {
            rt_kprintf("Failed to allocate memory for tid array.\n");
            perf_free(thread_values_copy);
            perf_free(input_copy);
            perf_free(cmd->events);
            perf_free(cmd->cpu_ids);
            perf_free(cmd);
            return NULL;
        }
        cmd->nr_threads = thread_count;

        // Tokenize again to fill the tid array
        strcpy(thread_values_copy, thread_values); // Reset the copy
        temp_rest = thread_values_copy;
        uint32_t i = 0;
        while ((temp_token = strtok_r(temp_rest, ",", &temp_rest))) {
            // uint32_t tid = (uint32_t)atoi(temp_token);
            char *endptr;
            rt_thread_t tid = (rt_thread_t)strtol(temp_token, &endptr, 0);
            if (endptr == temp_token || *endptr != '\0')
            {
                rt_kprintf("Conversion error: invalid hexadecimal string\n");
                return NULL;
            }
            cmd->tids[i++] = tid;
        }

        perf_free(thread_values_copy);
    }
    perf_free(input_copy);
    return cmd;
}
