dnl $Id$
dnl config.m4 for extension xapm

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(xapm, for xapm support,
dnl Make sure that the comment is aligned:
dnl [  --with-xapm             Include xapm support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(xapm, whether to enable xapm support,
Make sure that the comment is aligned:
[  --enable-xapm           Enable xapm support])

if test "$PHP_XAPM" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-xapm -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/xapm.h"  # you most likely want to change this
  dnl if test -r $PHP_XAPM/$SEARCH_FOR; then # path given as parameter
  dnl   XAPM_DIR=$PHP_XAPM
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for xapm files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       XAPM_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$XAPM_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the xapm distribution])
  dnl fi

  dnl # --with-xapm -> add include path
  dnl PHP_ADD_INCLUDE($XAPM_DIR/include)

  dnl # --with-xapm -> check for lib and symbol presence
  dnl LIBNAME=xapm # you may want to change this
  dnl LIBSYMBOL=xapm # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $XAPM_DIR/$PHP_LIBDIR, XAPM_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_XAPMLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong xapm lib version or lib not found])
  dnl ],[
  dnl   -L$XAPM_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(XAPM_SHARED_LIBADD)

  PHP_NEW_EXTENSION(xapm, xapm.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
