#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "movement.h"

#define DATA_PIN 5
#define PIXEL_COUNT 24

//DEFINITIONS AND DATATYPES

unsigned long animation_start_time = 0;                   //används för att beräkna när LED:s ska släckas vid träffar
const unsigned long hit_duration = 700;                   //dito
unsigned long animation_previous_time = 0;                //används för att beräkna när blink:s ska sluta vid missar och vinst
int animation_blink_count = 0;                            //räknar antal blink
bool animation_leds_on = false;                           //värdet som jag använder för att växla med: animation_leds_on/!animation_leds_on

unsigned long target_pulse_previous_time = 0;
const unsigned long target_pulse_interval = 5;

int target_brightness = 0;
int target_brightness_direction = 2;

const unsigned long start_led_interval = 250;
const int start_lives = 5;
const int start_position = 0;


unsigned long previous_time = 0;                          //huvudvariabeln för att beräkna när en viss tid gått, ex.vis för att avgöra när LED:ljuset ska "vandra"
unsigned long led_interval = 250;                         //den tid varje LED ska lysa när den vandrar i spelet
unsigned long last_debounce_time = 0;                     //används för att beräkna tiden sedan den råa knappavläsningen senast ändrades
const unsigned long debounce_interval = 30;               //knappvärdet måste vara oförändrat i minst 30 ms samt vara != stable_button_state innan det godkänns

int stable_button_state = HIGH;                           //utgångsläge är knapp ej nedtryckt = HIGH
int previous_button_reading = HIGH;                       //knapp inte nedtryckt
bool restart_armed = false;                               //restart "laddad" men körs först när knapptryckning godkänts (oförändr. >30ms och != stable_button_state)
const int buttonPin = A0;

int current_position = 0;                                 //index för den LED som är tänd; första elementet i arrayen har index 0
int target_position = 19;
int direction = 1;                                        //1 betyder rörelse vä->hö, -1 tvärtom

int score = 0;
int lives = 5;

Adafruit_NeoPixel ring(
    PIXEL_COUNT,
    DATA_PIN,
    NEO_GRB + NEO_KHZ800
);


enum GameState //gammal lösning kunde ge running = 1, game_won = 1, game_lost = 1. Här går det inte - spelet kan bara vara i ett läge åt gången
{
  PLAYING,
  SHOWING_HIT,
  SHOWING_MISS,
  SHOWING_WON,
  SHOWING_LOST,
  WON,
  LOST
};

GameState game_state = PLAYING; // skapar en enum-variabel och sätter grundläget till PLAYING;

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

static void reset_game(unsigned long current_time);



//FUNCTIONS

void setup(void)
{
  Serial.begin(9600);
  
  pinMode(buttonPin, INPUT_PULLUP);  //sätter A0 som ingång och med internt motstånd, sk PULLUP vilket knappen behöver 
  
  ring.begin();
  show_playing_field();              //tänder upp target_position och current_position i LED-varvet
}
static void show_playing_field(void)
{
  ring.clear();

  ring.setPixelColor(
      target_position,
      ring.Color(
        target_brightness, 
        target_brightness * 3 / 5, 
        0
      )
  );

  ring.setPixelColor(
      current_position,
      ring.Color(0, 0, 25)
  );

  ring.show();

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
      target_position = random(PIXEL_COUNT);  //byt target_position slumpmässigt för varje HIT
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

//ANIMATION-FUNCTIONS

static void start_hit_animation(unsigned long current_time)

{
  for (int i = 0; i < PIXEL_COUNT; i++)
  { 
    ring.setPixelColor(                                         //buffra pixlar, färg och ljusstyrka (här för samtliga pixlar)
        i, 
        ring.Color(0, 25, 0)
    );
  }
  ring.show();                                                 //visa vad som ligger i bufferten

  animation_start_time = current_time;                         //uppdatera animation_start_time för att nedan kunna beräkna och jämföra med hit_duration
  game_state = SHOWING_HIT;
}



static void update_hit_animation(unsigned long current_time)
{
  if (current_time - animation_start_time >= hit_duration)      //om 700 ms har passerat...
  {
    show_playing_field();  
    
    previous_time = current_time;                               //spelpixeln får ett helt intervall innan den flyttas igen
    
    game_state = PLAYING;                                        //återgå till huvudlooop:en med dess LED-vandring och knappavläsning
  }  
}

static void start_miss_animation(unsigned long current_time)
{
  animation_blink_count = 0;
  animation_leds_on = true;
  animation_previous_time = current_time;
  
  for (int i = 0; i < PIXEL_COUNT; i++)
  {
    ring.setPixelColor(                                          
      i, 
      ring.Color(25, 0, 0)
    );
                                                  
  }
  
  ring.show();    

  game_state = SHOWING_MISS;                                        //detta leder oss via huvudloopen till update_miss_animation nedan
}

static void update_miss_animation(unsigned long current_time)
{
  if (current_time - animation_previous_time >= 75)                //om 75 ms har gått... (vi har en separat timer för MISS-animationen)
  {
    animation_previous_time = current_time;                         //spara tiden för den senaste växlingen i MISS-animationen
    animation_leds_on = !animation_leds_on;                         //du måste byta till motsatsen av dess nuvarande värde - annars sker ingen växling!!

    ring.clear();

    if (animation_leds_on == true)
    {
      for (int i = 0; i < PIXEL_COUNT; i++)                         
      {
        ring.setPixelColor(                                         
          i, 
          ring.Color(25, 0, 0)
        );                
      }
    }                             
    
    ring.show();                                                    //tänd alla lampor
    
    if (animation_leds_on == false)                                 //om lamporna nu är släckta
    {
      animation_blink_count = animation_blink_count + 1;            //öka blinkräknaren
    }   
    if (animation_blink_count >= 3)                                 //tre blinkningar är färdiga
    {
      show_playing_field();  

      previous_time = current_time;

      game_state = PLAYING;                                     
    }
  }
}


static void start_won_animation(unsigned long current_time)
{
  animation_blink_count = 0;
  animation_leds_on = true;
  animation_previous_time = current_time;
  
  for (int i = 0; i < PIXEL_COUNT; i++)                         
  {
    ring.setPixelColor(                                           
        i, 
        ring.Color(0, 25, 0)
    );                
  }

  ring.show();
    
  game_state = SHOWING_WON;
}

static void update_won_animation(unsigned long current_time)
{
 if (current_time - animation_previous_time >= 75)                //om 75 ms har gått... (vi har en separat timer för MISS-animationen)
  {
    animation_previous_time = current_time;                         //spara tiden för den senaste växlingen i MISS-animationen
    animation_leds_on = !animation_leds_on;                         //du måste byta till motsatsen av dess nuvarande värde - annars sker ingen växling!!

    ring.clear();

    if (animation_leds_on == true)
    {
      for (int i = 0; i < PIXEL_COUNT; i++)                         
      {
        ring.setPixelColor(                                         
            i, 
            ring.Color(0, 25, 0)
        );                
      }
    }                             
    
    ring.show();                                                    
    
    if (animation_leds_on == false)                                 //om lamporna nu är släckta
    {
      animation_blink_count = animation_blink_count + 1;            //öka blinkräknaren
    }   
    if (animation_blink_count >= 20)                                 //20 blinkningar är färdiga
    {
      ring.clear();
      
      ring.show();                                                 
                                                        
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

  ring.fill(ring.Color(25, 0, 0));
  ring.show();

  game_state = SHOWING_LOST;
}

static void update_lost_animation(unsigned long current_time)
{
  if (current_time - animation_previous_time >= 200)                //om 200 ms har gått... (vi har en separat animationtimer)
  {
    animation_previous_time = current_time;                         //spara tiden för den senaste växlingen i animationen
    animation_leds_on = !animation_leds_on;                         //du måste byta till motsatsen av dess nuvarande värde - annars sker ingen växling!!

    ring.clear();

    if (animation_leds_on == true)
    {
      for (int i = 0; i < PIXEL_COUNT; i++)                         
      {
        ring.setPixelColor(                                         
            i, 
            ring.Color(25, 0, 0)
        );                
      }
    }                             
    
    ring.show();                                                    
    
    if (animation_leds_on == false)                                 //om lamporna nu är släckta
    {
      animation_blink_count = animation_blink_count + 1;            //öka blinkräknaren
    }   
    if (animation_blink_count >= 10)                                 //10 blinkningar är färdiga
    {
      ring.clear();
      ring.show();

      game_state = LOST;  
    }
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

  game_state = PLAYING;
}


//HUVUDLOOP

void loop(void)
{
  //current_time deklarationen måste ligga före kontrollerna nedan    
  unsigned long current_time = millis();                      //läs aktuell tid från mcu-start med funktionen millis. Slår om efter typ 49 dygn, tidsdiff fungerar eftersom unsigned long används.  

  if (game_state == WON || game_state == LOST)
  {
    int button_state = digitalRead(buttonPin);                 //rå knappavläsning
    
    if (button_state != previous_button_reading)                   //KNAPP: om råvärdet skiljer sig från förra gången, till exempel användaren släppte upp knappen efter WON/LOST
    {
      last_debounce_time = current_time;                       //starta om debounce-timern
    }  
    
    if (current_time - last_debounce_time >= debounce_interval) //om längre tid än 30 ms gått...så är knappförändringen stabil
    {
      if (button_state != stable_button_state)
      {
        stable_button_state = button_state;

        if (stable_button_state == HIGH)                         //om stabil avläsningen är HIGH - ej nedtryckt, armeras omstart
        {
        restart_armed = true;
        }
        
        else if (stable_button_state == LOW && restart_armed)
        {
          reset_game(current_time);
          restart_armed = false;
        }
      }
    }
    previous_button_reading = button_state;                     //annars kan nästa loopvarv inte upptäcka en ny rå förändring korrekt
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


  int button_state = digitalRead(buttonPin);        //KNAPP: läs och skriv in råa värdet på knappnedtryckning (HIGH - ej nedtryckt eller LOW - nedtryckt till variabel button_state)
  
  //LED-VANDRINGSLOGIK
  if (current_time - previous_time >= led_interval) //OM angiven tid för den tid en LED ska lysa har passerat...
  {
    previous_time = current_time;                   //uppdatera tid för senaste tändning/släckning
        
    update_circular_position(
      &current_position,
      PIXEL_COUNT
    );

    show_playing_field();

  }
  

  //KNAPP-LOGIK
  if (button_state != previous_button_reading)      //KNAPP: om knappförändring skett sedan förra loopvarvet
  {
    last_debounce_time = current_time;
  } 
  
  if (current_time - last_debounce_time >= debounce_interval) //om längre tid än 30 ms gått...
  {
    if (button_state != stable_button_state)                   //om vi dessutom ser ett knappskifte - konstatera att det ägde rum och...
    {
      stable_button_state = button_state;                      //...uppdatera knappläget
      if (stable_button_state == LOW)                          //om det nya knappläget är nedtryckt = LOW
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
  }
  
  previous_button_reading = button_state;       //uppdatera senaste knappavläsning
  
}