/**
 * HAL
 *
 * Copyright (c) 2018 by Axway. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "HAL/JSValue.hpp"
#include "HAL/JSString.hpp"
#include "HAL/JSObject.hpp"
#include "HAL/detail/JSUtil.hpp"

namespace HAL {

	std::unordered_map<std::intptr_t, std::size_t> JSValue::js_value_retain_count_map__;

	void JSValue::Protect(JSContextRef js_context_ref, JSValueRef js_value_ref) {
		const auto ptr = reinterpret_cast<std::intptr_t>(js_value_ref);
		const auto iter = js_value_retain_count_map__.find(ptr);
		if (iter == js_value_retain_count_map__.end()) {
			JSValueProtect(js_context_ref, js_value_ref);
			js_value_retain_count_map__.emplace(ptr, 1);
		} else {
			js_value_retain_count_map__[ptr] = ++iter->second;
		}
	}

	void JSValue::Unprotect(JSContextRef js_context_ref, JSValueRef js_value_ref) {
		const auto ptr = reinterpret_cast<std::intptr_t>(js_value_ref);
		const auto iter = js_value_retain_count_map__.find(ptr);
		if (iter != js_value_retain_count_map__.end()) {
			const auto count = --iter->second;
			if (count == 0) {
				JSValueUnprotect(js_context_ref, js_value_ref);
				js_value_retain_count_map__.erase(ptr);
			} else {
				js_value_retain_count_map__[ptr] = count;
			}
		}
	}

	JSString JSValue::ToJSONString(unsigned indent) const {
		JSValueRef exception{ nullptr };
		JSStringRef js_string_ref = JSValueCreateJSONString(static_cast<JSContextRef>(js_context__), js_value_ref__, indent, &exception);
		if (exception) {
			if (js_string_ref) {
				JSStringRelease(js_string_ref);
			}
			detail::ThrowRuntimeError(JSValue(js_context__, exception));
			return JSString();
		}

		if (js_string_ref) {
			JSString js_string(js_string_ref);
			JSStringRelease(js_string_ref);
			return js_string;
		}

		return JSString();
	}

	JSValue::operator JSString() const {
		JSValueRef exception{ nullptr };
		JSStringRef js_string_ref = JSValueToStringCopy(static_cast<JSContextRef>(js_context__), js_value_ref__, &exception);
		if (exception) {
			if (js_string_ref) {
				JSStringRelease(js_string_ref);
			}
			detail::ThrowRuntimeError(JSValue(js_context__, exception));
			return JSString();
		}

		if (js_string_ref) {
			JSString js_string(js_string_ref);
			JSStringRelease(js_string_ref);
			return js_string;
		}

		return JSString();
	}

	JSValue::operator std::string() const {
		JSValueRef exception{ nullptr };
		JSStringRef js_string_ref = JSValueToStringCopy(static_cast<JSContextRef>(js_context__), js_value_ref__, &exception);
		if (exception) {
			if (js_string_ref) {
				JSStringRelease(js_string_ref);
			}
			detail::ThrowRuntimeError(JSValue(js_context__, exception));
			return "";
		}

		if (js_string_ref) {
			JSString js_string(js_string_ref);
			JSStringRelease(js_string_ref);
			return static_cast<std::string>(js_string);
		}

		return "";
	}

	JSValue::operator bool() const HAL_NOEXCEPT {
#ifdef HAL_USE_STRING_BOOLEAN_CONVERSION
		// Use Java-like string to boolean conversion.
		// This converts "false" string to false & "true" string to true unlike JavaScript standard.
		static JSStringRef js_string_true_ref;
		static JSStringRef js_string_false_ref;
		static std::once_flag of;
		std::call_once(of, [=] {
			js_string_true_ref = JSStringCreateWithUTF8CString("true");
			js_string_false_ref = JSStringCreateWithUTF8CString("false");
		});
		if (IsString()) {
			const auto js_string_ref = JSValueToStringCopy(static_cast<JSContextRef>(js_context__), js_value_ref__, nullptr);
			if (JSStringIsEqual(js_string_ref, js_string_true_ref)) {
				JSStringRelease(js_string_ref);
				return true;
			}
			if (JSStringIsEqual(js_string_ref, js_string_false_ref)) {
				JSStringRelease(js_string_ref);
				return false;
			}
			JSStringRelease(js_string_ref);
		}
#endif
		return JSValueToBoolean(static_cast<JSContextRef>(js_context__), js_value_ref__);
	}

	JSValue::operator double() const {
		JSValueRef exception{ nullptr };
		const double result = JSValueToNumber(static_cast<JSContextRef>(js_context__), js_value_ref__, &exception);

		if (exception) {
			detail::ThrowRuntimeError(JSValue(js_context__, exception));
		}

		return result;
	}

	JSValue::operator int32_t() const {
		return detail::to_int32_t(operator double());
	}

	JSValue::operator JSObject() const {
		const auto js_context_ref__ = static_cast<JSContextRef>(js_context__);

		JSValueRef exception{ nullptr };
		auto js_object_ref = JSValueToObject(js_context_ref__, js_value_ref__, &exception);

		if (exception) {
			if (js_object_ref) {
				JSValueUnprotect(js_context_ref__, js_object_ref);
			}
			detail::ThrowRuntimeError(JSValue(js_context__, exception));
		}

		return JSObject(js_context__, js_object_ref);
	}

	bool JSValue::IsUndefined() const HAL_NOEXCEPT {
		return JSValueIsUndefined(static_cast<JSContextRef>(js_context__), js_value_ref__);
	}

	bool JSValue::IsNull() const HAL_NOEXCEPT {
		return JSValueIsNull(static_cast<JSContextRef>(js_context__), js_value_ref__);
	}

	bool JSValue::IsBoolean() const HAL_NOEXCEPT {
		return JSValueIsBoolean(static_cast<JSContextRef>(js_context__), js_value_ref__);
	}

	bool JSValue::IsNumber() const HAL_NOEXCEPT {
		return JSValueIsNumber(static_cast<JSContextRef>(js_context__), js_value_ref__);
	}

	bool JSValue::IsString() const HAL_NOEXCEPT {
		return JSValueIsString(static_cast<JSContextRef>(js_context__), js_value_ref__);
	}

	bool JSValue::IsObject() const HAL_NOEXCEPT {
		return JSValueIsObject(static_cast<JSContextRef>(js_context__), js_value_ref__);
	}

	bool JSValue::IsInstanceOfConstructor(const JSObject& constructor) const {
		JSValueRef exception{ nullptr };
		const bool result = JSValueIsInstanceOfConstructor(static_cast<JSContextRef>(js_context__), js_value_ref__, static_cast<JSObjectRef>(constructor), &exception);
		if (exception) {
			detail::ThrowRuntimeError(JSValue(js_context__, exception));
		}

		return result;
	}
	JSValue::~JSValue() HAL_NOEXCEPT {
		Unprotect(static_cast<JSContextRef>(js_context__), js_value_ref__);
	}

	JSValue::JSValue(const JSValue& rhs) HAL_NOEXCEPT
		: js_context__(rhs.js_context__)
		, js_value_ref__(rhs.js_value_ref__) {
		Protect(static_cast<JSContextRef>(js_context__), js_value_ref__);
	}

	JSValue::JSValue(JSValue&& rhs) HAL_NOEXCEPT
		: js_context__(rhs.js_context__)
		, js_value_ref__(rhs.js_value_ref__) {
		Protect(static_cast<JSContextRef>(js_context__), js_value_ref__);
	}

	JSValue& JSValue::operator=(JSValue rhs) {
		swap(rhs);
		Protect(static_cast<JSContextRef>(js_context__), js_value_ref__);
		return *this;
	}

	void JSValue::swap(JSValue& other) HAL_NOEXCEPT {
		using std::swap;

		// By swapping the members of two classes, the two classes are
		// effectively swapped.
		swap(js_value_ref__, other.js_value_ref__);
		swap(js_context__, other.js_context__);
	}

	JSValue::JSValue(JSContext js_context, const JSString& js_string, bool parse_as_json)
		: js_context__(js_context) {
		if (parse_as_json) {
			js_value_ref__ = JSValueMakeFromJSONString(static_cast<JSContextRef>(js_context), static_cast<JSStringRef>(js_string));
			if (!js_value_ref__) {
				const std::string message = "Input is not a valid JSON string: " + to_string(js_string);
				detail::ThrowRuntimeError("JSValue", message);
			}
		} else {
			js_value_ref__ = JSValueMakeString(static_cast<JSContextRef>(js_context__), static_cast<JSStringRef>(js_string));
		}
		Protect(static_cast<JSContextRef>(js_context__), js_value_ref__);
	}

	// For interoperability with the JavaScriptCore API.
	JSValue::JSValue(JSContext js_context, JSValueRef js_value_ref) HAL_NOEXCEPT
		: js_context__(js_context)
		, js_value_ref__(js_value_ref) {
		Protect(static_cast<JSContextRef>(js_context__), js_value_ref__);
	}

	bool operator==(const JSValue& lhs, const JSValue& rhs) HAL_NOEXCEPT {
		return JSValueIsStrictEqual(static_cast<JSContextRef>(lhs.get_context()), static_cast<JSValueRef>(lhs), static_cast<JSValueRef>(rhs));
	}

} // namespace HAL {
