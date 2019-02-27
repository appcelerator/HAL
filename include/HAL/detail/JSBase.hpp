/**
 * HAL
 *
 * Copyright (c) 2018 by Axway. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _HAL_DETAIL_JSBASE_HPP_
#define _HAL_DETAIL_JSBASE_HPP_

#define HAL_NOEXCEPT_ENABLE
#define HAL_MOVE_CTOR_AND_ASSIGN_DEFAULT_ENABLE

// See http://msdn.microsoft.com/en-us/library/b0084kay.aspx for the
// list of Visual C++ "Predefined Macros". Visual Studio 2013 Update 3
// RTM ships with MSVC 18.0.30723.0

#if defined(_MSC_VER) && _MSC_VER <= 1800
// According to Microsoft's "Visual C++ Team Blog":
//
// VS 2013 supported rvalue references, except for automatically
// generated move ctors/assigns. Similarly, VS 2013 supported
// defaulted and deleted functions, except for defaulted move
// ctors/assigns.
//
// The CTP supports noexcept's unconditional form. (Additionally,
// while noexcept's terminate() semantics have been implemented, they
// require library support, and the CTP was intentionally shipped
// without updated libraries.)
//
// Reference:
// http://blogs.msdn.com/b/vcblog/archive/2013/12/02/c-11-14-core-language-features-in-vs-2013-and-the-nov-2013-ctp.aspx

#undef HAL_NOEXCEPT_ENABLE
#undef HAL_MOVE_CTOR_AND_ASSIGN_DEFAULT_ENABLE

#endif  // #defined(_MSC_VER) && _MSC_VER <= 1800

#ifdef HAL_NOEXCEPT_ENABLE
#define HAL_NOEXCEPT noexcept
#else
#define HAL_NOEXCEPT
#endif

#include "HAL_EXPORT.h"

#define USE_EDGEMODE_JSRT
#define NOMINMAX
#define NOGDI

#include <jsrt.h>

#define ASSERT_AND_THROW_JS_ERROR(x) HAL::detail::CheckAndThrowChakraRuntimeError(x);

#endif  // _HAL_DETAIL_JSBASE_HPP_
