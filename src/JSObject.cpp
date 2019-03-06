/**
 * HAL
 *
 * Copyright (c) 2019 by Axway. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "HAL/JSObject.hpp"
#include "HAL/JSValue.hpp"
#include "HAL/JSString.hpp"
#include "HAL/JSClass.hpp"
#include "HAL/JSError.hpp"
#include "HAL/JSArray.hpp"
#include "HAL/detail/JSUtil.hpp"
#include <cassert>

namespace HAL {

	std::unordered_map<JSObjectRef, std::intptr_t> JSObject::js_object_ref_to_private_data_map__;

	bool JSObject::HasProperty(const std::string& property_name) const HAL_NOEXCEPT {
		const auto js_property_name = JSString(property_name);
		return JSObjectHasProperty(static_cast<JSContextRef>(js_context__), js_object_ref__, static_cast<JSStringRef>(js_property_name));
	}

	JSValue JSObject::GetProperty(const std::string& property_name) const {
		const auto js_property_name = JSString(property_name);
		return GetProperty(js_property_name);
	}

	JSValue JSObject::GetProperty(const JSString property_name) const {
		JSValueRef exception{ nullptr };
		const auto js_context_ref = static_cast<JSContextRef>(js_context__);
		const auto js_value_ref = JSObjectGetProperty(js_context_ref, js_object_ref__, static_cast<JSStringRef>(property_name), &exception);
		if (exception) {
			if (js_value_ref) {
				JSValueUnprotect(js_context_ref, js_value_ref);
			}
			detail::ThrowRuntimeError("Unable to get property ", static_cast<std::string>(property_name));
		}

		assert(js_value_ref);
		return JSValue(js_context__, js_value_ref);
	}

	JSValue JSObject::GetProperty(unsigned property_index) const {
		JSValueRef exception{ nullptr };
		const auto js_context_ref = static_cast<JSContextRef>(js_context__);
		const auto js_value_ref = JSObjectGetPropertyAtIndex(js_context_ref, js_object_ref__, property_index, &exception);
		if (exception) {
			if (js_value_ref) {
				JSValueUnprotect(js_context_ref, js_value_ref);
			}
			detail::ThrowRuntimeError("Unable to get property at index ", std::to_string(property_index));
		}

		assert(js_value_ref);
		return JSValue(js_context__, js_value_ref);
	}

	void JSObject::SetProperty(const std::string& property_name, const JSValue& property_value, const std::unordered_set<JSPropertyAttribute>& attributes) {
		SetProperty(JSString(property_name), property_value, attributes);
	}

	void JSObject::SetProperty(const JSString& property_name, const JSValue& property_value, const std::unordered_set<JSPropertyAttribute>& attributes) {
		JSValueRef exception{ nullptr };
		JSObjectSetProperty(static_cast<JSContextRef>(js_context__), js_object_ref__, 
			static_cast<JSStringRef>(property_name), 
			static_cast<JSValueRef>(property_value), 
			detail::ToJSPropertyAttributes(attributes), &exception);
		if (exception) {
			detail::ThrowRuntimeError(JSValue(js_context__, exception));
		}
	}

	void JSObject::SetProperty(unsigned property_index, const JSValue& property_value) {
		JSValueRef exception{ nullptr };
		JSObjectSetPropertyAtIndex(static_cast<JSContextRef>(js_context__), js_object_ref__, property_index, static_cast<JSValueRef>(property_value), &exception);
		if (exception) {
			detail::ThrowRuntimeError(JSValue(js_context__, exception));
		}
	}

	bool JSObject::DeleteProperty(const std::string& property_name) {
		JSValueRef exception{ nullptr };
		const auto js_property_name = JSString(property_name);
		const bool result = JSObjectDeleteProperty(static_cast<JSContextRef>(js_context__), js_object_ref__, static_cast<JSStringRef>(js_property_name), &exception);
		if (exception) {
			detail::ThrowRuntimeError(JSValue(js_context__, exception));
		}

		return result;
	}

	JSPropertyNameArray JSObject::GetPropertyNames() const HAL_NOEXCEPT {
		return JSPropertyNameArray(*this);
	}

	std::unordered_map<std::string, JSValue> JSObject::GetProperties() const HAL_NOEXCEPT {
		std::unordered_map<std::string, JSValue> properties;
		const auto names = GetPropertyNames();
		for (std::size_t i = 0; i < names.GetCount(); i++) {
			const auto property_name = names.GetNameAtIndex(i);
			properties.emplace(property_name, GetProperty(property_name));
		}
		return properties;
	}

	bool JSObject::IsFunction() const HAL_NOEXCEPT {
		return JSObjectIsFunction(static_cast<JSContextRef>(js_context__), js_object_ref__);
	}

	bool JSObject::IsArray() const HAL_NOEXCEPT {
		JSObject global_object = js_context__.get_global_object();
		JSValue array_value = global_object.GetProperty("Array");
		if (!array_value.IsObject()) {
			return false;
		}

		JSObject array = static_cast<JSObject>(array_value);
		JSValue isArray_value = array.GetProperty("isArray");
		if (!isArray_value.IsObject()) {
			return false;
		}
		JSObject isArray = static_cast<JSObject>(isArray_value);
		if (!isArray.IsFunction()) {
			return false;
		}

		JSValue self = static_cast<JSValue>(*this);
		JSValue result = isArray(self, global_object);
		if (result.IsBoolean()) {
			return static_cast<bool>(result);
		}

		return false;
	}

	bool JSObject::IsError() const HAL_NOEXCEPT {
		const auto global_object = js_context__.get_global_object();
		const auto error_value = global_object.GetProperty("Error");
		if (!error_value.IsObject()) {
			return false;
		}
		const auto error = static_cast<JSObject>(error_value);
		const auto self = static_cast<JSValue>(*this);
		return static_cast<std::string>(self) == "[object Error]" || self.IsInstanceOfConstructor(error);
	}

	bool JSObject::IsConstructor() const HAL_NOEXCEPT {
		return JSObjectIsConstructor(static_cast<JSContextRef>(js_context__), js_object_ref__);
	}

	JSValue JSObject::operator()(JSObject this_object) { return CallAsFunction(std::vector<JSValue>(), this_object); }
	JSValue JSObject::operator()(JSValue&                     argument, JSObject this_object) { return CallAsFunction({ argument }, this_object); }
	JSValue JSObject::operator()(const std::vector<JSValue>&  arguments, JSObject this_object) { return CallAsFunction(arguments, this_object); }

	JSObject JSObject::CallAsConstructor() { return CallAsConstructor(std::vector<JSValue>  {}); }
	JSObject JSObject::CallAsConstructor(const JSValue&               argument) { return CallAsConstructor(std::vector<JSValue>  {argument}); }
	JSObject JSObject::CallAsConstructor(const std::vector<JSValue>&  arguments) {

		if (!IsConstructor()) {
			detail::ThrowRuntimeError("JSObject", "This JavaScript object is not a constructor.");
		}

		const auto js_context_ref = static_cast<JSContextRef>(js_context__);
		JSValueRef exception{ nullptr };
		JSObjectRef js_object_ref = nullptr;
		if (!arguments.empty()) {
			const auto arguments_array = detail::to_vector(arguments);
			js_object_ref = JSObjectCallAsConstructor(js_context_ref, js_object_ref__, arguments_array.size(), &arguments_array[0], &exception);
		} else {
			js_object_ref = JSObjectCallAsConstructor(js_context_ref, js_object_ref__, 0, nullptr, &exception);
		}

		if (exception) {
			if (js_object_ref) {
				JSValueUnprotect(js_context_ref, js_object_ref);
			}
			detail::ThrowRuntimeError(JSValue(js_context__, exception));
		}

		// postcondition
		assert(js_object_ref);
		return JSObject(js_context__, js_object_ref);
	}

	void JSObject::RegisterPrivateData(JSObjectRef js_object_ref, void* private_data) {
		// we won't store nullptr
		if (private_data == nullptr) {
			return;
		}
		const auto value = reinterpret_cast<std::intptr_t>(private_data);

		const auto position = js_object_ref_to_private_data_map__.find(js_object_ref);
		const bool found = position != js_object_ref_to_private_data_map__.end();

		if (found) {
			// private data should not be shared by multiple JSObjectRef
			assert(position->first == js_object_ref);
		} else {
			const auto insert_result = js_object_ref_to_private_data_map__.emplace(js_object_ref, value);
			const bool inserted = insert_result.second;

			// postcondition
			assert(inserted);
		}
	}

	void JSObject::UnRegisterPrivateData(JSObjectRef js_object_ref) {
		const auto position = js_object_ref_to_private_data_map__.find(js_object_ref);
		const bool found = position != js_object_ref_to_private_data_map__.end();

		if (found) {
			js_object_ref_to_private_data_map__.erase(js_object_ref);
		}
	}

	void* JSObject::GetPrivate() const HAL_NOEXCEPT {
		const auto data = JSObjectGetPrivate(js_object_ref__);
		if (data) {
			return data;
		} else {
			const auto position = js_object_ref_to_private_data_map__.find(js_object_ref__);
			const auto found = position != js_object_ref_to_private_data_map__.end();
			if (found) {
				return reinterpret_cast<void*>(position->second);
			}
		}
		return nullptr;
	}

	bool JSObject::SetPrivate(void* data) const HAL_NOEXCEPT {
		return JSObjectSetPrivate(js_object_ref__, data);
	}

	JSObject::~JSObject() HAL_NOEXCEPT {
		JSValue::Unprotect(static_cast<JSContextRef>(js_context__), js_object_ref__);
	}

	JSObject::JSObject(const JSObject& rhs) HAL_NOEXCEPT
		: js_object_ref__(rhs.js_object_ref__)
		, js_context__(rhs.js_context__) {
		JSValue::Protect(static_cast<JSContextRef>(js_context__), js_object_ref__);
	}

	JSObject::JSObject(JSObject&& rhs) HAL_NOEXCEPT
		: js_object_ref__(rhs.js_object_ref__)
		, js_context__(rhs.js_context__) {
		JSValue::Protect(static_cast<JSContextRef>(js_context__), js_object_ref__);
	}

	JSObject& JSObject::operator=(JSObject rhs) {
		JSValue::Protect(static_cast<JSContextRef>(js_context__), js_object_ref__);
		swap(rhs);
		return *this;
	}

	void JSObject::swap(JSObject& other) HAL_NOEXCEPT {
		using std::swap;

		// By swapping the members of two classes, the two classes are
		// effectively swapped.
		swap(js_context__, other.js_context__);
		swap(js_object_ref__, other.js_object_ref__);
	}

	JSObject::JSObject(const JSContext& js_context, const JSClass& js_class)
		: js_context__(js_context)
		, js_object_ref__(JSObjectMake(static_cast<JSContextRef>(js_context), static_cast<JSClassRef>(js_class), nullptr)) {
		JSValue::Protect(static_cast<JSContextRef>(js_context__), js_object_ref__);
	}

	// For interoperability with the JavaScriptCore API.
	JSObject::JSObject(const JSContext& js_context, JSObjectRef js_object_ref)
		: js_context__(js_context)
		, js_object_ref__(js_object_ref) {
		JSValue::Protect(static_cast<JSContextRef>(js_context__), js_object_ref__);
	}

	JSObject::operator JSValue() const {
		return JSValue(js_context__, js_object_ref__);
	}

	JSObject::operator JSArray() const {
		return JSArray(js_context__, js_object_ref__);
	}

	JSObject::operator JSError() const {
		return JSError(js_context__, js_object_ref__);
	}

	JSValue JSObject::CallAsFunction(const std::vector<JSValue>&  arguments, JSObject this_object) {
		if (!IsFunction()) {
			detail::ThrowRuntimeError("JSObject::CallAsFunction", "This JavaScript object is not a function.");
		}

		const auto js_context = JSContext::GetGlobalContext();
		const auto js_context_ref = static_cast<JSContextRef>(js_context);
		JSValueRef exception{ nullptr };
		JSValueRef js_value_ref{ nullptr };
		if (!arguments.empty()) {
			const auto arguments_array = detail::to_vector(arguments);
			js_value_ref = JSObjectCallAsFunction(js_context_ref, js_object_ref__, static_cast<JSObjectRef>(this_object), arguments_array.size(), &arguments_array[0], &exception);
		} else {
			js_value_ref = JSObjectCallAsFunction(js_context_ref, js_object_ref__, static_cast<JSObjectRef>(this_object), 0, nullptr, &exception);
		}

		if (exception) {
			if (js_value_ref) {
				JSValueUnprotect(js_context_ref, js_value_ref);
			}
			detail::ThrowRuntimeError(JSValue(js_context, exception));
		}

		assert(js_value_ref);
		return JSValue(js_context, js_value_ref);
	}

} // namespace HAL {
