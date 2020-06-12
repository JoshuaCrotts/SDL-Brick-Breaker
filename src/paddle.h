#ifndef PADDLE_H
#define PADDLE_H

#include "commons.h"

extern Entity* paddle;
extern App app;
extern Stage stage;

extern void init_paddle(void);
extern void paddle_update(void);
extern void paddle_draw(void);
extern void paddle_die(void);

extern void drawRect(SDL_Rect*, uint8_t, uint8_t, uint8_t, uint8_t, bool);

#endif