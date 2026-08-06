#include <Arduino.h>
//#include "hit_target_game.h"
#include "oled_display.h"
#include "game_selector.h"
#include "button_input.h"
#include "neopixel_display.h"

void setup(void)
{
  Serial.begin(9600);

  init_button_input(BUTTON_ACTION, A0);
  init_button_input(BUTTON_MENU, A1);
  init_neopixel_display();

  if (!init_oled_display())
  {
    Serial.println("OLED initialization failed!");
  }

  init_game_selector();

}

void loop(void)
{
  unsigned long current_time = millis();

  update_button_input(current_time);

  update_game_selector(current_time
  );
}
