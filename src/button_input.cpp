#include <Arduino.h>
#include "button_input.h"

struct ButtonState
{
    int pin;
    int stable_state;
    int  previous_reading;
    unsigned long last_debounce_time;
    bool    initialized;
    bool    pressed_event;
    bool    released_event;
};

static const unsigned long debounce_interval = 30;

static ButtonState buttons[BUTTON_COUNT];

void init_button_input(ButtonId button, int pin)
{
    buttons[button].pin = pin;
    buttons[button].stable_state = HIGH;
    buttons[button].previous_reading = HIGH;
    buttons[button].last_debounce_time = 0;
    buttons[button].initialized = true;
    buttons[button].pressed_event = false;
    buttons[button].released_event = false;
    
    pinMode(pin, INPUT_PULLUP);
}

void update_button_input(unsigned long current_time)
{
  for (int i = 0; i < BUTTON_COUNT; i++)
  {
    if (buttons[i].initialized == false)
    {
      continue;
    }

    buttons[i].pressed_event = false;
    buttons[i].released_event = false;

    int button_reading = digitalRead(buttons[i].pin);

    if (button_reading != buttons[i].previous_reading)
    {
      buttons[i].last_debounce_time = current_time;
    }

    if (current_time - buttons[i].last_debounce_time >= debounce_interval)
    {
      if (button_reading != buttons[i].stable_state)
      {
        buttons[i].stable_state = button_reading;

        if (buttons[i].stable_state == LOW)
        {
          buttons[i].pressed_event = true;
        }
        else
        {
          buttons[i].released_event = true;
        }
      }
    }

    buttons[i].previous_reading = button_reading;
  }
}

bool was_button_pressed(ButtonId button)
{
    return buttons[button].pressed_event;
}

bool was_button_released(ButtonId button)
{
    return buttons[button].released_event;
}

bool is_button_down(ButtonId button)
{
    return buttons[button].stable_state == LOW;   //true om knapp är nedtryckt (LOW), false otherwise
}