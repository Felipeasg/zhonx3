/**************************************************************************/
/*!
    @file     tone.h
    @author  BM (PACABOT)
    @date
    @version  0.0
 */
/**************************************************************************/
#ifndef __TONE_H__
#define __TONE_H__

#define c	261
#define d   294
#define e   329
#define f   349
#define g   391
#define gS  415
#define a   440
#define aS  455
#define b   466
#define cH  523
#define cSH 554
#define dH  587
#define dSH 622
#define eH  659
#define fH  698
#define fSH 740
#define gH  784
#define gSH 830
#define aH  880

#define C2 	131
#define C2d 139
#define D2 	147
#define D2d 156
#define E2 	165
#define F2 	175
#define F2d 185
#define G2 	196
#define G2d 208
#define A2 	220
#define A2d 233
#define B2 	247

#define C3 	262
#define C3d 278
#define D3 	294
#define D3d 312
#define E3 	330
#define F3 	350
#define F3d 370
#define G3 	393
#define G3d 416
#define A3 	441
#define A3d 467
#define B3 	494

void tonesplayer(int *note, int *duration, int size, int tempo);
void tone(int note, int duration);
void toneSetVolulme(int volume);
void toneTest(void);

#endif

// double   1 la ronde vaut 16
// Au clair de la lune
//  int music[11]={C3,C3,C3,D3,E3,D3,C3,E3,D3,D3,C3};
//  int duree[11]={2,2,2,2,4,4,2,2,2,2,8};


// Marseillaise
//  int music[18]={G2,G2,G2,C3,C3,D3,D3,G3,E3,C3,C3,E3,C3,A2,F3,D3,B2,D3};
//  int duree[18]={1,2,1,4,4,4,4,7,1,3,1,3,1,4,8,3,1,8}

// Joyeux anniversaire
//  int music[25]={G2,G2,A2,G2,C3,B2,G2,G2,A2,G2,D3,C3,G2,G2,G3,E3,C3,B2,A2,F3,F3,E3,C3,D3,C3};
//  int duree[25]={2,2,4,4,4,8,2,2,4,4,4,8,2,2,4,4,4,4,8,2,2,4,4,4,8};
