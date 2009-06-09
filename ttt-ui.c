/*
 * Copyright © 1987-2009 Bart Massey
 * ALL RIGHTS RESERVED
 * [This program is licensed under the "MIT License"]
 * Please see the file COPYING in the source
 * distribution of this software for license terms.
 */

#include <Xm/Xm.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Form.h>
#include <Xm/Scale.h>
#include "inspmc.h"
#include <stdlib.h>
#include <stdio.h>
#include "ttt.h"

extern String fallback[];

static Widget xowidget[9];
static Widget status;
static XtAppContext app;
static Pixmap xpixmap, opixmap, bpixmap;
static XmString compWinString, humanWinString, drawString, emptyString;
static int curlev;

static struct globalOptions {
  Pixmap xpixmap, opixmap, bpixmap;
  String compWinString, humanWinString, drawString;
  int level;
  Boolean compFirst;
  Boolean misere;
} globalData;

static XtResource resources[] = {
  {"xPixmap", "MovePixmap", XtRPixmap, sizeof(Pixmap),
   XtOffset(struct globalOptions *, xpixmap),
   XtRString, 0},

  {"oPixmap", "MovePixmap", XtRPixmap, sizeof(Pixmap),
   XtOffset(struct globalOptions *, opixmap),
   XtRString, 0},

  {"bPixmap", "MovePixmap", XtRPixmap, sizeof(Pixmap),
   XtOffset(struct globalOptions *, bpixmap),
   XtRString, 0},

  {"compWinString", "WinString", XtRString, sizeof(String),
   XtOffset(struct globalOptions *, compWinString),
   XtRString, (XtPointer) ""},

  {"humanWinString", "WinString", XtRString, sizeof(String),
   XtOffset(struct globalOptions *, humanWinString),
   XtRString, (XtPointer) ""},

  {"drawString", "WinString", XtRString, sizeof(String),
   XtOffset(struct globalOptions *, drawString),
   XtRString, (XtPointer) ""},

  {"level", "Level", XtRInt, sizeof(int),
   XtOffset(struct globalOptions *, level),
   XtRImmediate, (XtPointer) 2},

  {"compFirst", "CompFirst", XtRBoolean, sizeof(Boolean),
   XtOffset(struct globalOptions *, compFirst),
   XtRImmediate, (XtPointer) FALSE},

  {"misere", "Misere", XtRBoolean, sizeof(Boolean),
   XtOffset(struct globalOptions *, misere),
   XtRImmediate, (XtPointer) FALSE},
};

XrmOptionDescRec options[] = {
  { "-x", "*compFirst", XrmoptionNoArg, "false" },
  { "-o", "*compFirst", XrmoptionNoArg, "true" },
  { "-l", "*level", XrmoptionSepArg, 0 },
  { "-m", "*misere", XrmoptionNoArg, "true" },
};

static Pixmap markpixmap(enum who mark) {
  switch (mark) {
  case WHO_X:
    return xpixmap;
  case WHO_O:
    return opixmap;
  case WHO_B:
    break;
  }
  return bpixmap;
}

void refreshboard(struct board *b) {
  int x, y, i;
  enum who w;

  x = b->movex;
  y = b->movey;
  w = b->movewho;
  i = x + 3 * y;
  XtVaSetValues(xowidget[i],
   XmNlabelPixmap, markpixmap(w),
   NULL);
}

void resetboard(void) {
  int i;

  for (i = 0; i < 9; i++)
    XtVaSetValues(xowidget[i],
     XmNlabelPixmap, bpixmap,
     NULL);
  XtVaSetValues(status,
   XmNlabelString, emptyString,
   NULL);
}

/*ARGSUSED*/
static void movecallback(Widget w, XtPointer clientData, XtPointer callData) {
  int i, x, y;
  enum win winner;

  if (won)
    return;
  for (i = 0; i < 9; i++)
    if (xowidget[i] == w)
      break;
  if (i >= 9)
    abort();
  x = i % 3;
  y = i / 3;
  if (currentboard.cell[x][y] != WHO_B)
    return;
  currentboard.movex = x;
  currentboard.movey = y;
  currentboard.movewho = humanside;
  currentboard.cell[x][y] = humanside;
  currentboard.count--;
  refreshboard(&currentboard);
  switch (winner = referee(&currentboard, globalData.misere)) {
  case WIN_NOTYET:
    break;
  case WIN_X:
  case WIN_O:
    if (whotowin(humanside) == winner)
      XtVaSetValues(status, XmNlabelString, humanWinString, NULL);
    else
      XtVaSetValues(status, XmNlabelString, compWinString, NULL);
    won = 1;
    return;
  case WIN_DRAW:
    XtVaSetValues(status, XmNlabelString, drawString, NULL);
    won = 1;
    return;
  }
  if (!RANDOM(MAX(1, 15 * curlev - currentboard.count)))
    randommove(&currentboard, computerside);
  else
    computermove(&currentboard, computerside, 2 * curlev, globalData.misere);
  refreshboard(&currentboard);
  switch (winner = referee(&currentboard, globalData.misere)) {
  case WIN_NOTYET:
    break;
  case WIN_X:
  case WIN_O:
    if (whotowin(humanside) == winner)
      XtVaSetValues(status, XmNlabelString, humanWinString, NULL);
    else
      XtVaSetValues(status, XmNlabelString, compWinString, NULL);
    won = 1;
    break;
  case WIN_DRAW:
    XtVaSetValues(status, XmNlabelString, drawString, NULL);
    won = 1;
    break;
  }
}

/*ARGSUSED*/
static void quitcallback(Widget w, XtPointer cld, XtPointer cad) {
  exit(0);
}

/*ARGSUSED*/
static void newcallback(Widget w, XtPointer cld, XtPointer cad) {
  initboard();
}

/*ARGSUSED*/
static void levelcallback(Widget w, XtPointer cld, XtPointer cad) {
  XmScaleCallbackStruct *scb = (XmScaleCallbackStruct *) cad;

  curlev = scb->value;
}

void displayinit(int *argcp, char **argv) {
  int i;
  Widget shell, layout, board, buttonbox, quit, new, level;
  char wname[] = "cellx";

  shell = XtAppInitialize(&app, "TTT", options, XtNumber(options),
   argcp, argv, fallback, NULL, 0);
  
  InstallXmCvtBitmapStringToPixmap(shell);

  XtGetApplicationResources(shell, &globalData,
   resources, XtNumber(resources), NULL, 0);

  xpixmap = globalData.xpixmap;
  opixmap = globalData.opixmap;
  bpixmap = globalData.bpixmap;
  compfirst = globalData.compFirst;
  curlev = globalData.level;
  humanWinString = XmStringCreateLtoR(
   globalData.humanWinString, XmFONTLIST_DEFAULT_TAG );
  compWinString = XmStringCreateLtoR(
   globalData.compWinString, XmFONTLIST_DEFAULT_TAG );
  drawString = XmStringCreateLtoR(
   globalData.drawString, XmFONTLIST_DEFAULT_TAG );
  emptyString = XmStringCreateLtoR(
   "", XmFONTLIST_DEFAULT_TAG );

  layout = XtVaCreateManagedWidget("layout", xmRowColumnWidgetClass,
   shell, NULL);

  board = XtVaCreateManagedWidget("board", xmRowColumnWidgetClass,
   layout, NULL);

  for (i = 0; i < 9; i++) {
    wname[strlen(wname)-1] = i + '0';   /* XXX ASCII, template */
    xowidget[i] =
     XtVaCreateManagedWidget(wname, xmPushButtonWidgetClass, board,
     XmNlabelType, XmPIXMAP,
     XmNlabelPixmap, bpixmap,
     NULL);
    XtAddCallback(xowidget[i], XmNactivateCallback, movecallback, NULL);
  }

  status = XtVaCreateManagedWidget("status", xmLabelWidgetClass, layout, NULL);

  buttonbox = XtVaCreateManagedWidget("buttonBox", xmFormWidgetClass,
   layout, NULL);

  quit = XtVaCreateManagedWidget("quit", xmPushButtonWidgetClass,
   buttonbox, NULL);
  XtAddCallback(quit, XmNactivateCallback, quitcallback, NULL);

  new = XtVaCreateManagedWidget("new", xmPushButtonWidgetClass,
   buttonbox, XmNleftWidget, quit, NULL);
  XtAddCallback(new, XmNactivateCallback, newcallback, NULL);

  level = XtVaCreateManagedWidget("level", xmScaleWidgetClass,
   buttonbox, XmNvalue, curlev, XmNleftWidget, new, NULL);
  XtAddCallback(level, XmNvalueChangedCallback, levelcallback, NULL);
  
  
  XtRealizeWidget(shell);
}

void go(void) {
  XtAppMainLoop(app);
}
