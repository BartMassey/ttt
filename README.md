# TTT
Copyright (c) 1987 Bart Massey

TTT is an X11 tic-tac-toe program I wrote in 1987
in C.  It is every bit as beautiful today.

TTT was apparently originally textual; I clearly later added
a GUI with Motif widgets. The early development history is
long lost.

Looking at the resource converter for text to pixmaps
reminded me of how painful programming GUI apps in C with
Motif widgets was.  Thank goodness things are all better
now.  Or not.

I dug this code out a while back to add the
momentarily-interesting option of
[mis&egrave;re](http://en.wikipedia.org/wiki/Mis%C3%A8re)
play, following a discussion with Josh Triplett.

The interesting feature when TTT was written was its
"play-level" randomization, which tried to give an
interesting but winnable game at low level settings.

## Build and Run

This program depends on
[Open Motif](http://www.opengroup.org/openmotif/) (Debian
package `libmotif-dev`) or
[LessTif](http://lesstif.sourceforge.net). To compile,
you'll need `imake` installed (Debian package `xutils-dev`):
run `xmkmf` to generate a `Makefile`. The Debian version of
`imake` may yield warnings about "use `_DEFAULT` instead":
there is a
[patch](https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=870836)
for this in progress, and the warnings can be safely
ignored.

To run this, you'll need to install the "resources" used by
the UI. You can

    xrdb -merge TTT-fallback.ad
    xrdb -merge TTT-bonus.ad

or you can `make install` (with appropriate permissions).

## "TODO"

OK, neither of things will ever be done, but I was clearly
thinking of them at one point.

* Improve resource handling so that the program falls back
  to text if the bitmap graphics are unavailable.

* Embed fallback bitmap graphics in the program, or failing
  that at least graphics resources.

## License

This program is licensed under the "MIT License". Please see
the file `COPYING` in this distribution for license terms.
