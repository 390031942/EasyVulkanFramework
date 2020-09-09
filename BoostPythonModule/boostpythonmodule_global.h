#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(BOOSTPYTHONMODULE_LIB)
#  define BOOSTPYTHONMODULE_EXPORT Q_DECL_EXPORT
# else
#  define BOOSTPYTHONMODULE_EXPORT Q_DECL_IMPORT
# endif
#else
# define BOOSTPYTHONMODULE_EXPORT
#endif
