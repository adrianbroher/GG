// -*- C++ -*-
/* GG is a GUI for SDL and OpenGL.
   Copyright (C) 2003-2008 T. Zachary Laine

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2.1
   of the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
    
   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA

   If you do not wish to comply with the terms of the LGPL please
   contact the author as other terms are available for a fee.
    
   Zach Laine
   whatwasthataddress@gmail.com */
   
/** \file AdamParser.h TODO. */

#ifndef _GG_AdamParser_h_
#define _GG_AdamParser_h_

#ifndef GG_API
# ifdef _MSC_VER
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#  undef min
#  undef max
#  ifdef GiGi_EXPORTS
#   define GG_API __declspec(dllexport)
#  else
#   define GG_API __declspec(dllimport)
#  endif
# else
#  define GG_API
# endif
#endif

#include <string>


namespace adobe {
    struct adam_callback_suite_t;
}

namespace GG {

GG_API bool Parse(const std::string& sheet,
                  const std::string& filename,
                  const adobe::adam_callback_suite_t& callbacks);

}

#endif // _GG_AdamParser_h_
