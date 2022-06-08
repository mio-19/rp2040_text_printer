#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "ssd1306.h"

#define BTN_PIN 9

#define I2C_X i2c1
#define SDA_PIN 2
#define SCL_PIN 3

/*
#define I2C_X i2c0
#define SDA_PIN 12
#define SCL_PIN 13
*/

ssd1306_t disp;
void init() {
    stdio_init_all();

    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);

    i2c_init(I2C_X, 400000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);
    disp.external_vcc=false;
    ssd1306_init(&disp, 128, 32, 0x3C, I2C_X);
    ssd1306_clear(&disp);
}

bool btn_get(){
    return !gpio_get(BTN_PIN);
}

#define SCREEN_HEIGHT 32
#define SCREEN_WIDTH 128
#define FONT_HEIGHT 8
#define FONT_WIDTH 5
#define SCREEN_CHAR_HEIGHT (SCREEN_HEIGHT/FONT_HEIGHT)
#define SCREEN_CHAR_WIDTH (SCREEN_WIDTH/FONT_WIDTH)

#define OLED_BUFFER_SIZE (SCREEN_CHAR_HEIGHT * SCREEN_CHAR_WIDTH)
char oled_buffer[OLED_BUFFER_SIZE];
#define oled_ui(...) {snprintf(oled_buffer, OLED_BUFFER_SIZE, __VA_ARGS__);flush_oled_buffer();}
void flush_oled_buffer() {
  ssd1306_clear(&disp);
  unsigned char y = 0;
  unsigned char x = 0;
  for (size_t i = 0; oled_buffer[i]; i++) {
    if (oled_buffer[i] == '\r')
      continue;
    if (oled_buffer[i] == '\t')
      continue; // TODO
    if (oled_buffer[i] == '\n') {
      y++;
      x = 0;
      continue;
    }
    if (x > (SCREEN_WIDTH-FONT_WIDTH)) {
      y++;
      x = 0;
    }
    if (y >= SCREEN_CHAR_HEIGHT)
      break;
    ssd1306_draw_char(&disp, x, y*FONT_HEIGHT, 1, oled_buffer[i]);
    x += FONT_WIDTH;
  }
  for (;;) {
    if (x > (SCREEN_WIDTH-FONT_WIDTH)) {
      y++;
      x = 0;
    }
    if (y >= SCREEN_CHAR_HEIGHT)
      break;
    ssd1306_draw_char(&disp, x, y*FONT_HEIGHT, 1, ' ');
    x += FONT_WIDTH;
  }
  ssd1306_show(&disp);
}

// 0.01s
#define CYCLE 10
int main() {
    init();
    uint64_t count=0;
    for(;;) {
        sleep_ms(CYCLE);
        count++;
        // every 0.05s
        if(count%(50/CYCLE)==0){
            oled_ui("Welcome to the Device");
        }
    }
    return 0;
}