#!/bin/sh
echo '/* machine generated -- be careful! */'
echo '#include <X11/Intrinsic.h>'
echo ''
echo 'String fallback[] = {'
sed -e 's/"/\\"/' -e 's/^/  "/' -e 's/$/",/' $1
echo '  NULL'
echo '};'
