/**
 * HAL
 *
 * Copyright (c) 2014 by Appcelerator, Inc. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "HAL/JSContext.hpp"
#include "HAL/JSClass.hpp"
#include "HAL/JSString.hpp"
#include "HAL/JSValue.hpp"
#include "HAL/JSObject.hpp"
#include "HAL/JSArray.hpp"
#include "HAL/JSError.hpp"
#include "HAL/detail/JSUtil.hpp"

namespace HAL {

	JSObject JSContext::get_global_object() const HAL_NOEXCEPT {
		JsValueRef globalObject;
		ASSERT_AND_THROW_JS_ERROR(JsGetGlobalObject(&globalObject));
		return JSObject(globalObject);
	}

	JSObject JSContext::CreateDate() const HAL_NOEXCEPT {
		return CreateDate(std::vector<JSValue>{});
	}

	JSObject JSContext::CreateDate(const std::vector<JSValue>& arguments) const {
		const auto js_ctor = get_global_object().GetProperty("Date");
		assert(js_ctor.IsObject());
		const auto js_ctor_ref = static_cast<JsValueRef>(js_ctor);
		JsValueRef new_object;
		auto js_args = detail::to_arguments(arguments, js_ctor_ref);
		ASSERT_AND_THROW_JS_ERROR(JsConstructObject(js_ctor_ref, &js_args[0], static_cast<unsigned short>(js_args.size()), &new_object));
		return JSObject(new_object);
	}

	JSValue JSContext::CreateValueFromJSON(const JSString& js_string) const {
		return JSValue(js_string, true);
	}

	JSValue JSContext::CreateString() const HAL_NOEXCEPT {
		return JSValue(JSString(""), false);
	}

	JSValue JSContext::CreateString(const JSString& js_string) const HAL_NOEXCEPT {
		return JSValue(js_string, false);
	}

	JSValue JSContext::CreateString(const char* string) const HAL_NOEXCEPT {
		return CreateString(JSString(string));
	}

	JSValue JSContext::CreateString(const std::string& string) const HAL_NOEXCEPT {
		return CreateString(JSString(string));
	}

	JSValue JSContext::CreateUndefined() const HAL_NOEXCEPT {
		JsValueRef undefinedValue;
		ASSERT_AND_THROW_JS_ERROR(JsGetUndefinedValue(&undefinedValue));
		return undefinedValue;
	}

	JSValue JSContext::CreateNull() const HAL_NOEXCEPT {
		JsValueRef nullValue;
		ASSERT_AND_THROW_JS_ERROR(JsGetNullValue(&nullValue));
		return nullValue;
	}

	JSValue JSContext::CreateBoolean(bool boolean) const HAL_NOEXCEPT {
		JsValueRef boolValue;
		ASSERT_AND_THROW_JS_ERROR(JsBoolToBoolean(boolean, &boolValue));
		return boolValue;
	}

	JSValue JSContext::CreateNumber(double number) const HAL_NOEXCEPT {
		JsValueRef numberValue;
		ASSERT_AND_THROW_JS_ERROR(JsDoubleToNumber(number, &numberValue));
		return numberValue;
	}

	JSValue JSContext::CreateNumber(int32_t number) const HAL_NOEXCEPT {
		JsValueRef numberValue;
		ASSERT_AND_THROW_JS_ERROR(JsIntToNumber(number, &numberValue));
		return numberValue;
	}

	JSValue JSContext::CreateNumber(uint32_t number) const HAL_NOEXCEPT {
		JsValueRef numberValue;
		JsIntToNumber(number, &numberValue);
		return numberValue;
	}

	JSObject JSContext::CreateObject() const HAL_NOEXCEPT {
		return JSObject(nullptr);
	}

	JSObject JSContext::CreateObject(const JSClass& js_class) const HAL_NOEXCEPT {
		return JSObject(js_class);
	}

	JSObject JSContext::CreateObject(const std::unordered_map<std::string, JSValue>& properties) const HAL_NOEXCEPT {
		auto object = JSObject(nullptr);
		for (const auto kv : properties) {
			object.SetProperty(kv.first, kv.second);
		}
		return object;
	}

	JSObject JSContext::CreateObject(const JSClass& js_class, const std::unordered_map<std::string, JSValue>& properties) const HAL_NOEXCEPT {
		auto object = CreateObject(js_class);
		for (const auto kv : properties) {
			object.SetProperty(kv.first, kv.second);
		}
		return object;
	}

	JSArray JSContext::CreateArray() const HAL_NOEXCEPT {
		JsValueRef arrayValue;
		ASSERT_AND_THROW_JS_ERROR(JsCreateArray(0, &arrayValue));
		return JSArray(arrayValue);
	}

	JSArray JSContext::CreateArray(const std::vector<JSValue>& arguments) const {
		JsValueRef arrayValue;
		ASSERT_AND_THROW_JS_ERROR(JsCreateArray(arguments.size(), &arrayValue));

		for (std::size_t i = 0; i < arguments.size(); i++) {
			JsValueRef index;
			ASSERT_AND_THROW_JS_ERROR(JsIntToNumber(static_cast<int>(i), &index));
			ASSERT_AND_THROW_JS_ERROR(JsSetIndexedProperty(arrayValue, index, static_cast<JsValueRef>(arguments.at(i))));
		}

		return JSArray(arrayValue, arguments);
	}

	JSError JSContext::CreateError() const HAL_NOEXCEPT {
		JsValueRef errorValue;
		ASSERT_AND_THROW_JS_ERROR(JsCreateError(static_cast<JsValueRef>(CreateUndefined()), &errorValue));
		return JSError(errorValue);
	}

	JSError JSContext::CreateError(const std::string& message) const HAL_NOEXCEPT {
		JsValueRef errorValue;
		ASSERT_AND_THROW_JS_ERROR(JsCreateError(static_cast<JsValueRef>(CreateString(message)), &errorValue));
		return JSError(errorValue);
	}

	JSValue JSContext::JSEvaluateScript(const std::string& script, const std::string& source_url) const {
		std::wstring script_string = L"";
		std::wstring source_url_string = L"";
		if (!script.empty()) {
			script_string = detail::to_wstring(script);
		}
		if (!source_url.empty()) {
			source_url_string = detail::to_wstring(source_url);
		}

		JsValueRef executable;
		ASSERT_AND_THROW_JS_ERROR(JsParseScript(script_string.data(), 0, source_url_string.data(), &executable));

		auto js_executable = JSObject(executable);
		assert(js_executable.IsFunction());

		return js_executable(get_global_object());
	}

	JSValue JSContext::JSEvaluateScript(const std::string& content, JSObject this_object, const std::string& source_url) const {
		std::wstring script_string = L"";
		std::wstring source_url_string = L"";
		if (!content.empty()) {
			std::string script = "(function() {\n {\n" + content + "\n}\n return this; })";
			script_string = detail::to_wstring(script);
		}
		if (!source_url.empty()) {
			source_url_string = detail::to_wstring(source_url);
		}

		JsValueRef executable;
		ASSERT_AND_THROW_JS_ERROR(JsParseScript(script_string.data(), 0, source_url_string.data(), &executable));

		auto js_executable = static_cast<JSObject>(JSObject(executable)(this_object));
		assert(js_executable.IsFunction());

		JsValueRef executor;
		ASSERT_AND_THROW_JS_ERROR(JsParseScript(L"(function(func, global) { return func.call(global); })", 0, L"", &executor));

		auto js_executor = JSObject(executor)(this_object);
		assert(js_executor.IsObject());

		const std::vector<JSValue> executor_args = { js_executable, this_object };
		return static_cast<JSObject>(js_executor)(executor_args, this_object);
	}

	bool JSContext::JSCheckScriptSyntax(const std::string& script, const std::string& source_url) const HAL_NOEXCEPT {
		// FIXME TODO IMPLEMENT
		return true;
	}

	void JSContext::GarbageCollect() const HAL_NOEXCEPT {
		JsRuntimeHandle runtime;
		ASSERT_AND_THROW_JS_ERROR(JsGetRuntime(js_context_ref__, &runtime));
		JsCollectGarbage(runtime);
	}

	JSContext::~JSContext() HAL_NOEXCEPT {

	}

	JSContext::JSContext(const JSContext& rhs) HAL_NOEXCEPT
		: js_context_ref__(rhs.js_context_ref__) {
	}

	JSContext::JSContext(JSContext&& rhs) HAL_NOEXCEPT
		: js_context_ref__(rhs.js_context_ref__) {
	}

	JSContext& JSContext::operator=(JSContext rhs) HAL_NOEXCEPT {
		swap(rhs);
		return *this;
	}

	void JSContext::swap(JSContext& other) HAL_NOEXCEPT {
		using std::swap;

		// By swapping the members of two classes, the two classes are
		// effectively swapped.
		swap(js_context_ref__, other.js_context_ref__);
	}

	// For interoperability with the JavaScriptCore C API.
	JSContext::JSContext(JsContextRef js_context_ref) HAL_NOEXCEPT
		: js_context_ref__(js_context_ref) {

	}

} // namespace HAL {
