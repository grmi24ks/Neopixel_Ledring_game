#include <Arduino.h>
#include "hit_target_game.h"

void setup(void)
{
  init_hit_target_game();
}

void loop(void)
{
  update_hit_target_game();
}