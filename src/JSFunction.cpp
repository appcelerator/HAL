/**
 * HAL
 *
 * Copyright (c) 2019 by Axway. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "HAL/JSFunction.hpp"

namespace HAL {

	JSFunction::JSFunction(JSContext js_context, JSObjectRef js_object_ref)
		: JSObject(js_context, js_object_ref) {
	}

}