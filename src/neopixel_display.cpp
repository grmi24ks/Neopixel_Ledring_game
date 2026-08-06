#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "neopixel_display.h"

#define DATA_PIN 5
#define PIXEL_COUNT 24

int get_neopixel_count(void)
{
    return PIXEL_COUNT;
}

static Adafruit_NeoPixel ring(
    PIXEL_COUNT,
    DATA_PIN,
    NEO_GRB + NEO_KHZ800
);

void init_neopixel_display(void)
{
  ring.begin();
}

void clear_neopixel_display(void)
{
  ring.clear();
}

void set_neopixel_color(int position, int red, int green, int blue)
{
  ring.setPixelColor(
      position,
      ring.Color(red, green, blue)
  );
}

void fill_neopixel_display(int red, int green, int blue)
{
  ring.fill(
      ring.Color(red, green, blue)
  );
}

void show_neopixel_display(void)
{
  ring.show();
}