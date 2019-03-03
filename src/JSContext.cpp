/**
 * HAL
 *
 * Copyright (c) 2019 by Axway. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "HAL/JSContext.hpp"
#include "HAL/JSClass.hpp"
#include "HAL/JSFunction.hpp"
#include "HAL/JSString.hpp"
#include "HAL/JSValue.hpp"
#include "HAL/JSObject.hpp"
#include "HAL/JSArray.hpp"
#include "HAL/JSError.hpp"
#include "HAL/detail/JSUtil.hpp"
#include <cassert>

namespace HAL {

	JSGlobalContextRef JSContext::js_global_context_ref__;

	JSObject JSContext::get_global_object() const HAL_NOEXCEPT {
		assert(js_global_context_ref__ != nullptr);
		return JSObject(GetGlobalContext(), JSContextGetGlobalObject(js_global_context_ref__));
	}

	JSObject JSContext::CreateDate() const HAL_NOEXCEPT {
		return CreateDate(std::vector<JSValue>{});
	}

	JSObject JSContext::CreateDate(const std::vector<JSValue>& arguments) const {
		JSValueRef exception{ nullptr };
		JSObjectRef js_object_ref = nullptr;
		if (!arguments.empty()) {
			std::vector<JSValueRef> arguments_array = detail::to_vector(arguments);
			js_object_ref = JSObjectMakeDate(js_global_context_ref__, arguments_array.size(), &arguments_array[0], &exception);
		} else {
			js_object_ref = JSObjectMakeDate(js_global_context_ref__, 0, nullptr, &exception);
		}
		if (exception) {
			if (js_object_ref) {
				JSValueUnprotect(js_global_context_ref__, js_object_ref);
			}
			detail::ThrowRuntimeError(JSValue(JSContext(js_global_context_ref__), exception));
		}
		return JSObject(JSContext(js_global_context_ref__), js_object_ref);
	}

	JSValue JSContext::CreateValueFromJSON(const JSString& js_string) const {
		return JSValue(GetGlobalContext(), js_string, true);
	}

	JSFunction JSContext::CreateFunction(const std::string& body) const {
		return CreateFunction(body, std::vector<JSString>());
	}

	JSFunction JSContext::CreateFunction(const std::string& body, const std::vector<JSString>& parameter_names) const {
		return JSFunction(GetGlobalContext(), body, parameter_names);
	}

	JSValue JSContext::CreateString() const HAL_NOEXCEPT {
		return JSValue(GetGlobalContext(), JSString(""), false);
	}

	JSValue JSContext::CreateString(const JSString& js_string) const HAL_NOEXCEPT {
		return JSValue(GetGlobalContext(), js_string, false);
	}

	JSValue JSContext::CreateString(const char* string) const HAL_NOEXCEPT {
		return CreateString(JSString(string));
	}

	JSValue JSContext::CreateString(const std::string& string) const HAL_NOEXCEPT {
		return CreateString(JSString(string));
	}

	JSValue JSContext::CreateUndefined() const HAL_NOEXCEPT {
		return JSValue(GetGlobalContext(), JSValueMakeUndefined(js_global_context_ref__));
	}

	JSValue JSContext::CreateNull() const HAL_NOEXCEPT {
		return JSValue(GetGlobalContext(), JSValueMakeNull(js_global_context_ref__));
	}

	JSValue JSContext::CreateBoolean(bool boolean) const HAL_NOEXCEPT {
		return JSValue(GetGlobalContext(), JSValueMakeBoolean(js_global_context_ref__, boolean));
	}

	JSValue JSContext::CreateNumber(double number) const HAL_NOEXCEPT {
		return JSValue(GetGlobalContext(), JSValueMakeNumber(js_global_context_ref__, number));
	}

	JSValue JSContext::CreateNumber(int32_t number) const HAL_NOEXCEPT {
		return JSValue(GetGlobalContext(), JSValueMakeNumber(js_global_context_ref__, static_cast<double>(number)));
	}

	JSValue JSContext::CreateNumber(uint32_t number) const HAL_NOEXCEPT {
		return JSValue(GetGlobalContext(), JSValueMakeNumber(js_global_context_ref__, static_cast<double>(number)));
	}

	JSObject JSContext::CreateObject() const HAL_NOEXCEPT {
		return CreateObject(JSClass());
	}

	JSObject JSContext::CreateObject(const JSClass& js_class) const HAL_NOEXCEPT {
		return JSObject(GetGlobalContext(), js_class);
	}

	JSObject JSContext::CreateObject(const std::unordered_map<std::string, JSValue>& properties) const HAL_NOEXCEPT {
		return CreateObject(JSClass(), properties);
	}

	JSObject JSContext::CreateObject(const JSClass& js_class, const std::unordered_map<std::string, JSValue>& properties) const HAL_NOEXCEPT {
		auto object = CreateObject(js_class);
		for (const auto kv : properties) {
			object.SetProperty(kv.first, kv.second);
		}
		return object;
	}

	JSArray JSContext::CreateArray() const HAL_NOEXCEPT {
		return JSArray(GetGlobalContext());
	}

	JSArray JSContext::CreateArray(const std::vector<JSValue>& arguments) const {
		return JSArray(GetGlobalContext(), arguments);
	}

	JSError JSContext::CreateError() const HAL_NOEXCEPT {
		return JSError(GetGlobalContext());
	}

	JSError JSContext::CreateError(const std::vector<JSValue>& arguments) const HAL_NOEXCEPT {
		return JSError(GetGlobalContext(), arguments);
	}

	JSError JSContext::CreateError(const std::string& message) const HAL_NOEXCEPT {
		return JSError(GetGlobalContext(), message);
	}

	JSError JSContext::CreateError(const detail::js_runtime_error& e) const HAL_NOEXCEPT {
		return JSError(GetGlobalContext(), e);
	}

	JSValue JSContext::JSEvaluateScript(const std::string& content, const std::string& source_url) const {
		return JSEvaluateScript(content, get_global_object(), source_url);
	}

	JSValue JSContext::JSEvaluateScript(const std::string& content, JSObject this_object, const std::string& source_url) const {
		const auto js_source_url = JSString(source_url);
		const auto js_content = JSString(content);
		JSValueRef js_value_ref{ nullptr };
		const JSStringRef source_url_ref = source_url.empty() ? nullptr : static_cast<JSStringRef>(js_source_url);
		JSValueRef exception{ nullptr };
		js_value_ref = ::JSEvaluateScript(js_global_context_ref__, static_cast<JSStringRef>(js_content), static_cast<JSObjectRef>(this_object), source_url_ref, 0, &exception);

		if (exception) {
			if (js_value_ref) {
				JSValueUnprotect(js_global_context_ref__, js_value_ref);
			}
			detail::ThrowRuntimeError(JSValue(JSContext(js_global_context_ref__), exception));
		}

		return JSValue(GetGlobalContext(), js_value_ref);
	}

	bool JSContext::JSCheckScriptSyntax(const std::string& content, const std::string& source_url) const HAL_NOEXCEPT {
		const auto js_source_url = JSString(source_url);
		const auto js_content = JSString(content);
		const JSStringRef source_url_ref = (source_url.length() > 0) ? static_cast<JSStringRef>(js_source_url) : nullptr;
		JSValueRef exception{ nullptr };
		bool result = ::JSCheckScriptSyntax(js_global_context_ref__, static_cast<JSStringRef>(js_content), source_url_ref, 0, &exception);

		if (exception) {
			detail::ThrowRuntimeError(JSValue(JSContext(js_global_context_ref__), exception));
		}

		return result;
	}

	void JSContext::GarbageCollect() const HAL_NOEXCEPT {
		JSGarbageCollect(js_global_context_ref__);
	}

	JSContext::~JSContext() HAL_NOEXCEPT {

	}

	JSContext::JSContext(const JSContext& rhs) HAL_NOEXCEPT
		: js_context_ref__(rhs.js_context_ref__) {
		if (js_global_context_ref__ == nullptr) {
			js_global_context_ref__ = JSContextGetGlobalContext(js_context_ref__);
		}
	}

	JSContext::JSContext(JSContext&& rhs) HAL_NOEXCEPT
		: js_context_ref__(rhs.js_context_ref__) {
		if (js_global_context_ref__ == nullptr) {
			js_global_context_ref__ = JSContextGetGlobalContext(js_context_ref__);
		}
	}

	JSContext& JSContext::operator=(JSContext rhs) HAL_NOEXCEPT {
		std::swap(js_context_ref__, rhs.js_context_ref__);
		if (js_global_context_ref__ == nullptr) {
			js_global_context_ref__ = JSContextGetGlobalContext(js_context_ref__);
		}
		return *this;
	}

	// For interoperability with the JavaScriptCore C API.
	JSContext::JSContext(JSContextRef js_context_ref) HAL_NOEXCEPT
		: js_context_ref__(js_context_ref) {
		if (js_global_context_ref__ == nullptr) {
			js_global_context_ref__ = JSContextGetGlobalContext(js_context_ref__);
		}
	}

} // namespace HAL {
