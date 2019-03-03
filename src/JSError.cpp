/**
 * HAL
 *
 * Copyright (c) 2019 by Axway. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "HAL/JSError.hpp"
#include "HAL/JSValue.hpp"
#include <sstream>

namespace HAL {

	std::deque<std::string> JSError::NativeStack__;

	JSError::JSError(JSContext js_context, const std::vector<JSValue>& arguments)
		: JSObject(js_context, MakeError(js_context, arguments)) {
	}

	JSError::JSError(JSContext js_context, JSObjectRef js_object_ref)
		: JSObject(js_context, js_object_ref) {
	}

	JSError::JSError(JSContext js_context, const std::string& message)
		: JSObject(js_context, MakeError(js_context, { js_context.CreateString(message) })) {
	}

	JSError::JSError(JSContext js_context, const detail::js_runtime_error& e)
		: JSObject(js_context, MakeError(js_context, { js_context.CreateString(e.js_message()) })) {
		SetProperty("name", js_context.CreateString(e.js_name()));
		SetProperty("fileName", js_context.CreateString(e.js_filename()));
		SetProperty("stack", js_context.CreateString(e.js_stack()));
		SetProperty("nativeStack", js_context.CreateString(e.js_nativeStack()));
		SetProperty("lineNumber", js_context.CreateNumber(e.js_linenumber()));
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

	JSObjectRef JSError::MakeError(const JSContext& js_context, const std::vector<JSValue>& arguments) {
		const auto js_context_ref = static_cast<JSContextRef>(js_context);
		JSValueRef exception{ nullptr };
		JSObjectRef js_object_ref = nullptr;
		if (!arguments.empty()) {
			std::vector<JSValueRef> arguments_array = detail::to_vector(arguments);
			js_object_ref = JSObjectMakeError(js_context_ref, arguments_array.size(), &arguments_array[0], &exception);
		} else {
			js_object_ref = JSObjectMakeError(js_context_ref, 0, nullptr, &exception);
		}

		if (exception) {
			if (js_object_ref) {
				JSValueUnprotect(js_context_ref, js_object_ref);
			}
			detail::ThrowRuntimeError(JSValue(js_context, exception));
		}

		return js_object_ref;
	}

} // namespace HAL {
