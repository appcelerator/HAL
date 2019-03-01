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

	JSFunction::JSFunction(JSContext js_context, const std::string& body, const std::vector<JSString>& parameter_names)
		: JSObject(js_context, MakeFunction(js_context, body, parameter_names)) {
	}

	JSObjectRef JSFunction::MakeFunction(const JSContext& js_context, const std::string& body, const std::vector<JSString>& parameter_names) {
		const auto function_name = JSString("anonymous");
		const auto js_body = JSString(body);
		const auto js_context_ref = static_cast<JSContextRef>(js_context);
		JSValueRef exception{ nullptr };
		JSObjectRef js_object_ref = nullptr;
		if (!parameter_names.empty()) {
			std::vector<JSStringRef> parameter_name_array = detail::to_vector(parameter_names);
			js_object_ref = JSObjectMakeFunction(js_context_ref, static_cast<JSStringRef>(function_name), static_cast<unsigned>(parameter_name_array.size()), &parameter_name_array[0], static_cast<JSStringRef>(js_body), nullptr, 0, &exception);
		} else {
			js_object_ref = JSObjectMakeFunction(js_context_ref, static_cast<JSStringRef>(function_name), 0, nullptr, static_cast<JSStringRef>(js_body), nullptr, 0, &exception);
		}

		if (exception) {
			if (js_object_ref) {
				JSValueUnprotect(js_context_ref, js_object_ref);
			}
			detail::ThrowRuntimeError("Unable to make anonymous function");
		}

		return js_object_ref;
	}

}