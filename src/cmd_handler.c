#include "cmd_handler.h"
#include "usb_handler.h"

#include <FreeRTOS.h>
#include <pico/stdlib.h>
#include <queue.h>
#include <stdio.h>

typedef bool (*CommandHandler)(uint8_t *payload, size_t len);

bool cmd_arm_analyzer(uint8_t *payload, size_t len) { return true; }

bool cmd_disarm_analyzer(uint8_t *payload, size_t len) { return true; }

bool cmd_set_frequency(uint8_t *payload, size_t len) { return true; }

bool cmd_enable_pin(uint8_t *payload, size_t len) { return true; }

bool cmd_disable_pin(uint8_t *payload, size_t len) { return true; }

typedef struct CommandMapping_t {
  uint8_t id;
  CommandHandler handler;
} CommandMapping_t;

CommandMapping_t commands[NUM_CMDS] = {
    (CommandMapping_t){ARM, *cmd_arm_analyzer},
    (CommandMapping_t){DISARM, *cmd_disarm_analyzer},
    (CommandMapping_t){SET_FREQ, *cmd_set_frequency},
    (CommandMapping_t){EN_PIN, *cmd_enable_pin},
    (CommandMapping_t){DS_PIN, *cmd_disable_pin},
};

void cmd_handler_task(void *params) {
  (void)params;
  uint8_t msg_buf[2]; // TODO: Retrieve cmd msg len from header file
  BaseType_t status;
  char cmd_status[1];

  while (true) {
    cmd_status[0] = CMD_UNKNOWN;
    status = xQueueReceive(cmd_msg_queue, msg_buf,
                           portMAX_DELAY); // Block until a cmd is received
    if (status != pdPASS)
      continue;

    for (size_t i = 0; i < NUM_CMDS; i++) {
      if (commands[i].id == msg_buf[0]) {
        cmd_status[0] =
            commands[i].handler(msg_buf + 1, 1) ? CMD_SUCESS : CMD_FAIL;
        printf("CMD: %d returned %d \n", commands[i].id, cmd_status[0]);
        break;
      }
    }
    usb_cmd_send(cmd_status, 1);
  }
}
