/*
 * Copyright © 1987 Bart Massey
 * [This program is licensed under the "MIT License"]
 * Please see the file COPYING in the source
 * distribution of this software for license terms.
 */

/*
 * This program plays the game of Tic-Tac-Toe.
 * It reads an integer move from stdin, then
 * redisplays the board on stdout.
 * The "-o" flag lets the computer play "x",
 * and thus move first.
 */

#include <stdlib.h>
#include <stdio.h>
#include "ttt.h"
 
enum who humanside = WHO_X;
enum who computerside = WHO_O;
struct board currentboard;
struct board newboard;
int compfirst;
int won;

enum win whotowin(enum who w) {
  switch (w) {
  case WHO_X:
    return WIN_X;
  case WHO_O:
    return WIN_O;
  case WHO_B:
    break;
  }
  abort();
  /*NOTREACHED*/
}

static enum who whoother(enum who w) {
  switch (w) {
  case WHO_X:
    return WHO_O;
  case WHO_O:
    return WHO_X;
  case WHO_B:
    break;
  }
  abort();
  /*NOTREACHED*/
}

enum win referee(struct board *b, int misere) {
  int i, j, blanks;
  enum who w;
  enum win victor;

  w = b->movewho;
  victor = whotowin(w);
  if (misere)
      victor = whotowin(whoother(w));
  blanks = 0;
  /* row */
  i = b->movex;
  for (j = 0; j < 3; j++)
    if (b->cell[i][j] != w)
      break;
  if (j >= 3)
    return victor;
  /* col */
  j = b->movey;
  for (i = 0; i < 3; i++)
    if (b->cell[i][j] != w)
      break;
  if (i >= 3)
    return victor;
  /* main diagonal */
  if (b->movex == b->movey) {
    for (i = 0; i < 3; i++)
      if (b->cell[i][i] != w)
      	break;
    if (i >= 3)
      return victor;
  }
  /* off diagonal */
  if (b->movex == 2 - b->movey) {
    for (i = 0; i < 3; i++)
      if (b->cell[i][2 - i] != w)
      	break;
    if (i >= 3)
      return victor;
  }
  /* cleanup */
  if (b->count)
    return WIN_NOTYET;
  return WIN_DRAW;
}

static int minmax(struct board *b, enum who w, int lev, int misere) {
  enum win whowins;
  struct board btmp;
  int i, j, maxval, curmax;

  whowins = referee(b, misere);
  switch (whowins) {
  case WIN_X:
  case WIN_O:
    if (whotowin(w) == whowins)
      return 1;
    return -1;
  case WIN_DRAW:
    return 0;
  case WIN_NOTYET:
    break;
  }
  if (lev <= 0)
    return 0;
  curmax = -2;
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      if (b->cell[i][j] == WHO_B) {
        btmp = *b;
	btmp.movex = i;
	btmp.movey = j;
	btmp.movewho = w;
	btmp.cell[i][j] = w;
	btmp.count--;
	maxval = -minmax(&btmp, whoother(w), lev - 1, misere);
	if (maxval > curmax)
	  curmax = maxval;
      }
  if (curmax > 1 || curmax < -1)
    abort();
  return curmax;
}

void computermove(struct board *b, enum who w, int lev, int misere) {
  int i, j, n, move, threshold;
  int score[3][3];
  struct board btmp;

  for(i = 0; i < 3; i++)
    for(j = 0; j < 3; j++) {
      score[i][j] = -2;
      if (b->cell[i][j] == WHO_B) {
        btmp = *b;
	btmp.movex = i;
	btmp.movey = j;
	btmp.movewho = w;
	btmp.cell[i][j] = w;
	btmp.count--;
        score[i][j] = -minmax(&btmp, whoother(w), lev, misere);
      }
    }
  for (threshold = 1; threshold > -2; --threshold) {
    n = 0;
    for (i = 0; i < 3; i++)
      for(j = 0; j < 3; j++)
        if (score[i][j] >= threshold)
	  n++;
    if (n > 0)
      break;
  }
  if (threshold <= -2)
    abort();
  move = RANDOM(n);
  for(i = 0; i < 3; i++)
    for(j = 0; j < 3; j++)
      if (score[i][j] >= threshold) {
        if (!move) {
	  b->movex = i;
	  b->movey = j;
          b->movewho = w;
          b->cell[i][j] = w;
	  b->count--;
	  return;
        }
	--move;
      }
  abort();
  /*NOTREACHED*/
}

void randommove(struct board *b, enum who w) {
  int i, j, move;

  move = RANDOM(b->count);
  for(i = 0; i < 3; i++)
    for(j = 0; j < 3; j++)
      if (b->cell[i][j] == WHO_B) {
        if (!move) {
	  b->movex = i;
	  b->movey = j;
          b->movewho = w;
          b->cell[i][j] = w;
	  b->count--;
	  return;
        }
	--move;
      }
  abort();
  /*NOTREACHED*/
}

void initboard(void) {
  int move, x, y;

  won = 0;
  currentboard = newboard;
  resetboard();
  if (compfirst)
    onemove();
}


int main(int argc, char **argv) {
  newboard.count = 9;
  displayinit(&argc, argv);
  SEED(getpid());
  initboard();
  go();
  abort();
  /*NOTREACHED*/
}
