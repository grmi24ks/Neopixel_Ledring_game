#ifndef BUTTON_INPUT_H
#define BUTTON_INPUT_H

#include <Arduino.h>

enum ButtonId                                   //skapar en enum-datatyp...
{
    BUTTON_ACTION,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_MENU,
    BUTTON_COUNT                                //är ej en knapp utan anger antal knapplatser och används senare för att ange arraystorlek
};

void init_button_input(ButtonId, int pin);      //och så här anropar man den 

void update_button_input(unsigned long current_time);

bool was_button_pressed(ButtonId button);

bool was_button_released(ButtonId button);

bool is_button_down(ButtonId button);

#endif