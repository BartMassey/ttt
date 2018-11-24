#!/bin/sh
# Copyright © 1987 Bart Massey
# [This program is licensed under the "MIT License"]
# Please see the file COPYING in the source
# distribution of this software for license terms.
echo '/* machine generated -- be careful! */'
echo '#include <X11/Intrinsic.h>'
echo ''
echo 'String fallback[] = {'
sed -e 's/"/\\"/' -e 's/^/  "/' -e 's/$/",/' $1
echo '  NULL'
echo '};'
