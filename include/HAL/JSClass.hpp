/**
* HAL
*
* Copyright (c) 2019 by Axway. All Rights Reserved.
* Licensed under the terms of the Apache Public License.
* Please see the LICENSE included with this distribution for details.
*/

#ifndef _HAL_JSCLASS_HPP_
#define _HAL_JSCLASS_HPP_

#include "HAL/detail/JSBase.hpp"
#include "HAL/detail/JSUtil.hpp"
#include <functional>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <cctype>

namespace HAL {

	class JSContext;
	class JSValue;
	class JSObject;

	typedef std::function<JSValue(JSObject, JSObject, const std::vector<JSValue>&)> JSObjectCallAsFunctionCallback;
	typedef std::function<JSValue(JSObject)> JSObjectGetPropertyCallback;
	typedef std::function<bool(JSObject, const JSValue& value)> JSObjectSetPropertyCallback;

	class HAL_EXPORT JSClass {
	public:
		JSClass() HAL_NOEXCEPT;
		virtual ~JSClass() HAL_NOEXCEPT;
		JSClass(const JSClass& rhs) HAL_NOEXCEPT;
		JSClass(JSClass&& rhs) HAL_NOEXCEPT;
		JSClass& operator=(JSClass rhs) HAL_NOEXCEPT {
			std::swap(js_class_ref__, rhs.js_class_ref__);
			std::swap(js_class_definition__, rhs.js_class_definition__);
			return *this;
		}

		virtual void Initialize(const JSClassDefinition& other);
		virtual void InitializePropertyCallbacks() { assert(false); }
		virtual void AddFunctionProperty(const std::string& name, JSObjectCallAsFunctionCallback callback) { assert(false); }
		virtual void AddValueProperty(const std::string& name, JSObjectGetPropertyCallback, JSObjectSetPropertyCallback) { assert(false); };
		virtual void AddConstantProperty(const std::string& name, JSObjectGetPropertyCallback) { assert(false); };
		virtual void SetParent(const JSClass& js_class) {
			js_class_definition__.parentClass = static_cast<JSClassRef>(js_class);
		}

		explicit operator JSClassRef() const HAL_NOEXCEPT {
			return js_class_ref__;
		}

	protected:
		// Prevent heap based objects.
		void* operator new(std::size_t) = delete;   // #1: To prevent allocation of scalar objects
		void* operator new[](std::size_t) = delete; // #2: To prevent allocation of array of objects

#pragma warning(push)
#pragma warning(disable: 4251)
		JSClassRef js_class_ref__;
		JSClassDefinition js_class_definition__;
		std::vector<::JSStaticValue>    static_values__;
		std::vector<::JSStaticFunction> static_functions__;
#pragma warning(pop)
	};

	template<typename T>
	class JSExportClass final : public JSClass {
	public:
		JSExportClass() HAL_NOEXCEPT;
		virtual ~JSExportClass() HAL_NOEXCEPT;
		JSExportClass& operator=(const JSExportClass&) = default;
		JSExportClass(const JSExportClass&) = default;
		JSExportClass(JSExportClass&&) = default;

		virtual void AddValueProperty(const std::string& name, JSObjectGetPropertyCallback, JSObjectSetPropertyCallback) override;
		virtual void AddConstantProperty(const std::string& name, JSObjectGetPropertyCallback) override;
		virtual void AddFunctionProperty(const std::string& name, JSObjectCallAsFunctionCallback callback) override;
		virtual void InitializePropertyCallbacks() override;

		static JSValueRef CallGetterFunction(JSContextRef context, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception);
		static bool CallSetterFunction(JSContextRef context, JSObjectRef object, JSStringRef propertyName, JSValueRef value, JSValueRef* exception);
		static JSValueRef CallNamedFunction(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);
	protected:
		// Prevent heap based objects.
		void* operator new(std::size_t) = delete;   // #1: To prevent allocation of scalar objects
		void* operator new[](std::size_t) = delete; // #2: To prevent allocation of array of objects

#pragma warning(push)
#pragma warning(disable: 4251)
		static std::unordered_map<std::string, JSObjectCallAsFunctionCallback> name_to_function_map__;
		static std::unordered_map<std::string, JSObjectGetPropertyCallback> name_to_getter_map__;
		static std::unordered_map<std::string, JSObjectSetPropertyCallback> name_to_setter_map__;
		static std::unordered_map<std::string, JSValueRef> name_to_constant_map__;
#pragma warning(pop)
	};

	template<typename T>
	JSExportClass<T>::JSExportClass() HAL_NOEXCEPT : JSClass() {

	}

	template<typename T>
	JSExportClass<T>::~JSExportClass() HAL_NOEXCEPT {

	}

	template<typename T>
	std::unordered_map<std::string, JSObjectCallAsFunctionCallback> JSExportClass<T>::name_to_function_map__;

	template<typename T>
	std::unordered_map<std::string, JSObjectGetPropertyCallback> JSExportClass<T>::name_to_getter_map__;

	template<typename T>
	std::unordered_map<std::string, JSObjectSetPropertyCallback> JSExportClass<T>::name_to_setter_map__;

	template<typename T>
	std::unordered_map<std::string, JSValueRef> JSExportClass<T>::name_to_constant_map__;

	template<typename T>
	void JSExportClass<T>::AddFunctionProperty(const std::string& name, JSObjectCallAsFunctionCallback callback) {
		const auto position = name_to_function_map__.find(name);
		const auto found = position != name_to_function_map__.end();
		assert(!found);
		name_to_function_map__.emplace(name, callback);
	};

	template<typename T>
	void JSExportClass<T>::AddValueProperty(const std::string& name, JSObjectGetPropertyCallback getter, JSObjectSetPropertyCallback setter) {
		{
			const auto getter_position = name_to_getter_map__.find(name);
			const auto getter_found = getter_position != name_to_getter_map__.end();
			assert(!getter_found);
			name_to_getter_map__.emplace(name, getter);
		}
		{
			const auto setter_position = name_to_setter_map__.find(name);
			const auto setter_found = setter_position != name_to_setter_map__.end();
			assert(!setter_found);
			name_to_setter_map__.emplace(name, setter);
		}
	};

	template<typename T>
	void JSExportClass<T>::AddConstantProperty(const std::string& name, JSObjectGetPropertyCallback getter) {
		const auto getter_position = name_to_getter_map__.find(name);
		const auto getter_found = getter_position != name_to_getter_map__.end();
		assert(!getter_found);
		name_to_getter_map__.emplace(name, getter);
		name_to_constant_map__.emplace(name, nullptr);
	};

	template<typename T>
	JSValueRef JSExportClass<T>::CallGetterFunction(JSContextRef context, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) {
		return JSValueMakeUndefined(context);
	}

	template<typename T>
	bool JSExportClass<T>::CallSetterFunction(JSContextRef context, JSObjectRef object, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
		return false;
	}

	template<typename T>
	JSValueRef JSExportClass<T>::CallNamedFunction(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
	{
		return JSValueMakeUndefined(context);
	}

	template<typename T>
	void JSExportClass<T>::InitializePropertyCallbacks() {
		js_class_definition__.staticValues = nullptr;
		if (!name_to_getter_map__.empty()) {
			static_values__.clear();

			for (auto pair : name_to_getter_map__) {
				const auto property_name = pair.first;
				JSStaticValue static_value;
				static_value.name = property_name.c_str()
				static_value.getProperty = CallGetterrFunction;
				static_value.setProperty = name_to_setter_map__.find(property_name) != name_to_setter_map__.end() ? CallSetterFunction : nullptr;
				static_value.attributes = kJSPropertyAttributeNone;
				static_values__.push_back(static_value);
			}

			static_values__.push_back({ nullptr, nullptr, nullptr, kJSPropertyAttributeNone });
			js_class_definition__.staticValues = &static_values__[0];
		}

		js_class_definition__.staticFunctions = nullptr;
		if (!name_to_function_map__.empty()) {
			static_functions__.clear();

			for (const auto pair : name_to_function_map__) {
				const auto function_name = pair.first;
				JSStaticFunction static_function;
				static_function.name = function_name.c_str();
				static_function.callAsFunction = CallNamedFunction;
				static_function.attributes = kJSPropertyAttributeNone;
				static_functions__.push_back(static_function);
			}

			static_functions__.push_back({ nullptr, nullptr, kJSPropertyAttributeNone });
			js_class_definition__.staticFunctions = &static_functions__[0];
		}
	}
} // namespace HAL {

#endif // _HAL_JSCLASS_HPP_
