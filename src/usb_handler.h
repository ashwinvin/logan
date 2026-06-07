#ifndef USB_HANDLER_H
#define USB_HANDLER_H

#include <pico/stdlib.h>
#include <FreeRTOS.h>
#include <queue.h>

#define CMD_ITF 2
#define DATA_ITF 3

#define CMD_MSG_LEN 2 // in bytes

extern QueueHandle_t cmd_msg_queue;

enum {
  USB_DISCONNECTED = 0,
  USB_MOUNTED,
  USB_SUSPENDED,
};

extern volatile uint8_t usb_status; 

void tud_mount_cb(void);
void tud_umount_cb(void);
void tud_suspend_cb(bool remote_wakeup_en);
void tud_resume_cb(void);
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts);
void tud_cdc_rx_cb(uint8_t itf);


void usb_cmd_send(char *data, size_t len);
void usb_data_send(char *data, size_t len);

#endif
