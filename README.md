# TTT

TTT is a graphical tic-tac-toe program I wrote in 1995 in C
with Motif widgets.  It is every bit as beautiful today.  I
dug it out a while back to add the momentarily-interesting
option of
[mis&egrave;re](http://en.wikipedia.org/wiki/Mis%C3%A8re)
play, following a discussion with Josh Triplett.  Looking at
the resource converter for text to pixmaps reminded me of
how painful programming GUI apps in C with Motif widgets
was.  Thank goodness things are all better now.  Or not.

The interesting feature when TTT was written was its
"play-level" randomization, which tried to give an
interesting but winnable game at low level settings.

## Build and Run

This program depends on
[Open Motif](http://www.opengroup.org/openmotif/) (Debian
package `libmotif-dev`) or
[LessTif](http://lesstif.sourceforge.net). To compile,
you'll need `imake` installed: run `xmkmf` to generate
`Makefile`.

To run this, you'll need to install its resources ("xrdb
-merge TTT-fallback.ad; xrdb -merge TTT-bonus.ad", or "make
install" with appropriate permissions).

## License

This program is licensed under the "MIT License". Please see
the file `COPYING` in this distribution for license terms.
