  #include <Arduino.h>

  //DEFINITIONS AND DATATYPES

  const int ledPins[] = {D2, D3, D4, D5, D6, D7, D8, D9, D10, D11};
  const int ledCount = 10;
  const int buttonPin = A0;
  int current_position = 0;
  int target_position = 5;

  //FUNCTIONS

  void setup(void)
  {
    Serial.begin(9600);
    
    pinMode(buttonPin, INPUT_PULLUP);  
    
    for (int i = 0; i < ledCount; i++)
      {
          pinMode(ledPins[i], OUTPUT);
          digitalWrite(ledPins[i], LOW);
      }
  }

  void loop(void)
  {
    digitalWrite(ledPins[current_position], HIGH);
    
    int button_state = digitalRead(buttonPin);
    
    if (button_state == 0)
    {
      if (current_position == target_position)
      {
        Serial.println("HIT!");
      }
      else
      {
        Serial.println("MISS!");
      }
    }
    
    
    delay(500);

    digitalWrite(ledPins[current_position], LOW);

    current_position++;

    if (current_position >= ledCount) 
    {
      current_position = 0;
    }
    
    Serial.println(digitalRead(buttonPin));
    delay(100);
  }