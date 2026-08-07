#ifndef OLED_DISPLAY_H               //if not defined kör nedanstående
#define OLED_DISPLAY_H

bool init_oled_display(void);
void show_oled_test_screen(void);
void show_game_menu(int selected_game);

void oled_show_board(int lives, int score);
void oled_show_miss(void);
void oled_show_hit(void);
void oled_showing_won(void);
void oled_showing_lost(void);

#endif