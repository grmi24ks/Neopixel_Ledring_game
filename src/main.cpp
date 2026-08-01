#include <Arduino.h>

//DEFINITIONS AND DATATYPES

unsigned long animation_start_time = 0;
const unsigned long hit_duration = 700;
unsigned long animation_previous_time = 0;
int animation_blink_count = 0;
bool animation_leds_on = false;

unsigned long previous_time = 0;
unsigned long led_interval = 250;
unsigned long last_debounce_time = 0;
const unsigned long debounce_interval = 30;

int stable_button_state = HIGH;
int previous_button_reading = HIGH; //knapp inte nedtryckt
const int buttonPin = A0;

const int ledPins[] = {D2, D3, D4, D5, D6, D7, D8, D9, D10, D11};
const int ledCount = 10;
int current_position = 0;
int target_position = 7;
int direction = 1;     // 1 betyder rörelse vä->hö, -1 tvärtom

int score = 0;
int lives = 10;

enum GameState //gammal lösning kunde ge running = 1, game_won = 1, game_lost = 1. Här går det inte - spelet kan bara vara i ett läge åt gången
{
  PLAYING,
  SHOWING_HIT,
  SHOWING_MISS,
  SHOWING_WON,
  WON,
  LOST
};

GameState game_state = PLAYING; // skapar en enum-variabel och sätter grundläget till PLAYING;

static void handle_hit(void);
static void handle_miss(void);
static void show_game_lost(void);
static void start_hit_animation(unsigned long current_time);
static void update_hit_animation(unsigned long current_time);
static void start_miss_animation(unsigned long current_time);
static void update_miss_animation(unsigned long current_time);
static void start_won_animation(unsigned long current_time);
static void update_won_animation(unsigned long current_time);


//FUNCTIONS

void setup(void)
{
  Serial.begin(9600);
  
  pinMode(buttonPin, INPUT_PULLUP);  //sätter A0 som ingång och med internt motstånd, sk PULLUP
  
  for (int i = 0; i < ledCount; i++) //smidigt sätt att assign:a LED-lamporna till fysiska OUTPUTS och sätta dem till släckta == LOW 
    {
        pinMode(ledPins[i], OUTPUT);
        digitalWrite(ledPins[i], LOW);
    }

  digitalWrite(ledPins[current_position], HIGH);  //tänd den aktuella LED-lampan

}

static void handle_hit(void)
{
  score = score + 100;
  
  Serial.print("Score: ");
  Serial.println(score);
  Serial.print("Lives remaining: ");
  Serial.println(lives);

  led_interval = led_interval - 50;

  if (led_interval == 0)
  {
    Serial.println("CONGRATULATIONS - YOU WON THE GAME!");
    game_state = WON;
  }
}

static void handle_miss(void)
{
  lives = lives - 1;

  if (lives == 0) 
  {
    Serial.println("-- GAME OVER --");
    game_state = LOST;
  }
  Serial.print("Lives remaining: ");
  Serial.println(lives);
  }


static void show_game_lost(void)
{
  for (int i = 0; i < ledCount; i++)       //öka LED nr så att nästa LED-lampa kan tändas
  {
    digitalWrite(ledPins[i], LOW);
  }
}


static void start_hit_animation(unsigned long current_time)

{
  for (int i = 0; i < ledCount; i++)
  {
    digitalWrite(ledPins[i], HIGH);
    
  }
  
  animation_start_time = current_time;
  game_state = SHOWING_HIT;
}

static void update_hit_animation(unsigned long current_time)
{
  if (current_time - animation_start_time >= hit_duration)
  {
    for (int i = 0; i < ledCount; i++)
    {
      digitalWrite(ledPins[i], LOW);
    }
    digitalWrite(ledPins[current_position], HIGH);  //tänd den aktuella LED-lampan
    game_state = PLAYING;
  }  
}

static void start_miss_animation(unsigned long current_time)
{
  animation_blink_count = 0;
  animation_leds_on = true;
  animation_previous_time = current_time;
  
  for (int i = 0; i < ledCount; i++)
    {
      digitalWrite(ledPins[i], HIGH);
    }
  
  game_state = SHOWING_MISS;
}

static void update_miss_animation(unsigned long current_time)
{
  if (current_time - animation_previous_time >= 100)                //om 100 ms har gått... (vi har en separat timer för animationen)
  {
    animation_previous_time = current_time;                         //spara tiden för den senaste växlingen i MISS-animationen
    animation_leds_on = !animation_leds_on;                              //du måsta byta till motsatsen av dess nuvarande värde - annars sker ingen växling!!

    for (int i = 0; i < ledCount; i++)                         
    {
      digitalWrite(ledPins[i], animation_leds_on);                  //skriva det nya läget till alla LED
    }
    
    if (animation_leds_on == false)                                 //om lamporna nu är släckta
    {
      animation_blink_count = animation_blink_count + 1;                 //öka blinkräknaren
    }   
    if (animation_blink_count >= 3)                                 //tre blinkningar är färdiga
    {
      digitalWrite(ledPins[current_position], HIGH);            //tänd current position igen och byt tillbaka till PLAYING
      previous_time = current_time;                             //synkningen gör att den återställda LED:n får lysa ett helt led_interval innan den flyttar sig igen, annars kanske ett hopp sker
      game_state = PLAYING;                                     //eftersom previous_time fortfarande innehöll tiden från före animationen (som pågår ganska länge)
    }
  }
}


static void start_won_animation(unsigned long current_time)
{
  animation_blink_count = 0;
  animation_leds_on = true;
  animation_previous_time = current_time;
  
  for (int i = 0; i < ledCount; i++)
    {
      digitalWrite(ledPins[i], HIGH);
    }
  
  game_state = SHOWING_WON;
}

static void update_won_animation(unsigned long current_time)
{
 if (current_time - animation_previous_time >= 100)                //om 100 ms har gått... (vi har en separat timer för animationen)
  {
    animation_previous_time = current_time;                         //spara tiden för den senaste växlingen i animationen
    animation_leds_on = !animation_leds_on;                         //du måsta byta till motsatsen av dess nuvarande värde - annars sker ingen växling!!

    for (int i = 0; i < ledCount; i++)                         
    {
      digitalWrite(ledPins[i], animation_leds_on);                  //skriva det nya läget till alla LED
    }
    
    if (animation_leds_on == false)                                 //om lamporna nu är släckta
    {
      animation_blink_count = animation_blink_count + 1;                 //öka blinkräknaren
    }   
    if (animation_blink_count >= 20)                                 //om 20 blinkningar är räknade
    {
      for (int i = 0; i < ledCount; i++)                         
      {
        digitalWrite(ledPins[i], LOW);                  
      }  
      game_state = WON;                               
    }
  }
}


void loop(void)
{
  unsigned long current_time = millis();          //läs aktuell tid från mcu-start med funktionen millis. Max typ 49 dygn. Måste ligga före de två kontrollerna nedan

  if(game_state ==  SHOWING_HIT)                  //har 700 ms gått? har 700 ms gått? 
  {
    update_hit_animation(current_time);
    return;
  }
  
  if(game_state == SHOWING_MISS)
  {
    update_miss_animation(current_time);
    return;
  }

  if(game_state == SHOWING_WON)
  {
    update_won_animation(current_time);
    return;
  }
  
  if (game_state != PLAYING)
  {
    return;                                            //detta avslutar det aktuella loopvarvet direkt. Programmet fortsätter, ser denna state och upplevs avslutad av användaren
  }



    int button_state = digitalRead(buttonPin);        //KNAPP: läs och skriv in råa värdet på knappnedtryckning (HIGH - ej nedtryckt eller LOW - neddtryckt till variabel button_state)
    
    

    if (current_time - previous_time >= led_interval) //OM angiven tid för den tid en LED ska lysa har passerat...
    {
      previous_time = current_time;                   //uppdatera tid för senaste tändning/släckning
      
      digitalWrite(ledPins[current_position], LOW);   //släck current_position-LED     
      
      current_position = current_position + direction;
      
      if (current_position >= ledCount)               // Om positionen passerar sista LED:n, flytta tillbaka ett steg och byt riktning
      {
        current_position = ledCount - 2;
        direction = -1;
      }
      else if (current_position < 0)                  // Om positionen passerar första LED:n, flytta fram ett steg och byt riktning
      {
        current_position = 1;
        direction = 1;
      }
      digitalWrite(ledPins[current_position], HIGH);  //tänd den aktuella LED-lampan//öka LED nr så att nästa LED-lampa kan styras
    }
    

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
              handle_miss();
              
              if (game_state == LOST)
              {
                show_game_lost();
              }
              else
              {
                start_miss_animation(current_time);
              }
              
              
             
            }
        }
      }
    }
    
    previous_button_reading = button_state;       //uppdatera senaste knappavläsning
  
}