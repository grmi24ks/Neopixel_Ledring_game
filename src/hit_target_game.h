// Include guard hindrar samma header från att läsas in flera gånger.
// Deklarationen berättar att funktionen finns och hur den ska anropas.

#ifndef HIT_TARGET_GAME_H               //if not defined kör nedanstående
#define HIT_TARGET_GAME_H

void init_hit_target_game(void);
void update_hit_target_game(void);
bool is_hit_target_game_finished(void);

#endif
