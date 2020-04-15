/******************************************************************************
 *
 *  Copyright (C) 2014 Google, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/
#include <string.h>

#include <aos/debug.h>
#include <aos/cli.h>
#include "common/bt_defs.h"
#include "common/bt_trace.h"
#include "stack/hcidefs.h"
#include "stack/hcimsgs.h"
#include "stack/btu.h"
#include "hci/hci_internals.h"
#include "hci/hci_hal.h"
#include "hci/hci_layer.h"
#include "hci/hci_layer.h"
#include "hci_h5_mp.h"

#include "osi/osi.h"
#include "osi/allocator.h"
#include "osi/list.h"
#include "osi/alarm.h"
#include "osi/thread.h"
#include "osi/mutex.h"
#include "osi/fixed_queue.h"

#define HCI_HOST_TASK_PINNED_TO_CORE    (TASK_PINNED_TO_CORE)
#define HCI_HOST_TASK_STACK_SIZE        (2048 + BT_TASK_EXTRA_STACK_SIZE)
#define HCI_HOST_TASK_PRIO              (BT_TASK_MAX_PRIORITIES - 3)
#define HCI_HOST_TASK_NAME              "hciT"

typedef struct {
    uint16_t opcode;
    future_t *complete_future;
    command_complete_cb complete_callback;
    command_status_cb status_callback;
    void *context;
    BT_HDR *command;
} waiting_command_t;

typedef struct {
    bool timer_is_set;
    osi_alarm_t *command_response_timer;
    list_t *commands_pending_response;
    osi_mutex_t commands_pending_response_lock;
} command_waiting_response_t;

typedef struct {
    int command_credits;
    fixed_queue_t *command_queue;

    command_waiting_response_t cmd_waiting_q;

    /*
      non_repeating_timer_t *command_response_timer;
      list_t *commands_pending_response;
      osi_mutex_t commands_pending_response_lock;
    */
} hci_host_env_t;

// Using a define here, because it can be stringified for the property lookup
static const uint32_t COMMAND_PENDING_TIMEOUT = 8000;

// Our interface
static bool interface_created;
static hci_t interface;
static hci_host_env_t hci_host_env;
static osi_thread_t *hci_host_thread;
static bool hci_host_startup_flag;
static const hci_h5_mp_t *h5_int_interface;

// Modules we import and callbacks we export
static hci_hal_callbacks_t hal_callbacks;

static int hci_layer_init_env(void);
static void hci_layer_deinit_env(void);
static void hci_host_thread_handler(void *arg);
static void event_command_ready(fixed_queue_t *queue);
static void restart_command_waiting_response_timer(command_waiting_response_t *cmd_wait_q);
static void command_timed_out(void *context);
static void hal_says_packet_ready(BT_HDR *packet);
static bool filter_incoming_event(BT_HDR *packet);
static serial_data_type_t event_to_data_type(uint16_t event);
static waiting_command_t *get_waiting_command(command_opcode_t opcode);
static void dispatch_reassembled(BT_HDR *packet);
static future_t *transmit_command_futured(BT_HDR *command);

static void init_layer_interface(void);

// Module lifecycle functions
static void hci_bt_mp_start(char *wbuf, int wbuf_len, int argc, char **argv)
{
    osi_alarm_create_mux();
    osi_alarm_init();
    if (hci_layer_init_env()) {
        goto error;
    }

    hci_host_thread = osi_thread_create(HCI_HOST_TASK_NAME, HCI_HOST_TASK_STACK_SIZE, HCI_HOST_TASK_PRIO, HCI_HOST_TASK_PINNED_TO_CORE, 2);
    if (hci_host_thread == NULL) {
        return;
    }

    init_layer_interface();
extern const hci_h5_mp_t *hci_get_h5_int_interface_mp();
    h5_int_interface = hci_get_h5_int_interface_mp();

    h5_int_interface->h5_int_init(&hal_callbacks);

    hci_host_startup_flag = true;

    printf("start success\n");

    return;
error:
    hci_shut_down();
    return;
}

static void hci_bt_mp_end(char *wbuf, int wbuf_len, int argc, char **argv)
{
    h5_int_interface->h5_int_cleanup();
    hci_host_startup_flag  = false;
    hci_layer_deinit_env();

    osi_thread_free(hci_host_thread);
    hci_host_thread = NULL;

    printf("end success\n");
}


static void hci_bt_mp_test(char *wbuf, int wbuf_len, int argc, char **argv)
{
    BT_HDR *response;
    uint16_t opcode;
    uint32_t parameter_size;
    uint8_t param[32];
    int i;
    char *endptr;

    if (argc < 3) {
        printf("too few argc\n");
        return;
    }

    opcode =  strtol(argv[1], &endptr, 16);

    if (endptr == NULL) {
        printf("arg error\n");
        return;
    }

    parameter_size = strtol(argv[2], &endptr, 16);

    if (endptr == NULL) {
        printf("arg error\n");
        return;
    }

    if (argc != parameter_size + 3 || parameter_size > 32) {
        printf("too few argc\n");
        return;
    }

    for (i = 0 ; i < parameter_size; i ++) {
        param[i] = strtol(argv[3 + i], &endptr, 16);

        if (endptr == NULL) {
            printf("arg error\n");
            return;
        }
    }

    BT_HDR *command = (BT_HDR *)calloc(1, sizeof(BT_HDR) + HCI_COMMAND_PREAMBLE_SIZE + parameter_size);

    if (command == NULL) {
        printf("malloc error\n");
        return;
    }

    command->event = 0;
    command->offset = 0;
    command->layer_specific = 0;
    command->len = HCI_COMMAND_PREAMBLE_SIZE + parameter_size;

    uint8_t *stream = command->data;
    UINT16_TO_STREAM(stream, opcode);
    UINT8_TO_STREAM(stream, parameter_size);
    ARRAY_TO_STREAM(stream, param, parameter_size);

    response = future_await(transmit_command_futured(command));

    if (response == NULL) {
        printf("command error");
        return;
    }

    printf("%02x ", response->len);
    for (i = 0; i < response->len; i++) {
        printf("%02x ", response->data[i]);
    }

    printf("\n");

    return;
}

static bool hci_host_task_post_mp(uint32_t timeout)
{
    return osi_thread_post(hci_host_thread, hci_host_thread_handler, NULL, 0, timeout);
}

static int hci_layer_init_env(void)
{
    command_waiting_response_t *cmd_wait_q;

    // The host is only allowed to send at most one command initially,
    // as per the Bluetooth spec, Volume 2, Part E, 4.4 (Command Flow Control)
    // This value can change when you get a command complete or command status event.
    hci_host_env.command_credits = 1;
    hci_host_env.command_queue = fixed_queue_new(QUEUE_SIZE_MAX);
    if (hci_host_env.command_queue) {
        fixed_queue_register_dequeue(hci_host_env.command_queue, event_command_ready);
    } else {
        HCI_TRACE_ERROR("%s unable to create pending command queue.", __func__);
        return -1;
    }

    // Init Commands waiting response list and timer
    cmd_wait_q = &hci_host_env.cmd_waiting_q;
    cmd_wait_q->timer_is_set = false;
    cmd_wait_q->commands_pending_response = list_new(NULL);
    if (!cmd_wait_q->commands_pending_response) {
        HCI_TRACE_ERROR("%s unable to create list for commands pending response.", __func__);
        return -1;
    }
    osi_mutex_new(&cmd_wait_q->commands_pending_response_lock);
    cmd_wait_q->command_response_timer = osi_alarm_new("cmd_rsp_to", command_timed_out, cmd_wait_q, COMMAND_PENDING_TIMEOUT);
    if (!cmd_wait_q->command_response_timer) {
        HCI_TRACE_ERROR("%s unable to create command response timer.", __func__);
        return -1;
    }

    return 0;
}

static void hci_layer_deinit_env(void)
{
    command_waiting_response_t *cmd_wait_q;

    if (hci_host_env.command_queue) {
        fixed_queue_free(hci_host_env.command_queue, osi_free_func);
    }

    cmd_wait_q = &hci_host_env.cmd_waiting_q;
    list_free(cmd_wait_q->commands_pending_response);
    osi_mutex_free(&cmd_wait_q->commands_pending_response_lock);
    osi_alarm_free(cmd_wait_q->command_response_timer);
    cmd_wait_q->command_response_timer = NULL;
}

static void hci_host_thread_handler(void *arg)
{
    /*
     * Previous task handles RX queue and two TX Queues, Since there is
     * a RX Thread Task in H4 layer which receives packet from driver layer.
     * Now HCI Host Task has been optimized to only process TX Queue
     * including command and data queue. And command queue has high priority,
     * All packets will be directly copied to single queue in driver layer with
     * H4 type header added (1 byte).
     */
    if (!fixed_queue_is_empty(hci_host_env.command_queue) &&
            hci_host_env.command_credits > 0) {
        fixed_queue_process(hci_host_env.command_queue);
    }
}

static void transmit_command(
    BT_HDR *command,
    command_complete_cb complete_callback,
    command_status_cb status_callback,
    void *context)
{
    uint8_t *stream;
    waiting_command_t *wait_entry = osi_calloc(sizeof(waiting_command_t));
    if (!wait_entry) {
        HCI_TRACE_ERROR("%s couldn't allocate space for wait entry.", __func__);
        return;
    }

    stream = command->data + command->offset;
    STREAM_TO_UINT16(wait_entry->opcode, stream);
    wait_entry->complete_callback = complete_callback;
    wait_entry->status_callback = status_callback;
    wait_entry->command = command;
    wait_entry->context = context;

    // Store the command message type in the event field
    // in case the upper layer didn't already
    command->event = MSG_STACK_TO_HC_HCI_CMD;
    HCI_TRACE_DEBUG("HCI Enqueue Comamnd opcode=0x%x\n", wait_entry->opcode);
    BTTRC_DUMP_BUFFER(NULL, command->data + command->offset, command->len);

    fixed_queue_enqueue(hci_host_env.command_queue, wait_entry, FIXED_QUEUE_MAX_TIMEOUT);
    hci_host_task_post_mp(OSI_THREAD_MAX_TIMEOUT);

}

static future_t *transmit_command_futured(BT_HDR *command)
{
    waiting_command_t *wait_entry = osi_calloc(sizeof(waiting_command_t));
    assert(wait_entry != NULL);

    future_t *future = future_new();

    uint8_t *stream = command->data + command->offset;
    STREAM_TO_UINT16(wait_entry->opcode, stream);
    wait_entry->complete_future = future;
    wait_entry->command = command;

    // Store the command message type in the event field
    // in case the upper layer didn't already
    command->event = MSG_STACK_TO_HC_HCI_CMD;

    fixed_queue_enqueue(hci_host_env.command_queue, wait_entry, FIXED_QUEUE_MAX_TIMEOUT);
    hci_host_task_post_mp(OSI_THREAD_MAX_TIMEOUT);
    return future;
}

static void transmit_downward(uint16_t type, void *data)
{
    if (type == MSG_STACK_TO_HC_HCI_CMD) {
        transmit_command((BT_HDR *)data, NULL, NULL, NULL);
        HCI_TRACE_WARNING("%s legacy transmit of command. Use transmit_command instead.\n", __func__);
    }

    hci_host_task_post_mp(OSI_THREAD_MAX_TIMEOUT);
}

static serial_data_type_t event_to_data_type(uint16_t event)
{
    if (event == MSG_STACK_TO_HC_HCI_ACL) {
        return DATA_TYPE_ACL;
    } else if (event == MSG_STACK_TO_HC_HCI_SCO) {
        return DATA_TYPE_SCO;
    } else if (event == MSG_STACK_TO_HC_HCI_CMD) {
        return DATA_TYPE_COMMAND;
    } else {
        HCI_TRACE_ERROR("%s invalid event type, could not translate 0x%x\n", __func__, event);
    }

    return 0;
}

static void transmit_cmd(BT_HDR *packet)
{
    uint16_t event = packet->event & MSG_EVT_MASK;
    uint16_t opcode;
    serial_data_type_t type = event_to_data_type(event);
    uint8_t  *data_temp = packet->data + packet->offset;

    switch (type) {
        case DATA_TYPE_COMMAND:
            STREAM_TO_UINT16(opcode, data_temp);

            if (opcode == HCI_VSC_H5_INIT) {
                h5_int_interface->h5_send_sync_cmd(opcode, NULL, packet->len);
                break;
            }

            h5_int_interface->h5_send_cmd(type, packet->data + packet->offset, packet->len);
            break;

        default:
            break;
    }
}


// Command/packet transmitting functions
static void event_command_ready(fixed_queue_t *queue)
{
    waiting_command_t *wait_entry = NULL;
    command_waiting_response_t *cmd_wait_q = &hci_host_env.cmd_waiting_q;

    wait_entry = fixed_queue_dequeue(queue, FIXED_QUEUE_MAX_TIMEOUT);

    hci_host_env.command_credits--;
    // Move it to the list of commands awaiting response
    osi_mutex_lock(&cmd_wait_q->commands_pending_response_lock, OSI_MUTEX_MAX_TIMEOUT);
    list_append(cmd_wait_q->commands_pending_response, wait_entry);
    osi_mutex_unlock(&cmd_wait_q->commands_pending_response_lock);

    transmit_cmd(wait_entry->command);

    restart_command_waiting_response_timer(cmd_wait_q);
}

static void restart_command_waiting_response_timer(command_waiting_response_t *cmd_wait_q)
{
    osi_mutex_lock(&cmd_wait_q->commands_pending_response_lock, OSI_MUTEX_MAX_TIMEOUT);
    if (cmd_wait_q->timer_is_set) {
        osi_alarm_cancel(cmd_wait_q->command_response_timer);
        cmd_wait_q->timer_is_set = false;
    }
    if (!list_is_empty(cmd_wait_q->commands_pending_response)) {
        osi_alarm_set(cmd_wait_q->command_response_timer, COMMAND_PENDING_TIMEOUT);
        cmd_wait_q->timer_is_set = true;
    }
    osi_mutex_unlock(&cmd_wait_q->commands_pending_response_lock);
}

static void command_timed_out(void *context)
{
    command_waiting_response_t *cmd_wait_q = (command_waiting_response_t *)context;
    waiting_command_t *wait_entry;

    osi_mutex_lock(&cmd_wait_q->commands_pending_response_lock, OSI_MUTEX_MAX_TIMEOUT);
    wait_entry = (list_is_empty(cmd_wait_q->commands_pending_response) ?
                  NULL : list_front(cmd_wait_q->commands_pending_response));
    osi_mutex_unlock(&cmd_wait_q->commands_pending_response_lock);

    if (wait_entry == NULL) {
        HCI_TRACE_ERROR("%s with no commands pending response", __func__);
    } else
        // We shouldn't try to recover the stack from this command timeout.
        // If it's caused by a software bug, fix it. If it's a hardware bug, fix it.
    {
        HCI_TRACE_ERROR("%s hci layer timeout waiting for response to a command. opcode: 0x%x", __func__, wait_entry->opcode);
    }
}

// Event/packet receiving functions
static void hal_says_packet_ready(BT_HDR *packet)
{
    filter_incoming_event(packet);
}

// Returns true if the event was intercepted and should not proceed to
// higher layers. Also inspects an incoming event for interesting
// information, like how many commands are now able to be sent.
static bool filter_incoming_event(BT_HDR *packet)
{
    waiting_command_t *wait_entry = NULL;
    uint8_t *stream = packet->data + packet->offset;
    uint8_t event_code;
    command_opcode_t opcode;

    STREAM_TO_UINT8(event_code, stream);
    STREAM_SKIP_UINT8(stream); // Skip the parameter total length field

    HCI_TRACE_DEBUG("Receive packet event_code=0x%x\n", event_code);

    if (event_code == HCI_COMMAND_COMPLETE_EVT) {
        STREAM_TO_UINT8(hci_host_env.command_credits, stream);
        STREAM_TO_UINT16(opcode, stream);
        wait_entry = get_waiting_command(opcode);
        if (!wait_entry) {
            HCI_TRACE_WARNING("%s command complete event with no matching command. opcode: 0x%x.", __func__, opcode);
        } else if (wait_entry->complete_callback) {
            wait_entry->complete_callback(packet, wait_entry->context);
        } else if (wait_entry->complete_future) {
            future_ready(wait_entry->complete_future, packet);
        }

        goto intercepted;
    } else if (event_code == HCI_COMMAND_STATUS_EVT) {
        uint8_t status;
        STREAM_TO_UINT8(status, stream);
        STREAM_TO_UINT8(hci_host_env.command_credits, stream);
        STREAM_TO_UINT16(opcode, stream);

        // If a command generates a command status event, it won't be getting a command complete event

        wait_entry = get_waiting_command(opcode);
        if (!wait_entry) {
            HCI_TRACE_WARNING("%s command status event with no matching command. opcode: 0x%x", __func__, opcode);
        } else if (wait_entry->status_callback) {
            wait_entry->status_callback(status, wait_entry->command, wait_entry->context);
        }

        goto intercepted;
    }

    return false;
intercepted:
    restart_command_waiting_response_timer(&hci_host_env.cmd_waiting_q);

    /*Tell HCI Host Task to continue TX Pending commands*/
    if (hci_host_env.command_credits &&
            !fixed_queue_is_empty(hci_host_env.command_queue)) {
        hci_host_task_post_mp(OSI_THREAD_MAX_TIMEOUT);
    }

    if (wait_entry) {
        // If it has a callback, it's responsible for freeing the packet
        if (event_code == HCI_COMMAND_STATUS_EVT ||
                (!wait_entry->complete_callback && !wait_entry->complete_future)) {
            osi_free(packet);
        }

        // If it has a callback, it's responsible for freeing the command
        if (event_code == HCI_COMMAND_COMPLETE_EVT || !wait_entry->status_callback) {
            osi_free(wait_entry->command);
        }

        osi_free(wait_entry);
    } else {
        osi_free(packet);
    }

    return true;
}

static waiting_command_t *get_waiting_command(command_opcode_t opcode)
{
    command_waiting_response_t *cmd_wait_q = &hci_host_env.cmd_waiting_q;
    osi_mutex_lock(&cmd_wait_q->commands_pending_response_lock, OSI_MUTEX_MAX_TIMEOUT);

    for (const list_node_t *node = list_begin(cmd_wait_q->commands_pending_response);
            node != list_end(cmd_wait_q->commands_pending_response);
            node = list_next(node)) {
        waiting_command_t *wait_entry = list_node(node);
        if (!wait_entry || wait_entry->opcode != opcode) {
            continue;
        }

        list_remove(cmd_wait_q->commands_pending_response, wait_entry);

        osi_mutex_unlock(&cmd_wait_q->commands_pending_response_lock);
        return wait_entry;
    }

    osi_mutex_unlock(&cmd_wait_q->commands_pending_response_lock);
    return NULL;
}

static void init_layer_interface(void)
{
    if (!interface_created) {
        interface.transmit_command = transmit_command;
        interface.transmit_command_futured = transmit_command_futured;
        interface.transmit_downward = transmit_downward;
        interface_created = true;
    }
}

static hci_hal_callbacks_t hal_callbacks = {
    hal_says_packet_ready
};

const hci_t *hci_layer_get_interface_mp(void)
{
    init_layer_interface();
    return &interface;
}


void cli_reg_cmd_bt_test_start(void)
{
    static const struct cli_command cmd_info = {
        "bt_mp_start",
        "bt_mp_start",
         hci_bt_mp_start};

    aos_cli_register_command(&cmd_info);
}

void cli_reg_cmd_bt_test_end(void)
{
    static const struct cli_command cmd_info = {
        "bt_mp_end",
        "bt_mp_end",
         hci_bt_mp_end};

    aos_cli_register_command(&cmd_info);
}

void cli_reg_cmd_bt_test(void)
{
    static const struct cli_command cmd_info = {
        "bt_mp_test",
        "bt_mp_test",
         hci_bt_mp_test};

    aos_cli_register_command(&cmd_info);
}

// void cli_reg_cmd_bt_test(void)
// {
//     static const struct cli_command cmd_info = {
//         "bt_mp_end",
//         "bt_test",
//          cmd_bt_test_func};

//     aos_cli_register_command(&cmd_info);
// }

// void cli_reg_cmd_bt_test(void)
// {
//     static const struct cli_command cmd_info = {
//         "bt_mp_start",
//         "bt_test",
//          cmd_bt_test_func};

//     aos_cli_register_command(&cmd_info);
// }

