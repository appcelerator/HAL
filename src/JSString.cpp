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

	JSString::JSString() HAL_NOEXCEPT : JSString("") {
	}

	JSString::JSString(const std::string& string) HAL_NOEXCEPT
		: js_string_ref__(JSStringCreateWithUTF8CString(string.c_str())) {
	}

	const std::size_t JSString::length() const  HAL_NOEXCEPT {
		return JSStringGetLength(js_string_ref__);
	}

	const std::size_t JSString::size() const HAL_NOEXCEPT {
		return length();
	}

	const bool JSString::empty() const HAL_NOEXCEPT {
		return length() == 0;
	}

	JSString::operator std::string() const HAL_NOEXCEPT {
		const auto size = JSStringGetMaximumUTF8CStringSize(js_string_ref__);
		const auto buffer = new char[size];
		JSStringGetUTF8CString(js_string_ref__, buffer, size);
		const auto str = std::string(buffer);
		delete[] buffer;

		return str;
	}

	JSString::~JSString() HAL_NOEXCEPT {
		JSStringRelease(js_string_ref__);
	}

	JSString::JSString(const JSString& rhs) HAL_NOEXCEPT
		: js_string_ref__(rhs.js_string_ref__) {
		JSStringRetain(js_string_ref__);
	}

	JSString::JSString(JSString&& rhs) HAL_NOEXCEPT
		: js_string_ref__(rhs.js_string_ref__) {
		JSStringRetain(js_string_ref__);
	}

	JSString& JSString::operator=(JSString rhs) HAL_NOEXCEPT {
		swap(rhs);
		JSStringRetain(js_string_ref__);
		return *this;
	}

	void JSString::swap(JSString& other) HAL_NOEXCEPT {
		using std::swap;

		// By swapping the members of two classes, the two classes are
		// effectively swapped.
		swap(js_string_ref__, other.js_string_ref__);
	}

	// For interoperability with the JavaScriptCore API.
	JSString::JSString(JSStringRef js_string_ref) HAL_NOEXCEPT
		: js_string_ref__(js_string_ref) {
		JSStringRetain(js_string_ref__);
	}

	bool operator==(const JSString& lhs, const JSString& rhs) {
		return JSStringIsEqual(static_cast<JSStringRef>(lhs), static_cast<JSStringRef>(rhs));
	}

} // namespace HAL {
