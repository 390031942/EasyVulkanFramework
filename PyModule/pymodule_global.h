#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(PYMODULE_LIB)
#  define PYMODULE_EXPORT Q_DECL_EXPORT
# else
#  define PYMODULE_EXPORT Q_DECL_IMPORT
# endif
#else
# define PYMODULE_EXPORT
#endif
