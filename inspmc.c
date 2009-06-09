#include <stdlib.h>
#include <stdio.h>
#include <Xm/Xm.h>
#include "inspmc.h"

static unsigned int one = 1;

static Boolean parsefromstr(Widget w, char *fromstr,
 Pixel *fg, Pixel *bg, char **fn) {
  char *sp, *fgcolorstr, *bgcolorstr;
  int fgcolorstrlen, bgcolorstrlen;
  XrmValue to, from;
  XtAppContext ac = XtWidgetToApplicationContext(w);
  
  /* parse string */
  while (*fromstr == ' ')
    fromstr++;
  for (sp = "xbm:"; *sp && *sp == *fromstr; sp++)
    fromstr++;
  if (*sp) {
    XtAppWarningMsg(ac, "conversionError", "string", "XtToolkitError",
      "Cannot convert \"%s\" to type Pixmap. (unknown format spec)",
      &fromstr, &one
     );
    return FALSE;
  }
  fgcolorstr = fromstr;
  while (*fromstr && *fromstr != ':')
    fromstr++;
  if (!*fromstr) {
    XtAppWarningMsg(ac, "conversionError", "string", "XtToolkitError",
      "Cannot convert \"%s\" to type Pixel. (unterminated foreground spec)",
      &fgcolorstr, &one
     );
    return FALSE;
  }
  fgcolorstrlen = fromstr - fgcolorstr;
  *fromstr++ = '\0';
  bgcolorstr = fromstr;
  while (*fromstr && *fromstr != ':')
    fromstr++;
  if (!*fromstr) {
    XtAppWarningMsg(ac, "conversionError", "string", "XtToolkitError",
      "Cannot convert \"%s\" to type Pixel. (unterminated background spec)",
      &bgcolorstr, &one
     );
    return FALSE;
  }
  bgcolorstrlen = fromstr - bgcolorstr;
  *fromstr++ = '\0';
  while (*fromstr == ' ')
    fromstr++;
  *fn = fromstr;
  /* convert values */
  from.addr = (XtPointer) fgcolorstr;
  from.size = fgcolorstrlen + 1;
  to.addr = (XtPointer) fg;
  to.size = sizeof *fg;
  if (!XtConvertAndStore(w, XtRString, &from, XtRPixel, &to))
    return FALSE;
  from.addr = (XtPointer) bgcolorstr;
  from.size = bgcolorstrlen + 1;
  to.addr = (XtPointer) bg;
  to.size = sizeof *bg;
  if (!XtConvertAndStore(w, XtRString, &from, XtRPixel, &to))
    return FALSE;
  return TRUE;
}

static Boolean cvtStringToPixmap(Display *disp, XrmValue *args, Cardinal *nargs,
  XrmValue *fromVal, XrmValue *toVal, XtPointer *converter_data) {
  char *path, *prefix;
  int res;
  unsigned int w, h;
  int xh, yh;
  Pixel fg, bg;
  Pixmap root, bitmap, pixmap;
  GC gc;
  XGCValues gcv;
  static char fn[1024];
  static char *fnp = fn;
  char *fromstr, *filename;
  char defaultpath[] = ".:/usr/include/X11/bitmaps";
  extern char *getenv();
  Widget widg = *((Widget *) args[0].addr);
  XtAppContext ac = XtWidgetToApplicationContext(widg);

  if (*nargs != 2)
    XtError("Wrong number of arguments to type converter.");
  fromstr = (char *) fromVal->addr;
  if (!fromstr) {
    XtAppWarningMsg(ac, "conversionError", "string", "XtToolkitError",
      "No bitmap file specified.", 0, 0
     );
    return FALSE;
  }
  if (!parsefromstr(widg, fromstr, &fg, &bg, &filename))
    return FALSE;
  root = XRootWindowOfScreen(XtScreen(widg));
  path = getenv("BITMAP_FILE_PATH");
  if (!path)
    path = defaultpath;
  while (*path) {
    prefix = path;
    while (*path != ':' && *path)
      path++;
    if (*path == ':') {
      *path = '\0';
      path++;
    }
    sprintf(fn, "%s/%s", prefix, filename);
    res = XReadBitmapFile(disp, root, fn, &w, &h, &bitmap, &xh, &yh);
    if (res == BitmapOpenFailed) {
      sprintf(fn, "%s/%s.xbm", prefix, (char *) fromVal->addr);
      res = XReadBitmapFile(disp, root, fn, &w, &h, &bitmap, &xh, &yh);
    }
    if (res != BitmapOpenFailed) {
      if (res != BitmapSuccess) {
        XtAppWarningMsg(ac, "conversionError", "string", "XtToolkitError",
         "Cannot convert \"%s\" to type Pixmap. (bad bitmap file)",
         &fnp, &one
         );
      	return FALSE;
      }
      pixmap = XCreatePixmap(disp, root, w, h, *((int *) args[1].addr));
      gcv.foreground = fg;
      gcv.background = bg;
      gcv.graphics_exposures = FALSE;
      gc = XCreateGC(disp, pixmap,
        GCForeground | GCBackground | GCGraphicsExposures, &gcv);
      XCopyPlane(disp, bitmap, pixmap, gc, 0, 0, w, h, 0, 0, 1);
      XFreePixmap(disp, bitmap);
      XFreeGC(disp,gc);
      if (toVal->addr) {
        * (Pixmap *) toVal->addr = pixmap;
      } else {
	static Pixmap tmp;

	tmp = pixmap;
	toVal->addr = (XtPointer) &tmp;
      }
      toVal->size = sizeof pixmap;
      * (Pixmap *) converter_data = pixmap;
      return TRUE;
    }
  }
  XtAppWarningMsg(ac, "conversionError", "string", "XtToolkitError",
   "Cannot convert \"%s\" to type Pixmap. (file not found)",
   &filename, &one
   );
  return FALSE;
}

/*ARGSUSED*/
static void freePixmap(XtAppContext app, XrmValue *to,
 XtPointer converter_data, XrmValue *args, Cardinal *nargs) {
  Display *disp = XDisplayOfScreen(XtScreen(*((Widget *) args[0].addr)));

  if (*nargs != 4)
    abort();
  XFreePixmap(disp, (Pixmap) converter_data);
}

void InstallXmCvtBitmapStringToPixmap(Widget w) {
  static XtConvertArgRec cvtArg[2];
  int depth;

  XtVaGetValues(w, XmNdepth, &depth, NULL);
  cvtArg[0].address_mode = XtImmediate;
  cvtArg[0].address_id = (XtPointer) w;
  cvtArg[0].size = sizeof w;
  cvtArg[1].address_mode = XtImmediate;
  cvtArg[1].address_id = (XtPointer) depth;
  cvtArg[1].size = sizeof depth;
  XtSetTypeConverter(XtRString, XtRPixmap, cvtStringToPixmap,
   cvtArg, XtNumber(cvtArg), XtCacheAll, freePixmap);
}
