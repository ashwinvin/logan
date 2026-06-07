#include "usb_handler.h"
#include "cmd_handler.h"

#include <FreeRTOS.h>
#include <queue.h>

#include <pico/stdio.h>
#include <tusb.h>

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
  }

  if (received_bytes == COMMAND_MSG_LEN + 1) {

    start = false;
    stop = true;

    memset(msg_buf, 0, COMMAND_MSG_LEN);
    received_bytes = 0;
  };
}
