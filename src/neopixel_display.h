#ifndef NEOPIXEL_DISPLAY_H
#define NEOPIXEL_DISPLAY_H

void init_neopixel_display(void);

void clear_neopixel_display(void);
void set_neopixel_color(int position, int red, int green, int blue);
void fill_neopixel_display(int red, int green, int blue);
void show_neopixel_display(void);

int get_neopixel_count(void);

#endif