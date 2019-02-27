/**
 * HAL
 *
 * Copyright (c) 2018 by Axway. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "HAL/JSString.hpp"
#include "HAL/detail/JSUtil.hpp"
#include <locale>
#include <codecvt>

namespace HAL {

	JSString::JSString() HAL_NOEXCEPT
		: JSString("") {
	}

	JSString::JSString(const std::string& string) HAL_NOEXCEPT {
		if (string.size() == 0) {
			ASSERT_AND_THROW_JS_ERROR(JsPointerToString(L"", 0, &js_string_ref__));
			return;
		}

		auto stringValue = detail::to_wstring(string);
		ASSERT_AND_THROW_JS_ERROR(JsPointerToString(stringValue.data(), stringValue.size(), &js_string_ref__));
		JsAddRef(js_string_ref__, nullptr);
	}

	const std::size_t JSString::length() const  HAL_NOEXCEPT {
		int length;
		ASSERT_AND_THROW_JS_ERROR(JsGetStringLength(js_string_ref__, &length));
		return static_cast<std::size_t>(length);
	}

	const std::size_t JSString::size() const HAL_NOEXCEPT {
		return length();
	}

	const bool JSString::empty() const HAL_NOEXCEPT {
		return length() == 0;
	}

	JSString::operator LPCWSTR() const HAL_NOEXCEPT {
		JsValueRef js_value_ref = js_string_ref__;
		JsValueType jsType;
		ASSERT_AND_THROW_JS_ERROR(JsGetValueType(js_value_ref, &jsType));
		if (jsType != JsValueType::JsString) {
			ASSERT_AND_THROW_JS_ERROR(JsConvertValueToString(js_string_ref__, &js_value_ref));
		}

		LPCWSTR stringValue = nullptr;
		std::size_t stringLength;
		ASSERT_AND_THROW_JS_ERROR(JsStringToPointer(js_value_ref, &stringValue, &stringLength));
		return stringValue;
	}

	JSString::operator std::string() const HAL_NOEXCEPT {
		JsValueRef js_value_ref = js_string_ref__;
		JsValueType jsType;
		ASSERT_AND_THROW_JS_ERROR(JsGetValueType(js_value_ref, &jsType));
		if (jsType != JsValueType::JsString) {
			ASSERT_AND_THROW_JS_ERROR(JsConvertValueToString(js_string_ref__, &js_value_ref));
		}

		LPCWSTR stringValue = nullptr;
		std::size_t stringLength;
		ASSERT_AND_THROW_JS_ERROR(JsStringToPointer(js_value_ref, &stringValue, &stringLength));
		if (stringLength > 0) {
			std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
			return std::string(converter.to_bytes(stringValue));
		}
		return "";
	}

	JSString::~JSString() HAL_NOEXCEPT {
		JsRelease(js_string_ref__, nullptr);
	}

	JSString::JSString(const JSString& rhs) HAL_NOEXCEPT
		: js_string_ref__(rhs.js_string_ref__) {
		JsAddRef(js_string_ref__, nullptr);
	}

	JSString::JSString(JSString&& rhs) HAL_NOEXCEPT
		: js_string_ref__(rhs.js_string_ref__) {
		JsAddRef(js_string_ref__, nullptr);
	}

	JSString& JSString::operator=(JSString rhs) HAL_NOEXCEPT {
		swap(rhs);
		JsAddRef(js_string_ref__, nullptr);
		return *this;
	}

	void JSString::swap(JSString& other) HAL_NOEXCEPT {
		using std::swap;

		// By swapping the members of two classes, the two classes are
		// effectively swapped.
		swap(js_string_ref__, other.js_string_ref__);
	}

	// For interoperability with the JSRT API.
	JSString::JSString(JsValueRef js_string_ref) HAL_NOEXCEPT
		: js_string_ref__(js_string_ref) {
		JsAddRef(js_string_ref__, nullptr);
	}

	bool operator==(const JSString& lhs, const JSString& rhs) {
		bool result;
		ASSERT_AND_THROW_JS_ERROR(JsStrictEquals(lhs.js_string_ref__, rhs.js_string_ref__, &result));
		return result;
	}

} // namespace HAL {
