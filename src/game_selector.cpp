/*update_game_selector() är appens övergripande state machine. Den körs varje loopvarv och gör olika saker beroende på app_state:

WAITING_FOR_MENU_RELEASE: väntar tills ACTION-knappen släpps och visar sedan menyn.
SHOWING_GAME_MENU: A1 flyttar markeringen och A0 startar valt spel.
RUNNING_HIT_TARGET_GAME: uppdaterar spelet och kontrollerar om det är slut. Då släcks ringen och state ändras till vänteläget.
RUNNING_PENDULUM_GAME: reserverat för det kommande spelet.

Alla return avslutar det aktuella loopvarvet efter att rätt state har hanterats, så att kod för andra states inte också körs.*/

#include <Arduino.h>

#include "game_selector.h"
#include "oled_display.h"
#include "button_input.h"
#include "hit_target_game.h"
#include "neopixel_display.h"

enum AppState
{
  SHOWING_GAME_MENU,
  WAITING_FOR_MENU_RELEASE,
  RUNNING_HIT_TARGET_GAME,
  RUNNING_PENDULUM_GAME,
};

static AppState app_state = SHOWING_GAME_MENU;
static int selected_game = 0;

void init_game_selector(void)
{
  app_state = SHOWING_GAME_MENU;
  selected_game = 0;

  show_game_menu(selected_game);      //skriver ut texten på menyn på oledskärmen, inget annat
}

void update_game_selector(unsigned long current_time)
{
  (void)current_time;
  
  if (app_state ==  WAITING_FOR_MENU_RELEASE)
  {
    if (!is_button_down(BUTTON_ACTION))
    {
      app_state = SHOWING_GAME_MENU;
      show_game_menu(selected_game);
    }

    return;
  }
  
  if (app_state == SHOWING_GAME_MENU)
  {
    if (was_button_pressed(BUTTON_MENU))
    {
      Serial.println("MENU button pressed");
      
      selected_game++;

      if (selected_game >= 2)
      {
          selected_game = 0;
      }

      show_game_menu(selected_game);
    }

    if (was_button_pressed(BUTTON_ACTION))
    {
      Serial.println("ACTION button pressed.");

        if (selected_game == 0)
        {
            init_hit_target_game();
            app_state = RUNNING_HIT_TARGET_GAME;
        }
        else
        {
            app_state = RUNNING_PENDULUM_GAME;
        }
    }

    return;
  }

  if (app_state == RUNNING_HIT_TARGET_GAME)
  {
    update_hit_target_game();

    if (is_hit_target_game_finished())
    {
      clear_neopixel_display();
      show_neopixel_display();
      
      app_state = WAITING_FOR_MENU_RELEASE;
    }

    return;
  }

  if (app_state == RUNNING_PENDULUM_GAME)
  {
    (void)current_time;
    return;
  }
}