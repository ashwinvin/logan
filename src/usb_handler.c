#include "usb_handler.h"
#include "cmd_handler.h"

#include <FreeRTOS.h>
#include <queue.h>

#include <pico/stdio.h>
#include <tusb.h>

// Application communication
void _usb_send(uint8_t itf, char *data, size_t len) {
  tud_cdc_n_write_char(itf, CMD_MSG_START);
  tud_cdc_n_write(itf, data, len);
  tud_cdc_n_write_char(itf, CMD_MSG_END);
  tud_cdc_n_write_flush(itf);
}

void usb_cmd_send(char *data, size_t len) { _usb_send(CMD_ITF, data, len); }

void usb_data_send(char *data, size_t len) { _usb_send(DATA_ITF, data, len); }

// TinyUSB Callbacks
void tud_mount_cb(void) { usb_status = USB_MOUNTED; }

void tud_umount_cb(void) { usb_status = USB_DISCONNECTED; }

void tud_suspend_cb(bool remote_wakeup_en) { usb_status = USB_SUSPENDED; }

void tud_resume_cb(void) {
  usb_status = tud_mounted() ? USB_MOUNTED : USB_DISCONNECTED;
}

void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts) {
  (void)itf;
  (void)rts;

  // TODO set some indicator
  if (dtr) {
    // Terminal connected
  } else {
    // Terminal disconnected
  }
}

// Invoked when CDC interface received data from host
void tud_cdc_rx_cb(uint8_t itf) {
  if (itf != CMD_ITF)
    return;

  static uint8_t msg_buf[CMD_MSG_LEN];
  static size_t received_bytes = 0;
  static bool start = false, stop = true;

  while (tud_cdc_n_available(itf)) {
    char buf = tud_cdc_n_read_char(itf);

    if (buf == CMD_MSG_START && stop) {
      start = true;
      stop = false;

    } else if (start && buf != CMD_MSG_END) {
      msg_buf[received_bytes] = buf;
      received_bytes += 1;

    } else if ((received_bytes == CMD_MSG_LEN) && (buf == CMD_MSG_END)) {
      xQueueSendToBack(cmd_msg_queue, msg_buf, pdMS_TO_TICKS(500));

      printf("Pushed command ");
      for (size_t i = 0; i < CMD_MSG_LEN; i++)
        printf("%u ", msg_buf[i]);
      printf("to queue\n");
      received_bytes += 1;

    } else { // The host sent an invalid command or forgot the end token.
      // Reset the msg buffer and wait for start token.
      received_bytes = CMD_MSG_LEN + 1;
    }
  }

  if (received_bytes == CMD_MSG_LEN + 1) {

    start = false;
    stop = true;

    memset(msg_buf, 0, CMD_MSG_LEN);
    received_bytes = 0;
  };
}
