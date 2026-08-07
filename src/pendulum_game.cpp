#include "pendulum_game.h"

#include <Arduino.h>
#include "movement.h"
#include "neopixel_display.h"
#include "button_input.h"
#include "oled_display.h"

//DEFINITIONS AND DATATYPES


static unsigned long previous_time = 0;                          //huvudvariabeln för att beräkna när en viss tid gått, ex.vis för att avgöra när LED:ljuset ska "vandra"
static unsigned long led_interval = 250;                         //den tid varje LED ska lysa när den vandrar i spelet

static int current_position = 10;                                 //index för den LED som är tänd; första elementet i arrayen har index 0
static int direction = 1;

static int green_positions[4] = {3, 4, 19, 20};
static int orange_positions[4] = {1, 2, 21, 22};
static int red_positions[4] = {0, 23};
static int pixel_count = 24;                                     //variabel som fylls av värdet i pixel_count i neopixel_display.cpp-modulen

static int end_position_brightness = 0;



//FUNCTIONS

void init_pendulum_game(void)
{
    Serial.println("Pendulum-game was initiated.");
}

static void show_pendulum_playing_field(void)

{
    clear_neopixel_display();

    for (int i = 0; i <=3;i++)
    {
        set_neopixel_color(
            green_positions[i],
            0, 25, 0
        );
    }

    for (int i = 0; i <= 3;i++)
    {
        set_neopixel_color(
            orange_positions[i],
            25, 15, 0
        );
    }

    for (int i = 0; i <= 1;i++)
    {
        set_neopixel_color(
            red_positions[i],
            50, 0, 0
        );
    }


    set_neopixel_color(
      current_position,
      0, 0, 25
    );  

  show_neopixel_display();

}

//MAINLOOP

void update_pendulum_game(void)
{
    unsigned long current_time = millis();

    //LED-VANDRING
    if (current_time - previous_time >= led_interval) //OM angiven tid för den tid en LED ska lysa har passerat...
    {
        previous_time = current_time;                   //uppdatera tid för senaste tändning/släckning
            
        update_pendulum_position(
        &current_position,
        &direction,                            //pekare: jag skickar med adressen till cur.pos och ger därmed movement.cpp lov att ändra variabeln.
        pixel_count
        );

        show_pendulum_playing_field();
    }
    
}
