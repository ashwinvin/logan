#include <pico/stdlib.h>
enum {
  USB_DISCONNECTED = 0,
  USB_MOUNTED,
  USB_SUSPENDED,
};

static volatile uint8_t usb_status = USB_DISCONNECTED;

void tud_mount_cb(void);
void tud_umount_cb(void);
void tud_suspend_cb(bool remote_wakeup_en);
void tud_resume_cb(void);
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts);
void tud_cdc_rx_cb(uint8_t itf);

