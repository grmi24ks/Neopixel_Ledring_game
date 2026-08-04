#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define DATA_PIN 5
#define PIXEL_COUNT 24

unsigned long previous_time = 0;                          //huvudvariabeln för att beräkna när en viss tid gått, ex.vis för att avgöra när LED:ljuset ska "vandra"
unsigned long led_interval = 250;

int direction = 1;                                        //1 betyder rörelse vä->hö, -1 tvärtom
int current_position = 0;                                 //index för den LED som är tänd; första elementet i arrayen har index 0

Adafruit_NeoPixel ring(
    PIXEL_COUNT,
    DATA_PIN,
    NEO_GRB + NEO_KHZ800
);

void setup(void)
{
    ring.begin();
    ring.clear();
    ring.show();
}

void loop(void)
{
    unsigned long current_time = millis();

    
    if (current_time - previous_time >= led_interval) //OM angiven tid för den tid en LED ska lysa har passerat...
    {
        previous_time = current_time;                   //uppdatera tid för senaste tändning/släckning
        
            
        
        ring.clear();                                   //släck LED   

        current_position = current_position + direction;
        
        if (current_position >= PIXEL_COUNT)               // Om positionen passerar sista LED:n, flytta tillbaka ett steg och byt riktning
        {
        current_position = PIXEL_COUNT - 2;
        direction = -1;
        }
        else if (current_position < 0)                  // Om positionen passerar första LED:n, flytta fram ett steg och byt riktning
        {
        current_position = 1;
        direction = 1;
        }

        ring.setPixelColor(                             //tänd LED-lampan på den nya positionen
            current_position, 
            ring.Color(0, 0, 25)
        );  
   
        ring.show();
    }
    
}