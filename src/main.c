#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "usb_handler.h"

#include <FreeRTOS.h>
#include <task.h>

#include <bsp/board_api.h>
#include <tusb.h>

#define LED_PIN 4
#define USBD_STACK_SIZE 3 * configMINIMAL_STACK_SIZE / 2

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
  }
}

void led_status_task(void *params) {
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  while (1) {
    vTaskDelay(pdMS_TO_TICKS(led_status_interval));
    if (usb_status == USB_DISCONNECTED)
      continue;
    vTaskDelay(pdMS_TO_TICKS(usb_status * 2000));
    gpio_put(LED_PIN, true);
    vTaskDelay(pdMS_TO_TICKS(led_status_interval));
    vTaskDelay(pdMS_TO_TICKS(usb_status * 2000));
    gpio_put(LED_PIN, false);
    printf("Blinkyyy");
  }
}

int main() {

  usb_status = USB_DISCONNECTED;
  board_init();
  stdio_usb_init();
  stdio_usb_init(); // Logging interface

  xTaskCreate(usb_device_task, "USB", USBD_STACK_SIZE, NULL,
              configMAX_PRIORITIES - 1, NULL);
  xTaskCreate(led_status_task, "LED", 256, NULL, configMAX_PRIORITIES - 3,
              NULL);

  vTaskStartScheduler();

  return 0;
}
