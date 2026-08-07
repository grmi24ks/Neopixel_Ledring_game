#include "hit_target_game.h"

#include <Arduino.h>
#include "movement.h"
#include "hit_target_game.h"
#include "neopixel_display.h"
#include "button_input.h"
#include "oled_display.h"

//DEFINITIONS AND DATATYPES
//static skyddar modulens interna tillstånd så att andra moduler kan anropa variabler/funktioner med samma namn utan att dessa påverkas

static int pixel_count = 0;                                     //variabel som fylls av värdet i pixel_count i neopixel_display.cpp-modulen

static unsigned long animation_start_time = 0;                   //används för att beräkna när LED:s ska släckas vid träffar
static const unsigned long hit_duration = 700;                   //dito
static unsigned long animation_previous_time = 0;                //används för att beräkna när blink:s ska sluta vid missar och vinst
static int animation_blink_count = 0;                            //räknar antal blink
static bool animation_leds_on = false;                           //värdet som jag använder för att växla med: animation_leds_on/!animation_leds_on

static unsigned long target_pulse_previous_time = 0;
static const unsigned long target_pulse_interval = 15;

static int target_brightness = 0;
static int target_brightness_direction = 2;

static const unsigned long start_led_interval = 250;
static const int start_lives = 5;
static const int start_position = 0;


static unsigned long previous_time = 0;                          //huvudvariabeln för att beräkna när en viss tid gått, ex.vis för att avgöra när LED:ljuset ska "vandra"
static unsigned long led_interval = 250;                         //den tid varje LED ska lysa när den vandrar i spelet

static int current_position = 0;                                 //index för den LED som är tänd; första elementet i arrayen har index 0
static int target_position = 19;
static int direction = 1;                                        //1 betyder rörelse vä->hö, -1 tvärtom

static int score = 0;
static int lives = 5;

enum GameState //game_selector känner inte till denna typ - den behöver bara fråga "is hit target game finished"
{
  PLAYING,
  SHOWING_HIT,
  SHOWING_MISS,
  SHOWING_WON,
  SHOWING_LOST,
  WON,
  LOST
};

static GameState game_state = PLAYING; // skapar en enum-variabel och sätter grundläget till PLAYING;

static void show_playing_field();
static void handle_hit(void);
static void handle_miss(unsigned long current_time);

static void update_target_pulse(unsigned long current_time);                  

static void start_hit_animation(unsigned long current_time);
static void update_hit_animation(unsigned long current_time);
static void start_miss_animation(unsigned long current_time);
static void update_miss_animation(unsigned long current_time);
static void start_won_animation(unsigned long current_time);
static void update_won_animation(unsigned long current_time);
static void start_lost_animation(unsigned long current_time);
static void update_lost_animation(unsigned long current_time);

static void return_to_PLAYING(int current_time);
static void reset_game(unsigned long current_time);

bool is_hit_target_game_finished(void);




//FUNCTIONS


void init_hit_target_game(void)
{
  pixel_count = get_neopixel_count();

  reset_game(millis());         //återställer poäng, liv, position, hastighet, state och visar spelplanen
}

static void show_playing_field(void)

{
  clear_neopixel_display();

  set_neopixel_color(
      target_position,
      target_brightness, 
      target_brightness * 3 / 5, 
      0
  );

  set_neopixel_color(
      current_position,
      0, 0, 25
  );

  show_neopixel_display();

}

static void handle_hit(void)
{
  int previous_target_position = target_position;
  
  score = score + 100;
  
  Serial.print("Score: ");
  Serial.println(score);
  Serial.print("Lives remaining: ");
  Serial.println(lives);
  
  if (led_interval <= 50)
  {
    led_interval = 0;
    Serial.println("CONGRATULATIONS - YOU WON THE GAME!");
    game_state = WON;
  } 
  else
  {
    led_interval -=50;                    //LED-vandringen blir snabbare och snabbare för varje HIT
    do 
    {
      target_position = random(pixel_count);  //byt target_position slumpmässigt för varje HIT
    }  
    while (target_position == previous_target_position);

    Serial.print("New target: ");
    Serial.println(target_position);
  }
}

static void handle_miss(unsigned long current_time)
{
  lives = lives - 1;

  if (lives == 0) 
  {
    Serial.println("-- GAME OVER --");
    
    start_lost_animation(current_time);
  }
  Serial.print("Lives remaining: ");
  Serial.println(lives);
  }


//ANIMATION-FUNCTIONS

static void start_hit_animation(unsigned long current_time)

{
  for (int i = 0; i < pixel_count; i++)
  { 
    set_neopixel_color(i, 0, 25, 0);    
  }
  show_neopixel_display();                                                 //visa vad som ligger i bufferten

  animation_start_time = current_time;                          //uppdatera animation_start_time för att nedan kunna beräkna och jämföra med hit_duration
  oled_show_hit();                        
  game_state = SHOWING_HIT;
}


static void update_hit_animation(unsigned long current_time)
{
  if (current_time - animation_start_time >= hit_duration)      //om 700 ms har passerat...
  {
    return_to_PLAYING(current_time);                                
  }  
}

static void start_miss_animation(unsigned long current_time)
{
  animation_blink_count = 0;
  animation_leds_on = true;
  animation_previous_time = current_time;
  
  for (int i = 0; i < pixel_count; i++)
  {
    set_neopixel_color(i, 25, 0, 0);                                              
  }
  
  show_neopixel_display();     
  oled_show_miss();
  game_state = SHOWING_MISS;                                        //detta leder oss via huvudloopen till update_miss_animation nedan
}

static void update_miss_animation(unsigned long current_time)
{
  if (current_time - animation_previous_time >= 75)                //om 75 ms har gått... (vi har en separat timer för MISS-animationen)
  {
    animation_previous_time = current_time;                         //spara tiden för den senaste växlingen i MISS-animationen
    animation_leds_on = !animation_leds_on;                         //du måste byta till motsatsen av dess nuvarande värde - annars sker ingen växling!!

    clear_neopixel_display(); 

    if (animation_leds_on == true)
    {
      for (int i = 0; i < pixel_count; i++)                         
      {
        set_neopixel_color(i, 15, 0, 0);                
      }
    }                             
    
    show_neopixel_display();                                                    //tänd alla lampor
    
    if (animation_leds_on == false)                                 //om lamporna nu är släckta
    {
      animation_blink_count = animation_blink_count + 1;            //öka blinkräknaren
    }   
    
    if (animation_blink_count >= 3)                                 //tre blinkningar är färdiga
    {
      return_to_PLAYING(current_time);                            
    }
  }
}


static void start_won_animation(unsigned long current_time)
{
  animation_blink_count = 0;
  animation_leds_on = true;
  animation_previous_time = current_time;
  
  for (int i = 0; i < pixel_count; i++)                         
  {
   set_neopixel_color(i, 0, 25, 0);                  
  }

  show_neopixel_display(); 
  oled_showing_won();  
  game_state = SHOWING_WON;
}

static void update_won_animation(unsigned long current_time)
{
 if (current_time - animation_previous_time >= 75)                //om 75 ms har gått... (vi har en separat timer för MISS-animationen)
  {
    animation_previous_time = current_time;                         //spara tiden för den senaste växlingen i MISS-animationen
    animation_leds_on = !animation_leds_on;                         //du måste byta till motsatsen av dess nuvarande värde - annars sker ingen växling!!

    clear_neopixel_display(); 

    if (animation_leds_on == true)
    {
      for (int i = 0; i < pixel_count; i++)                         
      {
        set_neopixel_color(i, 0, 25, 0);                   
      }
    }                             
    
    show_neopixel_display();                                                    
    
    if (animation_leds_on == false)                                 //om lamporna nu är släckta
    {
      animation_blink_count = animation_blink_count + 1;            //öka blinkräknaren
    }   
    if (animation_blink_count >= 20)                                 //20 blinkningar är färdiga
    {
      clear_neopixel_display(); 
      
      show_neopixel_display();                                                 
                                                        
      previous_time = current_time;                                 //återställ huvud-timer:n så att den aktuella LED-lampan får lysa ett helt led_interval efter miss-animationen.
      game_state = WON;                                     
    }
  }
}

static void start_lost_animation(unsigned long current_time)
{
  animation_blink_count = 0;
  animation_leds_on = true;
  animation_previous_time = current_time;

  fill_neopixel_display(25, 0, 0);
  show_neopixel_display(); 

  oled_showing_lost();
  game_state = SHOWING_LOST;
}

static void update_lost_animation(unsigned long current_time)
{
  if (current_time - animation_previous_time >= 200)                //om 200 ms har gått... (vi har en separat animationtimer)
  {
    animation_previous_time = current_time;                         //spara tiden för den senaste växlingen i animationen
    animation_leds_on = !animation_leds_on;                         //du måste byta till motsatsen av dess nuvarande värde - annars sker ingen växling!!

    clear_neopixel_display(); 

    if (animation_leds_on == true)
    {
      for (int i = 0; i < pixel_count; i++)                         
      {
        set_neopixel_color(i, 25, 0, 0);                
      }
    }                             
    
    show_neopixel_display();                                                 
    
    if (animation_leds_on == false)                                 //om lamporna nu är släckta
    {
      animation_blink_count = animation_blink_count + 1;            //öka blinkräknaren
    }   
    if (animation_blink_count >= 10)                                 //10 blinkningar är färdiga
    {
      clear_neopixel_display(); 
      show_neopixel_display(); 

      game_state = LOST;  
    }
  }
}


static void update_target_pulse(unsigned long current_time)
{
  if (current_time - target_pulse_previous_time >= target_pulse_interval)
  {
    target_pulse_previous_time = current_time;

    target_brightness += target_brightness_direction;

    if (target_brightness >=100)
    {
      target_brightness = 100;
      target_brightness_direction = -2;
    }

    if (target_brightness <=0)
    {
      target_brightness = 0;
      target_brightness_direction = 2;
    }
    show_playing_field();
  }
}
static void reset_game(unsigned long current_time)
{
  led_interval = start_led_interval;
  lives = start_lives;
  score = 0;
  current_position = start_position;
  direction = 1;
  
  previous_time = current_time;

  target_brightness = 0;
  target_brightness_direction = 2;
  target_pulse_previous_time = current_time;

  show_playing_field(); 
  oled_show_board(lives, score); 

  game_state = PLAYING;
}

static void return_to_PLAYING(int current_time)
{
  previous_time = current_time;                               //spelpixeln får ett helt intervall innan den flyttas igen
  oled_show_board(lives, score); 
  show_playing_field();
  game_state = PLAYING; 
}

bool is_hit_target_game_finished(void)
{
  return game_state == WON || game_state == LOST;           //Returnera true om minst ett av villkoren är sant
}


//HUVUDLOOP

void update_hit_target_game(void)
{
  //current_time deklarationen måste ligga före kontrollerna nedan    
  unsigned long current_time = millis();                       
  
  if (game_state == WON || game_state == LOST)
  {
    return;
  }
 
  if (game_state ==  SHOWING_HIT)                  //kör träffanimationen (som frågar har 700 ms gått) och hoppa över resten av huvudloop:en.
  {
    update_hit_animation(current_time);
    return;
  }
  
  if (game_state == SHOWING_MISS)
  {
    update_miss_animation(current_time);
    return;
  }

  if (game_state == SHOWING_WON)
  {
    update_won_animation(current_time);
    return;
  }
  
  if (game_state == SHOWING_LOST)
  {
    update_lost_animation(current_time);
    return;
  }

  if (game_state != PLAYING)
  {
    return;                                            //detta avslutar det aktuella loopvarvet direkt. Programmet fortsätter, ser denna state varje loop-varv och upplevs avslutad av användaren
  }

  update_target_pulse(current_time);

  //LED-VANDRING
  if (current_time - previous_time >= led_interval) //OM angiven tid för den tid en LED ska lysa har passerat...
  {
    previous_time = current_time;                   //uppdatera tid för senaste tändning/släckning
        
    update_circular_position(
      &current_position,                            //pekare: jag skickar med adressen till cur.pos och ger därmed movement.cpp lov att ändra variabeln.
      pixel_count
    );

    show_playing_field();                           //uppdateras alltså varje loopvarv, hela tiden och jättesnabbt

  }
  

  //KNAPPAVLÄSNING
  if (was_button_pressed(BUTTON_ACTION))                         //returnerar "trycktes knappen ned"
    {
        if (current_position == target_position)             //OCH om knappen tryckts ned då den aktuella lampan motsvarar mål-lampan
        {
            Serial.println("HIT!");                            //HIT!
            handle_hit();
            
            if (game_state == WON)
            {
            start_won_animation(current_time);
            }
            else
            {
            start_hit_animation(current_time);                
            }
            
        }
        else
        {
            Serial.println("MISS!");
            handle_miss(current_time);
            
            if (game_state != SHOWING_LOST)
            {
            start_miss_animation(current_time);
            }
        }
    }
  
}