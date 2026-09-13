#ifndef __DINO_H__
#define __DINO_H__

#include <stdint.h>

void Show_Score(void);
void Show_Ground(void);
void Show_Barrier(void);
void Show_Cloud(void);
void Show_Dino(void);
int IsColliding(struct Object_Position *a, struct Object_Position *b);
int DinoGame_Animation(void);
void Dino_Tick(void);
void DinoGame_Pos_Init(void);

#endif