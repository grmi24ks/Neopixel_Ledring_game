#include <Arduino.h>

//DEFINITIONS AND DATATYPES

int running = 1;
unsigned long previous_time = 0;
unsigned long led_interval = 100;
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
int game_won = 0;
int game_lost = 0;

static void handle_hit(void);
static void handle_miss(void);
static void show_game_won(void);
static void show_game_lost(void);
static void show_hit(void);
static void show_miss(void);

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

  led_interval = led_interval - 10;

  if (led_interval == 0)
  {
    Serial.println("CONGRATULATIONS - YOU WON THE GAME!");
    game_won = 1;
    running = 0;
  }
}

static void handle_miss(void)
{
  lives = lives - 1;

  if (lives == 0) 
  {
    Serial.println("-- GAME OVER --");
    game_lost = 1;
    running = 0;
  }
  Serial.print("Lives remaining: ");
  Serial.println(lives);
  }

static void show_game_won(void)
{
  for (int blink = 0; blink <= 19; blink++)  //loop:a genom nedanstående 20 ggr
  {
    for (int i = 0; i < ledCount; i++)       //öka LED nr så att nästa LED-lampa kan tändas
    {
      digitalWrite(ledPins[i], HIGH);
    }
    
    delay(100);

    for (int i = 0; i < ledCount; i++)
    {
      digitalWrite(ledPins[i], LOW);
    
    }
    delay(100);
    
  }
}

static void show_game_lost(void)
{
  for (int i = 0; i < ledCount; i++)       //öka LED nr så att nästa LED-lampa kan tändas
  {
    digitalWrite(ledPins[i], LOW);
  }
}


static void show_hit(void)
{
  for (int i = 0; i < ledCount; i++)
  {
    digitalWrite(ledPins[i], HIGH);
    
  }
  
  delay(700);

  for (int i = 0; i < ledCount; i++)
  {
    digitalWrite(ledPins[i], LOW);
  }
  digitalWrite(ledPins[current_position], HIGH);  //tänd den aktuella LED-lampan
}
  
static void show_miss(void)
{
  for (int blink = 0; blink < 3; blink++)
  {
    for (int i = 0; i < ledCount; i++)       //gå igenom alla LED-lampor
    {
      digitalWrite(ledPins[i], HIGH);
      
    }
    
    delay(100);

    for (int i = 0; i < ledCount; i++)
    {
      digitalWrite(ledPins[i], LOW);
    
    }
    delay(100);
    
  }
  digitalWrite(ledPins[current_position], HIGH);  //tänd den aktuella LED-lampan
}


void loop(void)
{
  if (!running)
  {
    return;                                        //detta avslutar det aktuella loopvarvet direkt!
  }

  unsigned long current_time = millis();          //läs aktuell tid från mcu-start med funktionen millis. Max typ 49 dygn. 

    int button_state = digitalRead(buttonPin);      //KNAPP: läs och skriv in råa värdet på knappnedtryckning (HIGH - ej nedtryckt eller LOW - neddtryckt till variabel button_state)
    
    

    if (current_time - previous_time >= led_interval) //OM angiven tid för den tid en LED ska lysa har passerat...
    {
      previous_time = current_time;                   //uppdatera tid för senaste tändning/släckning
      
      digitalWrite(ledPins[current_position], LOW);   //släck current_position-LED     
      
      current_position = current_position + direction;
      
      if (current_position >= ledCount)  // Om positionen passerar sista LED:n, flytta tillbaka ett steg och byt riktning
      {
        current_position = ledCount - 2;
        direction = -1;
      }
      else if (current_position < 0)     // Om positionen passerar första LED:n, flytta fram ett steg och byt riktning
      {
        current_position = 1;
        direction = 1;
      }
      digitalWrite(ledPins[current_position], HIGH);  //tänd den aktuella LED-lampan//öka LED nr så att nästa LED-lampa kan styras
    }
    
    if (button_state != previous_button_reading)   //KNAPP: om knappförändring skett sedan förra loopvarvet
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
              
              if (game_won == 1)
              {
                show_game_won();
              }
              else
              {
              show_hit();
              }
              
            }
            else
            {
              Serial.println("MISS!");
              handle_miss();
              
              if (game_lost == 1)
              {
                show_game_lost();
              }
              else
              {
                show_miss();
              }
              
              
             
            }
        }
      }
    }
    
    previous_button_reading = button_state;       //uppdatera senaste knappavläsning
  
}