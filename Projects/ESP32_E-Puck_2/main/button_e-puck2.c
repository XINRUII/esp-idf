/*

File    : button_E-Puck_2.c
Author  : Eliot Ferragni
Date    : 22 november 2017
REV 1.0

Functions to configure and use the button through interruption
*/
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/xtensa_api.h"
#include "freertos/queue.h"
#include "button_e-puck2.h"


#define ESP_INTR_FLAG_DEFAULT	0

static void button_isr_cb(void* arg);

static void button_isr_cb(void* arg)
{
  (void) arg;
  static TickType_t time = 0;

  //time is used to debounce the button. Otherwise with one press on the button,
  //the isr is called several times
  if(time < xTaskGetTickCountFromISR()){
    time = xTaskGetTickCountFromISR() + DEBOUNCE_TIME_MS;
    //do something
  }
}

uint8_t button_is_pressed(void){
	return !gpio_get_level(BUTTON_GPIO);
}

void button_init(void){
  gpio_config_t io_conf;
  //interrupt of falling edge
  io_conf.intr_type = GPIO_PIN_INTR_NEGEDGE;
  //bit mask of the pins
  io_conf.pin_bit_mask = ((uint64_t)1 << BUTTON_GPIO);
  //set as input mode    
  io_conf.mode = GPIO_MODE_INPUT;
  //enable pull-up mode (no pull-up on pin 34 to 39)
  io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
  //disable pull-down mode (no pull-down on pin 34 to 39)
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  gpio_config(&io_conf);

  //install gpio isr service
  gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
  //hook isr handler for specific gpio pin
  gpio_isr_handler_add(BUTTON_GPIO, button_isr_cb, (void*) BUTTON_GPIO);
}