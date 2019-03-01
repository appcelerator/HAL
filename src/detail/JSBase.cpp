/**
 * HAL
 *
 * Copyright (c) 2019 by Axway. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "HAL/detail/JSBase.hpp"

// Latest JavaScriptCore doesn't expose kJSClassDefinitionEmpty anymore.
// In that case we have a option to expose it to keep backward compatibility
#ifdef HAL_DEFINE_JSCLASSDEFINITIONEMPTY
#ifndef _HAL_DEFINE_KJSCLASSDEFINITIONEMPTY_
#define _HAL_DEFINE_KJSCLASSDEFINITIONEMPTY_
HAL_EXPORT const JSClassDefinition kJSClassDefinitionEmpty = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
#endif
#endif

namespace HAL { namespace detail {


}} // namespace HAL { namespace detail {
