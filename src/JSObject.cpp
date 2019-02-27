/**
 * HAL
 *
 * Copyright (c) 2018 by Axway. All Rights Reserved.
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

	JsPropertyIdRef JSObject::GetJsPropertyIdRef(const std::string& property_name) HAL_NOEXCEPT {
		const auto position = name_to_property_id_map__.find(property_name);
		if (position != name_to_property_id_map__.end()) {
			return position->second;
		}

		JsPropertyIdRef propertyId;
		const auto name = detail::to_wstring(property_name);
		ASSERT_AND_THROW_JS_ERROR(JsGetPropertyIdFromName(name.data(), &propertyId));
		name_to_property_id_map__.emplace(property_name, propertyId);

		return propertyId;
	}

	bool JSObject::HasProperty(const std::string& property_name) const HAL_NOEXCEPT {
		// JsHasProperty may error out when there's no such property.
		// In that case we just return false.
		bool hasProperty;
		if (JsHasProperty(js_object_ref__, GetJsPropertyIdRef(property_name), &hasProperty) == JsNoError) {
			return hasProperty;
		}
		return false;
	}

	JSValue JSObject::GetProperty(const std::string& property_name) const {
		JsValueRef value;
		ASSERT_AND_THROW_JS_ERROR(JsGetProperty(js_object_ref__, GetJsPropertyIdRef(property_name), &value));
		return value;
	}

	JSValue JSObject::GetProperty(unsigned property_index) const {
		JsValueRef js_value;
		JsValueRef numberValue;
		ASSERT_AND_THROW_JS_ERROR(JsIntToNumber(property_index, &numberValue));
		ASSERT_AND_THROW_JS_ERROR(JsGetIndexedProperty(js_object_ref__, numberValue, &js_value));
		return js_value;
	}

	void JSObject::SetProperty(const std::string& property_name, const JSValue& property_value) {
		ASSERT_AND_THROW_JS_ERROR(JsSetProperty(js_object_ref__, GetJsPropertyIdRef(property_name), static_cast<JsValueRef>(property_value), false));
	}

	void JSObject::SetProperty(unsigned property_index, const JSValue& property_value) {
		JsValueRef numberValue;
		ASSERT_AND_THROW_JS_ERROR(JsIntToNumber(property_index, &numberValue));
		ASSERT_AND_THROW_JS_ERROR(JsSetIndexedProperty(js_object_ref__, numberValue, static_cast<JsValueRef>(property_value)));
	}

	bool JSObject::DeleteProperty(const std::string& property_name) {
		JsValueRef result;
		ASSERT_AND_THROW_JS_ERROR(JsDeleteProperty(js_object_ref__, GetJsPropertyIdRef(property_name), true, &result));
		return static_cast<bool>(JSValue(result));
	}

	JSArray JSObject::GetPropertyNames() const HAL_NOEXCEPT {
		JsValueRef names;
		ASSERT_AND_THROW_JS_ERROR(JsGetOwnPropertyNames(js_object_ref__, &names));
		return JSArray(names);
	}

	std::unordered_map<std::string, JSValue> JSObject::GetProperties() const HAL_NOEXCEPT {
		std::unordered_map<std::string, JSValue> properties;
		for (const auto& property_name : static_cast<std::vector<std::string>>(GetPropertyNames())) {
			// Don't copy special properties
			if (property_name == "caller" || property_name == "arguments" || property_name == "constructor") {
				continue;
			}
			properties.emplace(property_name, GetProperty(property_name));
		}
		return properties;
	}

	bool JSObject::IsFunction() const HAL_NOEXCEPT {
		JsValueType valueType;
		ASSERT_AND_THROW_JS_ERROR(JsGetValueType(js_object_ref__, &valueType));
		return (valueType == JsValueType::JsFunction);
	}

	bool JSObject::IsArray() const HAL_NOEXCEPT {
		JsValueType valueType;
		ASSERT_AND_THROW_JS_ERROR(JsGetValueType(js_object_ref__, &valueType));
		return (valueType == JsValueType::JsArray);
	}

	bool JSObject::IsError() const HAL_NOEXCEPT {
		JsValueType valueType;
		ASSERT_AND_THROW_JS_ERROR(JsGetValueType(js_object_ref__, &valueType));
		return (valueType == JsValueType::JsError);
	}

	bool JSObject::IsConstructor() const HAL_NOEXCEPT {
		JsValueType valueType;
		ASSERT_AND_THROW_JS_ERROR(JsGetValueType(js_object_ref__, &valueType));
		return (valueType == JsValueType::JsFunction);
	}

	JSValue JSObject::operator()(JSObject this_object) { return CallAsFunction(std::vector<JSValue>(), this_object); }
	JSValue JSObject::operator()(JSValue&                     argument, JSObject this_object) { return CallAsFunction({ argument }, this_object); }
	JSValue JSObject::operator()(const std::vector<JSValue>&  arguments, JSObject this_object) { return CallAsFunction(arguments, this_object); }

	JSObject JSObject::CallAsConstructor() { return CallAsConstructor(std::vector<JSValue>  {}); }
	JSObject JSObject::CallAsConstructor(const JSValue&               argument) { return CallAsConstructor(std::vector<JSValue>  {argument}); }
	JSObject JSObject::CallAsConstructor(const std::vector<JSValue>&  arguments) {

		const auto jsexport_object_ptr = static_cast<JSExportObject*>(GetPrivate());
		assert(jsexport_object_ptr != nullptr);
		const auto ctor_func_ref = jsexport_object_ptr->get_constructor();
		assert(ctor_func_ref != nullptr);

		JsValueRef result;
		auto js_args = detail::to_arguments(arguments, js_object_ref__);
		ASSERT_AND_THROW_JS_ERROR(JsConstructObject(ctor_func_ref, &js_args[0], static_cast<unsigned short>(js_args.size()), &result));

		return JSObject(result);
	}

	void* JSObject::GetPrivate() const HAL_NOEXCEPT {
		void* data = nullptr;
		bool hasData = false;
		ASSERT_AND_THROW_JS_ERROR(JsHasExternalData(js_object_ref__, &hasData));
		if (hasData) {
			ASSERT_AND_THROW_JS_ERROR(JsGetExternalData(js_object_ref__, &data));
		}
		return data;
	}

	bool JSObject::SetPrivate(void* data) const HAL_NOEXCEPT {
		return (JsSetExternalData(js_object_ref__, data) == JsErrorCode::JsNoError);
	}

	JSObject::~JSObject() HAL_NOEXCEPT {
		JsRelease(js_object_ref__, nullptr);
	}

	JSObject::JSObject(const JSObject& rhs) HAL_NOEXCEPT
		: js_object_ref__(rhs.js_object_ref__) {
		JsAddRef(js_object_ref__, nullptr);
	}

	JSObject::JSObject(JSObject&& rhs) HAL_NOEXCEPT
		: js_object_ref__(rhs.js_object_ref__) {
		JsAddRef(js_object_ref__, nullptr);
	}

	JSObject& JSObject::operator=(JSObject rhs) {
		JsAddRef(js_object_ref__, nullptr);
		swap(rhs);
		return *this;
	}

	void JSObject::swap(JSObject& other) HAL_NOEXCEPT {
		using std::swap;

		// By swapping the members of two classes, the two classes are
		// effectively swapped.
		swap(js_object_ref__, other.js_object_ref__);
	}

	std::unordered_map<std::string, const JsPropertyIdRef> JSObject::name_to_property_id_map__;
	std::unordered_map<std::uintptr_t, const JsValueRef> JSObject::js_private_data_to_js_object_ref_map__;
	std::unordered_map<std::uintptr_t, JSExportConstructObjectCallback> JSObject::js_ctor_ref_to_constructor_map__;

	JSObject JSObject::GetObject(const JSExportObject* js_export_ptr) {
		const auto key = reinterpret_cast<std::uintptr_t>(js_export_ptr);
		const auto position = js_private_data_to_js_object_ref_map__.find(key);
		const auto found = position != js_private_data_to_js_object_ref_map__.end();
		assert(found);
		if (found) {
			return JSObject(position->second);
		}
		return GetUndefinedRef();
	}

	void JSObject::RegisterJSExportObject(const JSExportObject* js_export_ptr, const JsValueRef js_value_ref) {
		const auto key = reinterpret_cast<std::uintptr_t>(js_export_ptr);
		const auto position = js_private_data_to_js_object_ref_map__.find(key);
		const auto found = position != js_private_data_to_js_object_ref_map__.end();
		assert(!found);
		js_private_data_to_js_object_ref_map__.emplace(key, js_value_ref);
	}

	void JSObject::UnregisterJSExportObject(const JSExportObject* js_export_ptr) {
		const auto key = reinterpret_cast<std::uintptr_t>(js_export_ptr);
		const auto position = js_private_data_to_js_object_ref_map__.find(key);
		const auto found = position != js_private_data_to_js_object_ref_map__.end();
		assert(found);
		const auto js_value_ref = position->second;
		js_private_data_to_js_object_ref_map__.erase(key);
	}

	JSExportConstructObjectCallback JSObject::GetObjectInitializerCallback(const JsValueRef js_ctor_ref) {

		const auto jsexport_object_ptr = JSObject(js_ctor_ref).GetPrivate();
		assert(jsexport_object_ptr != nullptr);

		const auto key = reinterpret_cast<std::uintptr_t>(jsexport_object_ptr);
		const auto position = js_ctor_ref_to_constructor_map__.find(key);
		const auto found = position != js_ctor_ref_to_constructor_map__.end();
		assert(found);
		if (found) {
			return position->second;
		}
		return nullptr;
	}

	void JSObject::RemoveObjectConstructorCallback(const JsValueRef js_ctor_ref) {
		const auto jsexport_object_ptr = JSObject(js_ctor_ref).GetPrivate();
		assert(jsexport_object_ptr != nullptr);

		const auto key = reinterpret_cast<std::uintptr_t>(jsexport_object_ptr);
		const auto position = js_ctor_ref_to_constructor_map__.find(key);
		const auto found = position != js_ctor_ref_to_constructor_map__.end();
		assert(found);
		if (found) {
			js_ctor_ref_to_constructor_map__.erase(key);
		}
	}

	JSObject::JSObject(const JSClass& js_class) {
		const auto ctor_init = js_class.GetInitializeConstructorCallback();
		assert(ctor_init != nullptr);
		ctor_init(&js_object_ref__);

		const auto jsexport_object_ptr = GetPrivate();
		assert(jsexport_object_ptr != nullptr);

		const auto key = reinterpret_cast<std::uintptr_t>(jsexport_object_ptr);
		const auto position = js_ctor_ref_to_constructor_map__.find(key);
		if (position != js_ctor_ref_to_constructor_map__.end()) {
			js_ctor_ref_to_constructor_map__.erase(key);
		}
		js_ctor_ref_to_constructor_map__.emplace(key, js_class.GetConstructObjectCallback());

		JsAddRef(js_object_ref__, nullptr);
	}

	// For interoperability with the JSRT API.
	JSObject::JSObject(JsValueRef js_object_ref)
		: js_object_ref__(js_object_ref) {
		if (js_object_ref__ == nullptr) {
			ASSERT_AND_THROW_JS_ERROR(JsCreateObject(&js_object_ref__));
		}
		JsAddRef(js_object_ref__, nullptr);
	}

	JSObject::operator JSValue() const {
		return JSValue(js_object_ref__);
	}

	JSObject::operator JSArray() const {
		return JSArray(js_object_ref__);
	}

	JSObject::operator JSError() const {
		return JSError(js_object_ref__);
	}

	JSValue JSObject::CallAsFunction(const std::vector<JSValue>&  arguments, JSObject this_object) {
		const auto this_object_ref = static_cast<JsValueRef>(this_object);
		JsValueRef result;
		if (arguments.empty()) {
			JsValueRef js_args[] = { this_object_ref };
			ASSERT_AND_THROW_JS_ERROR(JsCallFunction(js_object_ref__, js_args, 1, &result));
		} else {
			auto js_args = detail::to_arguments(arguments, this_object_ref);
			ASSERT_AND_THROW_JS_ERROR(JsCallFunction(js_object_ref__, &js_args[0], static_cast<unsigned short>(js_args.size()), &result));
		}
		return result;
	}

} // namespace HAL {
