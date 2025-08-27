 /**
 * @file FIRMWARE PRESSURE METER
 * @author Robert R. Gomes
 * @brief PRESSURE MEASUREMENT DEVICE
 * @version 1.0
 * @date 03-06-2025
 * 
 * @copyright Copyright (c) 2025
 * 
*/

#include <Arduino.h>
#include <TFT_ST7735.h>
#include <SPI.h>

TFT_ST7735 tft = TFT_ST7735();

#include "MPX5010DP.h"
#include "Logo.h"

#define BUTTON_PIN    8
#define BUFF_SIZE     64

unsigned long timer0, timer1, timer2;

void buttom_is_pressed(void);
void init_display(void);
void reset_screen(void);
void clear_screen(void);
void show_logo(void);
void update_display(void);
void draw_icon(const unsigned short* icon, int16_t x, int16_t y, int8_t width, int8_t height);

void setup() {
  Serial.begin(115200);
  Serial.println(F("\nINIT DEVICE"));

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  init_display();
  reset_screen();
  show_logo();
  reset_screen();

  timer0 = timer1 = timer2 = millis();
}

void loop() {

  if (millis() - timer0 >= 20) {
    timer0 = millis();

    MPX5010_update_pressure();
    MPX5010_filtered_pressure();
  }

  if (millis() - timer1 >= 300) {
    timer1 = millis();

    update_display();
    Serial.println(mpx.filteredPressure);
  }

  if (millis() - timer2 >= 2000) {
    timer2 = millis();

    clear_screen();
  }

  buttom_is_pressed();
}


void buttom_is_pressed(void) {
  if (!digitalRead(BUTTON_PIN)) {
    while (true) {
      if (digitalRead(BUTTON_PIN)) {
        MPX5010_set_offset(mpx.filteredPressure);
        Serial.println(F("Sensor Calibrado!"));
        break;
      }
    }
  }
}


void init_display(void) {
  Serial.println(F("INIT DISPLAY"));

  tft.init();
  tft.setRotation(0);
  tft.fillScreen(ST7735_YELLOW);
}


void reset_screen(void) {
  // tft.setRotation(0);
  // tft.fillScreen(ST7735_YELLOW);

  tft.setRotation(2 * random(2)); // Rotate randomly to clear display left>right or right>left to reduce monotony!
  tft.fillScreen(ST7735_YELLOW);
  tft.setRotation(0);
}


void clear_screen(void) {
  tft.setTextFont(2);
  tft.setCursor(22, 60);
  tft.println("         ");
}


void show_logo(void) {
  //draw_icon(logo_cmos,  (tft.width() -  logoWidth)/2 - 50, (tft.height() -  logoHeight)/2, logoWidth,  logoHeight);
  draw_icon(logo_cmos, 5, (tft.height() -  logoHeight)/2, logoWidth,  logoHeight);
  delay(3000);
}


void draw_icon(const unsigned short* icon, int16_t x, int16_t y, int8_t width, int8_t height) {

  uint16_t pix_buffer[BUFF_SIZE];   // Pixel buffer (16 bits per pixel)

  // Set up a window the right size to stream pixels into
  tft.setAddrWindow(x, y, x + width - 1, y + height - 1);

  // Work out the number whole buffers to send
  uint16_t nb = ((uint16_t)height * width) / BUFF_SIZE;

  // Fill and send "nb" buffers to TFT
  for (int i = 0; i < nb; i++) {
    for (int j = 0; j < BUFF_SIZE; j++) {
      pix_buffer[j] = pgm_read_word(&icon[i * BUFF_SIZE + j]);
    }
    tft.pushColors(pix_buffer, BUFF_SIZE);
  }

  // Work out number of pixels not yet sent
  uint16_t np = ((uint16_t)height * width) % BUFF_SIZE;

  // Send any partial buffer left over
  if (np) {
    for (int i = 0; i < np; i++) pix_buffer[i] = pgm_read_word(&icon[nb * BUFF_SIZE + i]);
    tft.pushColors(pix_buffer, np);
  }
}


void update_display(void) {
  tft.setTextColor(ST7735_BLACK, 0xFFE0);
  //tft.setTextColor(ST7735_BLACK);
  tft.setTextSize(2);

  tft.setTextFont(1);
  tft.setCursor(20, 40);
  tft.println("PRESSAO");

  tft.setTextFont(2);
  tft.setCursor(32, 60);
  tft.println(mpx.filteredPressure);

  tft.setTextFont(1);
  tft.setCursor(32, 100);
  tft.println("cmH2O");
}