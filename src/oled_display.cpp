#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "oled_display.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

static Adafruit_SSD1306 display(
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    &Wire,
    OLED_RESET
);

bool init_oled_display(void)
{
  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS))
  {
    return false;
  }

  display.clearDisplay();                   //tömmer buffert
  display.display();                        //skriver ut på display:en det som finns i buffert

  return true;
}

void show_oled_test_screen(void)
{
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  display.println("NeoPixel Game");           //fyller buffert med data
  display.println();
  display.println("OLED OK");
  display.println("I2C: 0x3C");

  display.display();                           //skriver ut på display:en det som finns i bufferten
}

void show_game_menu(int selected_game)
{
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  display.println("SELECT GAME");
  display.println();

  if (selected_game == 0)
  {
    display.println("> HIT TARGET");
    display.println("  PENDULUM");
  }
  else
  {
    display.println("  HIT TARGET");
    display.println("> PENDULUM");
  }

  display.display();
}