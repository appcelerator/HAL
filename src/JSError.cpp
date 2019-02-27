/**
 * HAL
 *
 * Copyright (c) 2018 by Axway. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "HAL/JSError.hpp"
#include "HAL/JSValue.hpp"
#include <sstream>

namespace HAL {

std::deque<std::string> JSError::NativeStack__; 

JSError::JSError(JsValueRef js_object_ref, const std::vector<JSValue>& arguments)
		: JSObject(js_object_ref) {
}

JSError::JSError(JsValueRef js_object_ref)
		: JSObject(js_object_ref) {
}

std::string JSError::message() const {
	if (HasProperty("message")) {
		return static_cast<std::string>(GetProperty("message"));
	}
	return "";
}

std::string JSError::name() const {
	if (HasProperty("name")) {
		return static_cast<std::string>(GetProperty("name"));
	}
	return "";
}

std::string JSError::filename() const {
	if (HasProperty("fileName")) {
		return static_cast<std::string>(GetProperty("fileName"));
	}
	return "";
}

std::uint32_t JSError::linenumber() const {
	if (HasProperty("lineNumber")) {
		return static_cast<std::uint32_t>(GetProperty("lineNumber"));
	}
	return 0;
}

std::string JSError::stack() const {
	if (HasProperty("stack")) {
		return static_cast<std::string>(GetProperty("stack"));
	}
	return "";
}

std::string JSError::nativeStack() const {
	if (HasProperty("nativeStack")) {
		return static_cast<std::string>(GetProperty("nativeStack"));
	}
	return "";
}

std::string JSError::GetNativeStack() {
	std::ostringstream stacktrace;
	for (auto iter = JSError::NativeStack__.rbegin(); iter != JSError::NativeStack__.rend(); ++iter) {
		stacktrace << (std::distance(JSError::NativeStack__.rbegin(), iter) + 1) << "  " << *iter << "\n";
	}
	return stacktrace.str();
}

void JSError::ClearNativeStack() {
	JSError::NativeStack__.clear();
}

} // namespace HAL {
