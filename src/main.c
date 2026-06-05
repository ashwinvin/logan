#include "pico/stdlib.h"

#include <FreeRTOS.h>
#include <task.h>

#include <bsp/board_api.h>
#include <tusb.h>

#define LED_PIN 4
#define USBD_STACK_SIZE 3 * configMINIMAL_STACK_SIZE / 2

enum {
  USB_MOUNTED = 3000,
  USB_DISCONNECTED = 1500,
  USB_SUSPENDED = 500,
};

static uint32_t led_status_interval = USB_DISCONNECTED;

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
  while (1) {
    gpio_put(LED_PIN, 1);
    sleep_ms(250);
    gpio_put(LED_PIN, 0);
    sleep_ms(250);
  }
}

void usb_device_task(void *params) {
  tusb_rhport_init_t dev_init = {.role = TUSB_ROLE_DEVICE,
                                 .speed = TUSB_SPEED_AUTO};
  tusb_init(BOARD_TUD_RHPORT, &dev_init);

  if (board_init_after_tusb) {
    board_init_after_tusb();
  }

  while (1) {
    tud_task(); // Waits till a usb event occurs
    tud_cdc_write_flush();
  }
}

void led_status_task(void *params) {
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  while (1) {
    vTaskDelay(pdMS_TO_TICKS(led_status_interval));
    gpio_put(LED_PIN, true);
    vTaskDelay(pdMS_TO_TICKS(led_status_interval));
    gpio_put(LED_PIN, false);
  }
}

void cdc_task(void *params) {
  (void)params;

  while (1) {
    // connected() check for DTR bit
    // Most but not all terminal client set this when making connection
    // if ( tud_cdc_connected() )
    {
      // There are data available
      while (tud_cdc_available()) {
        uint8_t buf[64];

        // read and echo back
        uint32_t count = tud_cdc_read(buf, sizeof(buf));
        (void)count;

        // Echo back
        tud_cdc_write(buf, count);
      }

      tud_cdc_write_flush();
    }

    vTaskDelay(1);
  }
}

int main() {

  board_init();
  stdio_uart_init();

  xTaskCreate(usb_device_task, "USB", USBD_STACK_SIZE, NULL,
              configMAX_PRIORITIES - 1, NULL);
  xTaskCreate(cdc_task, "CDC", 256, NULL, configMAX_PRIORITIES - 2, NULL);
  xTaskCreate(led_status_task, "LED", 256, NULL, configMAX_PRIORITIES - 3,
              NULL);
  // Start scheduler
  vTaskStartScheduler();

  return 0;
}

void tud_mount_cb(void) { led_status_interval = USB_MOUNTED; }

void tud_umount_cb(void) { led_status_interval = USB_DISCONNECTED; }

void tud_suspend_cb(bool remote_wakeup_en) {
  led_status_interval = USB_SUSPENDED;
}

void tud_resume_cb(void) {
  led_status_interval = tud_mounted() ? USB_MOUNTED : USB_DISCONNECTED;
}

//--------------------------------------------------------------------+
// USB CDC
//--------------------------------------------------------------------+

// Invoked when cdc when line state changed e.g connected/disconnected
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
void tud_cdc_rx_cb(uint8_t itf) { (void)itf; }
