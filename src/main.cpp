  #include <Arduino.h>

  //DEFINITIONS AND DATATYPES

  unsigned long previous_time = 0;
  const unsigned long led_interval = 1000;
  unsigned long last_debounce_time = 0;
  const unsigned long debounce_interval = 30;
  int stable_button_state = HIGH;
  int previous_button_reading = HIGH; //knapp inte nedtryckt

  const int ledPins[] = {D2, D3, D4, D5, D6, D7, D8, D9, D10, D11};
  const int ledCount = 10;
  const int buttonPin = A0;
  int current_position = 0;
  int target_position = 4;
  
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

  static void show_miss(void)
  {
    for (int blink = 0; blink < 3; blink++)
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
    digitalWrite(ledPins[current_position], HIGH);  //tänd den aktuella LED-lampan
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
  
  void loop(void)
  {
    unsigned long current_time = millis();  //läs aktuell tid från mcu-start med funktionen millis. Max typ 49 dygn. 

    int button_state = digitalRead(buttonPin);      //KNAPP: läs och skriv in råa värdet på knappnedtryckning (HIGH - ej nedtryckt eller LOW - neddtryckt till variabel button_state)
    
    

    if (current_time - previous_time >= led_interval) //OM angiven tid för den tid en LED ska lysa har passerat...
    {
      previous_time = current_time;                   //uppdatera tid för senaste tändning/släckning
      
      digitalWrite(ledPins[current_position], LOW);   //släck current_position-LED

      current_position++;     
      
      if (current_position >= ledCount)               //OM nuvarande lampa är max antal, börja om på position 0. Dvs. Loop:a ledlampornas tänd/släck i seriell ordning
      {
        current_position = 0;
      }
      digitalWrite(ledPins[current_position], HIGH);  //tänd den aktuella LED-lampan//öka LED nr så att nästa LED-lampa kan styras
    }
    
    
    
    if (button_state != previous_button_reading)   //KNAPP: om knappförändring skett sedan förra loopvarvet
    {
      last_debounce_time = current_time;
    } 
    
    if (current_time - last_debounce_time >= debounce_interval)
    {
      if (button_state != stable_button_state)
      {
        stable_button_state = button_state;
        if (stable_button_state == LOW)
        {
            if (current_position == target_position)
            {
              Serial.println("HIT!");
              show_hit();
            }
            else
            {
              Serial.println("MISS!");
              show_miss();
            }
        }
      }
    }
    
    previous_button_reading = button_state;       //uppdatera knappläge
   
}