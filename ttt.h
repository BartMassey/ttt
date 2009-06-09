extern long getpid(void);
extern long lrand48(void);
#ifdef BSD
extern void srandom(long);
extern long random(void);
#define SEED(S) srandom(S)
#define RANDOM(N) ((random()>>2)%(N))
#else
extern void srand48(long);
#define SEED(S) srand48(S)
#define RANDOM(N) ((lrand48()>>2)%(N))
#endif

#define MAX(X,Y) (((X)<(Y))?(Y):(X))

enum who {
  WHO_B,
  WHO_X,
  WHO_O
};

enum win {
  WIN_NOTYET,
  WIN_X,
  WIN_O,
  WIN_DRAW
};

struct board {
  enum who cell[3][3];
  int movex, movey;
  enum who movewho;
  int count;
};

/* ttt.c */

extern enum who humanside;
extern enum who computerside;
extern struct board currentboard;
extern struct board newboard;
extern int compfirst;
extern int won;

extern enum win whotowin(enum who w);
extern enum win referee(struct board *b);
extern void computermove(struct board *b, enum who w, int lev);
extern void randommove(struct board *b, enum who w);
extern void initboard(void);

/* ttt-ui.c */

extern void refreshboard(struct board *b);
extern void resetboard(void);
extern void displayinit(int *argcp, char **argv);
extern void go(void);
