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

	JSString JSValue::ToJSONString(unsigned indent) const {
		// Create a function to get JSON.stringify
		JsValueRef make_stringify;
		ASSERT_AND_THROW_JS_ERROR(JsParseScript(L"JSON.stringify", 0, L"", &make_stringify));

		JsValueRef global_object_ref;
		JsGetGlobalObject(&global_object_ref);

		// Get JSON.stringify
		JsValueRef js_stringify_ref;
		JsValueRef this_arguments[] = { global_object_ref };
		ASSERT_AND_THROW_JS_ERROR(JsCallFunction(make_stringify, this_arguments, 1, &js_stringify_ref));

#ifndef NDEBUG
		JsValueType js_stringify_type;
		JsGetValueType(js_stringify_ref, &js_stringify_type);
		assert(js_stringify_type == JsValueType::JsFunction);
#endif

		// Call JSON.stringify
		JsValueRef js_string_ref;
		JsValueRef func_arguments[] = { global_object_ref, js_value_ref__ };
		ASSERT_AND_THROW_JS_ERROR(JsCallFunction(js_stringify_ref, func_arguments, 2, &js_string_ref));

		return JSString(js_string_ref);
	}

	JSValue::operator JSString() const {
		if (IsString()) {
			return JSString(js_value_ref__);
		} else {
			JsValueRef stringValue;
			ASSERT_AND_THROW_JS_ERROR(JsConvertValueToString(js_value_ref__, &stringValue));
			return JSString(stringValue);
		}
	}

	JSValue::operator std::string() const {
		if (IsString()) {
			return static_cast<std::string>(JSString(js_value_ref__));
		} else {
			JsValueRef stringValue;
			ASSERT_AND_THROW_JS_ERROR(JsConvertValueToString(js_value_ref__, &stringValue));
			return static_cast<std::string>(JSString(stringValue));
		}
	}

	JSValue::operator bool() const HAL_NOEXCEPT {
		bool boolValue;
		if (IsBoolean()) {
			ASSERT_AND_THROW_JS_ERROR(JsBooleanToBool(js_value_ref__, &boolValue));
		} else {
			JsValueRef booleanValue;
			ASSERT_AND_THROW_JS_ERROR(JsConvertValueToBoolean(js_value_ref__, &booleanValue));
			ASSERT_AND_THROW_JS_ERROR(JsBooleanToBool(booleanValue, &boolValue));
		}
		return boolValue;
	}

	JSValue::operator double() const {
		double doubleValue;
		if (IsNumber()) {
			ASSERT_AND_THROW_JS_ERROR(JsNumberToDouble(js_value_ref__, &doubleValue));
		} else {
			JsValueRef numberValue;
			ASSERT_AND_THROW_JS_ERROR(JsConvertValueToNumber(js_value_ref__, &numberValue));
			ASSERT_AND_THROW_JS_ERROR(JsNumberToDouble(numberValue, &doubleValue));
		}
		return doubleValue;
	}

	JSValue::operator int32_t() const {
		int intValue;
		if (IsNumber()) {
			ASSERT_AND_THROW_JS_ERROR(JsNumberToInt(js_value_ref__, &intValue));
		} else {
			JsValueRef numberValue;
			ASSERT_AND_THROW_JS_ERROR(JsConvertValueToNumber(js_value_ref__, &numberValue));
			ASSERT_AND_THROW_JS_ERROR(JsNumberToInt(numberValue, &intValue));
		}
		return intValue;
	}

	JSValue::operator JSObject() const {
		if (IsObject()) {
			return JSObject(js_value_ref__);
		} else {
			JsValueRef object_ref;
			ASSERT_AND_THROW_JS_ERROR(JsConvertValueToObject(js_value_ref__, &object_ref));
			return JSObject(object_ref);
		}
	}

	bool JSValue::IsUndefined() const HAL_NOEXCEPT {
		JsValueType type;
		ASSERT_AND_THROW_JS_ERROR(JsGetValueType(js_value_ref__, &type));
		return type == JsValueType::JsUndefined;
	}

	bool JSValue::IsNull() const HAL_NOEXCEPT {
		JsValueType type;
		ASSERT_AND_THROW_JS_ERROR(JsGetValueType(js_value_ref__, &type));
		return type == JsValueType::JsNull;
	}

	bool JSValue::IsBoolean() const HAL_NOEXCEPT {
		JsValueType type;
		ASSERT_AND_THROW_JS_ERROR(JsGetValueType(js_value_ref__, &type));
		return type == JsValueType::JsBoolean;
	}

	bool JSValue::IsNumber() const HAL_NOEXCEPT {
		JsValueType type;
		ASSERT_AND_THROW_JS_ERROR(JsGetValueType(js_value_ref__, &type));
		return type == JsValueType::JsNumber;
	}

	bool JSValue::IsString() const HAL_NOEXCEPT {
		JsValueType type;
		ASSERT_AND_THROW_JS_ERROR(JsGetValueType(js_value_ref__, &type));
		return type == JsValueType::JsString;
	}

	bool JSValue::IsObject() const HAL_NOEXCEPT {
		JsValueType type;
		ASSERT_AND_THROW_JS_ERROR(JsGetValueType(js_value_ref__, &type));
		return type == JsValueType::JsObject   ||
			type == JsValueType::JsArray       ||
			type == JsValueType::JsArrayBuffer ||
			type == JsValueType::JsDataView    ||
			type == JsValueType::JsError       ||
			type == JsValueType::JsTypedArray  ||
			type == JsValueType::JsFunction;
	}

	JSValue::~JSValue() HAL_NOEXCEPT {
		JsRelease(js_value_ref__, nullptr);
	}

	JSValue::JSValue(const JSValue& rhs) HAL_NOEXCEPT
		: js_value_ref__(rhs.js_value_ref__) {
		JsAddRef(js_value_ref__, nullptr);
	}

	JSValue::JSValue(JSValue&& rhs) HAL_NOEXCEPT
		: js_value_ref__(rhs.js_value_ref__) {
		JsAddRef(js_value_ref__, nullptr);
	}

	JSValue& JSValue::operator=(JSValue rhs) {
		swap(rhs);
		JsAddRef(js_value_ref__, nullptr);
		return *this;
	}

	void JSValue::swap(JSValue& other) HAL_NOEXCEPT {
		using std::swap;

		// By swapping the members of two classes, the two classes are
		// effectively swapped.
		swap(js_value_ref__, other.js_value_ref__);
	}

	JSValue::JSValue(const JSString& js_string, bool parse_as_json)
	{
		if (parse_as_json) {
			JsValueRef parse;
			ASSERT_AND_THROW_JS_ERROR(JsParseScript(L"JSON.parse", 0, L"", &parse));

			JsValueRef arguments[] = { js_string.js_string_ref__ };
			ASSERT_AND_THROW_JS_ERROR(JsCallFunction(parse, arguments, 1, &js_value_ref__));
		} else {
			js_value_ref__ = js_string.js_string_ref__;
		}
		JsAddRef(js_value_ref__, nullptr);
	}

	// For interoperability with the JSRT API.
	JSValue::JSValue(JsValueRef js_value_ref) HAL_NOEXCEPT
		: js_value_ref__(js_value_ref) {
		JsAddRef(js_value_ref__, nullptr);
	}

	bool operator==(const JSValue& lhs, const JSValue& rhs) HAL_NOEXCEPT {
		bool result;
		ASSERT_AND_THROW_JS_ERROR(JsStrictEquals(lhs.js_value_ref__, rhs.js_value_ref__, &result));
		return result;
	}

} // namespace HAL {
