/**
 * HAL
 *
 * Copyright (c) 2018 by Axway. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _HAL_JSSTRING_HPP_
#define _HAL_JSSTRING_HPP_

#include "HAL/detail/JSBase.hpp"
#include "HAL/JSValue.hpp"
#include <string>

namespace HAL {

	/*!
	 @class

	 @discussion A JSString is an RAII wrapper around a JSStringRef, the
	 JavaScriptCore C API representation of a JavaScript string.

	 A JSString satisfies satisfies all the requirements for use in all
	 STL containers. For example, a JSString can be used as a key in a
	 std::unordered_map.

	 Specifically, a JSString is comparable with an equivalence relation,
	 provides a strict weak ordering, and provides a custom hash
	 function.
	 */
	class HAL_EXPORT JSString final {

	public:

		/*!
		 @method

		 @abstract Create an empty JavaScript string with a length of zero.

		 @result An empty JavaScript string with a length of zero.
		 */
		JSString() HAL_NOEXCEPT;

		/*!
		 @method

		 @abstract Create a JavaScript string from a null-terminated UTF8
		 string.

		 @param string The null-terminated UTF8 string to copy into the new
		 JSString.

		 @result A JSString containing string.
		 */
		JSString(const std::string& string) HAL_NOEXCEPT;

		/*!
		 @method

		 @abstract Return the number of Unicode characters in this
		 JavaScript string.

		 @result The number of Unicode characters in this JavaScript
		 string.
		 */
		const std::size_t length() const HAL_NOEXCEPT;

		/*!
		 @method

		 @abstract Return the number of Unicode characters in this
		 JavaScript string.

		 @result The number of Unicode characters in this JavaScript
		 string.
		 */
		const std::size_t size() const HAL_NOEXCEPT;

		/*!
		 @method

		 @abstract Return true if this JavaScript string has a length of
		 zero.

		 @result true if this JavaScript string has a length of zero.
		 */
		const bool empty() const HAL_NOEXCEPT;

		/*!
		 @method

		 @abstract Convert this JavaScript string to a UTF-8 encoded
		 std::string.

		 @result This JavaScript string converted to a UTF-8 encoded
		 std::string.
		 */
		explicit operator std::string() const HAL_NOEXCEPT;
		explicit operator LPCWSTR()     const HAL_NOEXCEPT;

		~JSString()                   HAL_NOEXCEPT;
		JSString(const JSString&)     HAL_NOEXCEPT;
		JSString(JSString&&)          HAL_NOEXCEPT;
		JSString& operator=(JSString) HAL_NOEXCEPT;
		void swap(JSString&)          HAL_NOEXCEPT;

		// For interoperability with the JSRT API.
		explicit JSString(JsValueRef js_string_ref) HAL_NOEXCEPT;

		// For interoperability with the JSRT API.
		explicit operator JsValueRef() const {
			return js_string_ref__;
		}

	private:

		// These classes and functions need access to operator
		// JsValueRef().
		friend class JSContext;                 // JSEvaluateScript and JSCheckScriptSyntax
		friend class JSObject;                  // HasProperty
		friend class JSPropertyNameArray;       // GetNameAtIndex
		friend class JSPropertyNameAccumulator; // AddName

		// Only the following classes and functions can create a JSString.
		friend class JSValue;

		// Prevent heap based objects.
		static void * operator new(std::size_t);     // #1: To prevent allocation of scalar objects
		static void * operator new[](std::size_t); // #2: To prevent allocation of array of objects

		friend void swap(JSString& first, JSString& second) HAL_NOEXCEPT;
		HAL_EXPORT friend bool operator==(const JSString& lhs, const JSString& rhs);

		// Silence 4251 on Windows since private member variables do not
		// need to be exported from a DLL.
#pragma warning(push)
#pragma warning(disable: 4251)
		JsValueRef    js_string_ref__{ nullptr };
#pragma warning(pop)

	};

	inline
		std::string to_string(const JSValue& js_value) {
		return static_cast<std::string>(js_value);
	}

	inline
		std::string to_string(const JSString& js_string) {
		return static_cast<std::string>(js_string);
	}

	// Return true if the two JSStrings are equal.
	HAL_EXPORT bool operator==(const JSString& lhs, const JSString& rhs);

	// Return true if the two JSStrings are not equal.
	inline
		HAL_EXPORT bool operator!=(const JSString& lhs, const JSString& rhs) {
		return !(lhs == rhs);
	}

	// Define a strict weak ordering for two JSStrings.
	inline
		bool operator<(const JSString& lhs, const JSString& rhs) {
		return to_string(lhs) < to_string(rhs);
	}

	inline
		bool operator>(const JSString& lhs, const JSString& rhs) {
		return rhs < lhs;
	}

	inline
		bool operator<=(const JSString& lhs, const JSString& rhs) {
		return !(lhs > rhs);
	}

	inline
		bool operator>=(const JSString& lhs, const JSString& rhs) {
		return !(lhs < rhs);
	}

	inline
		std::ostream& operator << (std::ostream& ostream, const JSString& js_string) {
		ostream << to_string(js_string);
		return ostream;
	}

	inline
		void swap(JSString& first, JSString& second) HAL_NOEXCEPT {
		first.swap(second);
	}

} // namespace HAL {

#endif // _HAL_JSSTRING_HPP_
