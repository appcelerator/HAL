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
	class JSString;

	class HAL_EXPORT JSClass {
	public:
		JSClass() HAL_NOEXCEPT;
		virtual ~JSClass() HAL_NOEXCEPT;
		JSClass(const JSClass& rhs) HAL_NOEXCEPT;
		JSClass(JSClass&& rhs) HAL_NOEXCEPT;
		JSClass& operator=(JSClass rhs) HAL_NOEXCEPT;

		virtual void Copy(const JSClassDefinition& other);
		virtual void Initialize() { assert(false); }

		void SetParent(const JSClass& js_class) {
			js_class_definition__.parentClass = static_cast<JSClassRef>(js_class);
		}

		void SetClassVersion(const std::uint32_t& class_version) {
			js_class_definition__.version = class_version;
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
		JSClassRef js_class_ref__{ nullptr };
		JSClassDefinition js_class_definition__;
		std::vector<::JSStaticValue>    static_values__;
		std::vector<::JSStaticFunction> static_functions__;
#pragma warning(pop)
	};

	template<typename T>
	using GetNamedValuePropertyCallback = std::function<JSValue(T&)>;

	template<typename T>
	using SetNamedValuePropertyCallback = std::function<bool(T&, const JSValue&)>;

	template<typename T>
	using CallNamedFunctionCallback = std::function<JSValue(T&, const std::vector<JSValue>&, JSObject&)>;

	template<typename T>
	using HasPropertyCallback = std::function<bool(T&, const JSString&)>;

	template<typename T>
	using GetPropertyCallback = std::function<JSValue(T&, const JSString&)>;

	template<typename T>
	using SetPropertyCallback = std::function<bool(T&, const JSString&, const JSValue&)>;

	template<typename T>
	class JSExportClass final : public JSClass {
	public:
		JSExportClass() HAL_NOEXCEPT;
		virtual ~JSExportClass() HAL_NOEXCEPT;
		JSExportClass& operator=(const JSExportClass&) = default;
		JSExportClass(const JSExportClass&) = default;
		JSExportClass(JSExportClass&&) = default;

		void AddValueProperty(const std::string& name, GetNamedValuePropertyCallback<T>, SetNamedValuePropertyCallback<T>, bool enumerable);
		void AddConstantProperty(const std::string& name, GetNamedValuePropertyCallback<T>);
		void AddFunctionProperty(const std::string& name, CallNamedFunctionCallback<T> callback);
		void AddHasPropertyCallback(HasPropertyCallback<T> callback);
		void AddGetPropertyCallback(GetPropertyCallback<T> callback);
		void AddSetPropertyCallback(SetPropertyCallback<T> callback);

		virtual void Initialize() override;

		static void CallInitializeFunction(JSContextRef context, JSObjectRef object);
		static void CallFinalizeFunction(JSObjectRef object);
		static JSObjectRef CallAsConstructorFunction(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);
		static JSValueRef CallGetterFunction(JSContextRef context, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception);
		static bool CallSetterFunction(JSContextRef context, JSObjectRef object, JSStringRef propertyName, JSValueRef value, JSValueRef* exception);
		static JSValueRef CallNamedFunction(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);
		static bool CallHasPropertyFunction(JSContextRef ctx, JSObjectRef thisObject, JSStringRef property_name);
		static JSValueRef CallGetPropertyFunction(JSContextRef ctx, JSObjectRef thisObject, JSStringRef property_name, JSValueRef* exception);
		static bool CallSetPropertyFunction(JSContextRef ctx, JSObjectRef thisObject, JSStringRef property_name, JSValueRef value, JSValueRef* exception);

		static std::string GetComponentName(const std::string& function_name);

	private:
		// Prevent heap based objects.
		void* operator new(std::size_t) = delete;   // #1: To prevent allocation of scalar objects
		void* operator new[](std::size_t) = delete; // #2: To prevent allocation of array of objects

#pragma warning(push)
#pragma warning(disable: 4251)
		static std::unordered_map<std::string, CallNamedFunctionCallback<T>> name_to_function_map__;
		static std::unordered_map<std::string, GetNamedValuePropertyCallback<T>> name_to_getter_map__;
		static std::unordered_map<std::string, SetNamedValuePropertyCallback<T>> name_to_setter_map__;
		static std::unordered_map<std::string, JSValueRef> name_to_constant_map__;
		static std::unordered_set<std::string> is_dontenum_set__;

		static HasPropertyCallback<T> has_property_callback__;
		static GetPropertyCallback<T> get_property_callback__;
		static SetPropertyCallback<T> set_property_callback__;
#pragma warning(pop)
	};

	template<typename T>
	JSExportClass<T>::JSExportClass() HAL_NOEXCEPT : JSClass() {

	}

	template<typename T>
	JSExportClass<T>::~JSExportClass() HAL_NOEXCEPT {

	}

	template<typename T>
	std::unordered_map<std::string, CallNamedFunctionCallback<T>> JSExportClass<T>::name_to_function_map__;

	template<typename T>
	std::unordered_map<std::string, GetNamedValuePropertyCallback<T>> JSExportClass<T>::name_to_getter_map__;

	template<typename T>
	std::unordered_map<std::string, SetNamedValuePropertyCallback<T>> JSExportClass<T>::name_to_setter_map__;

	template<typename T>
	std::unordered_map<std::string, JSValueRef> JSExportClass<T>::name_to_constant_map__;

	template<typename T>
	std::unordered_set<std::string> JSExportClass<T>::is_dontenum_set__;

	template<typename T>
	HasPropertyCallback<T> JSExportClass<T>::has_property_callback__;

	template<typename T>
	GetPropertyCallback<T> JSExportClass<T>::get_property_callback__;

	template<typename T>
	SetPropertyCallback<T> JSExportClass<T>::set_property_callback__;

	template<typename T>
	void JSExportClass<T>::AddFunctionProperty(const std::string& name, CallNamedFunctionCallback<T> callback) {
		const auto position = name_to_function_map__.find(name);
		const auto found = position != name_to_function_map__.end();
		assert(!found);
		name_to_function_map__.emplace(name, callback);
	};

	template<typename T>
	void JSExportClass<T>::AddValueProperty(const std::string& name, GetNamedValuePropertyCallback<T> getter, SetNamedValuePropertyCallback<T> setter, bool enumerable) {
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

		if (!enumerable) {
			is_dontenum_set__.emplace(name);
		}
	};

	template<typename T>
	void JSExportClass<T>::AddConstantProperty(const std::string& name, GetNamedValuePropertyCallback<T> getter) {
		const auto getter_position = name_to_getter_map__.find(name);
		const auto getter_found = getter_position != name_to_getter_map__.end();
		assert(!getter_found);
		name_to_getter_map__.emplace(name, getter);
		name_to_constant_map__.emplace(name, nullptr);
	};

	template<typename T>
	void JSExportClass<T>::AddHasPropertyCallback(HasPropertyCallback<T> callback) {
		assert(has_property_callback__ == nullptr);
		has_property_callback__ = callback;
	}

	template<typename T>
	void JSExportClass<T>::AddGetPropertyCallback(GetPropertyCallback<T> callback) {
		assert(get_property_callback__ == nullptr);
		get_property_callback__ = callback;
	}

	template<typename T>
	void JSExportClass<T>::AddSetPropertyCallback(SetPropertyCallback<T> callback) {
		assert(set_property_callback__ == nullptr);
		set_property_callback__ = callback;
	}

	template<typename T>
	void JSExportClass<T>::CallInitializeFunction(JSContextRef local_context, JSObjectRef object) {
		const auto js_context = JSContext(local_context);
		auto js_object  = JSObject(js_context, object);

		const auto export_object_ptr = new T(js_context);
		const auto result = js_object.SetPrivate(export_object_ptr);
		export_object_ptr->initialize(js_object);
		export_object_ptr->postInitialize(js_object);
		assert(result);
	}

	template<typename T>
	void JSExportClass<T>::CallFinalizeFunction(JSObjectRef object) {
		const auto export_object_ptr = static_cast<T*>(JSObjectGetPrivate(object));
		if (export_object_ptr) {
			delete export_object_ptr;
			JSObjectSetPrivate(object, nullptr);
		}
	}

	template<typename T>
	JSObjectRef JSExportClass<T>::CallAsConstructorFunction(JSContextRef local_context, JSObjectRef constructor, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception) {
		const auto js_context = JSContext(local_context);
		try {
			auto js_constructor = JSObject(js_context, constructor);

			auto new_object = js_context.CreateObject(JSExport<T>::Class());
			new_object.SetProperty("constructor", js_constructor);

			const auto export_object_ptr = static_cast<T*>(new_object.GetPrivate());
			const auto js_arguments = detail::to_vector(js_context, argumentCount, arguments);
			export_object_ptr->postCallAsConstructor(js_context, js_arguments);

			return static_cast<JSObjectRef>(new_object);
		} catch (const detail::js_runtime_error& e) {
			*exception = static_cast<JSValueRef>(js_context.CreateError(e));
		} catch (const std::exception& e) {
			*exception = static_cast<JSValueRef>(js_context.CreateError(e.what()));
		} catch (...) {
			*exception = static_cast<JSValueRef>(js_context.CreateError("Unexpected exception"));
		}
		return nullptr;
	}


	template<typename T>
	JSValueRef JSExportClass<T>::CallGetterFunction(JSContextRef local_context, JSObjectRef thisObject, JSStringRef property_name_ref, JSValueRef* exception) {
		const auto property_name = static_cast<std::string>(JSString(property_name_ref));
		const auto js_context = JSContext(local_context);

		try {
			const auto js_object = JSObject(js_context, thisObject);
			const auto export_object_ptr = static_cast<T*>(js_object.GetPrivate());

			if (export_object_ptr) {
				// Check constant cache
				const auto constant_position = name_to_constant_map__.find(property_name);
				const auto constant_found = constant_position != name_to_constant_map__.end();

				if (constant_found && constant_position->second != nullptr) {
					return constant_position->second;
				}

				const auto position = name_to_getter_map__.find(property_name);
				const auto found = position != name_to_getter_map__.end();
				if (found) {
					const auto value_ref = static_cast<JSValueRef>(position->second(*export_object_ptr));
					if (constant_found) {
						name_to_constant_map__[property_name] = value_ref;
					}
					return value_ref;
				} else {
					*exception = static_cast<JSValueRef>(js_context.CreateError("Logical error while calling getter for " + property_name));
				}
			}
		} catch (const detail::js_runtime_error& e) {
			*exception = static_cast<JSValueRef>(js_context.CreateError(e));
		} catch (const std::exception& e) {
			*exception = static_cast<JSValueRef>(js_context.CreateError(e.what()));
		} catch (...) {
			*exception = static_cast<JSValueRef>(js_context.CreateError("Error while calling getter for " + property_name));
		}

		return JSValueMakeUndefined(local_context);
	}

	template<typename T>
	bool JSExportClass<T>::CallSetterFunction(JSContextRef local_context, JSObjectRef thisObject, JSStringRef property_name_ref, JSValueRef value, JSValueRef* exception) {
		const auto property_name = static_cast<std::string>(JSString(property_name_ref));
		const auto js_context = JSContext(local_context);

		try {
			const auto js_object = JSObject(js_context, thisObject);

			const auto export_object_ptr = static_cast<T*>(js_object.GetPrivate());

			if (export_object_ptr) {
				const auto position = name_to_setter_map__.find(property_name);
				const auto found = position != name_to_setter_map__.end();
				if (found) {
					return position->second(*export_object_ptr, JSValue(js_context, value));
				}
			} else {
				*exception = static_cast<JSValueRef>(js_context.CreateError("Logical error while calling setter for " + property_name));
			}
		} catch (const detail::js_runtime_error& e) {
			*exception = static_cast<JSValueRef>(js_context.CreateError(e));
		} catch (const std::exception& e) {
			*exception = static_cast<JSValueRef>(js_context.CreateError(e.what()));
		} catch (...) {
			*exception = static_cast<JSValueRef>(js_context.CreateError("Error while calling setter for " + property_name));
		}

		return false;
	}

	template<typename T>
	JSValueRef JSExportClass<T>::CallNamedFunction(JSContextRef local_context, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception) {
		const auto js_context = JSContext::GetGlobalContext();
		const auto js_function = JSObject(js_context, function);
		const auto js_function_name = js_function.GetProperty("name");
		assert(js_function_name.IsString());
		const auto function_name = static_cast<std::string>(js_function_name);

		const auto component_name = GetComponentName(function_name);
		JSError::NativeStack__.push_back(component_name);

		if (JSError::NativeStack__.size() > 10) {
			JSError::NativeStack__.pop_front();
		}

		try {
			auto js_object = JSObject(js_context, thisObject);
			const auto export_object_ptr = static_cast<T*>(js_object.GetPrivate());
			const auto position = name_to_function_map__.find(function_name);
			const auto found = position != name_to_function_map__.end();
			if (found) {
				// export_object_ptr goes null when funciton is called as a static function. It is acceptable here.
				const auto js_arguments = detail::to_vector(js_context, argumentCount, arguments);
				const auto result = static_cast<JSValueRef>(position->second(*export_object_ptr, js_arguments, js_object));
				if (!JSError::NativeStack__.empty()) {
					JSError::NativeStack__.pop_back();
				}
				return result;
			} else {
				*exception = static_cast<JSValueRef>(js_context.CreateError("Logical error while calling " + function_name));
			}
		} catch (const detail::js_runtime_error& e) {
			*exception = static_cast<JSValueRef>(js_context.CreateError(e));
		} catch (const std::exception& e) {
			*exception = static_cast<JSValueRef>(js_context.CreateError(e.what()));
		} catch (...) {
			*exception = static_cast<JSValueRef>(js_context.CreateError("Error while calling " + function_name));
		}
		return JSValueMakeUndefined(local_context);
	}

	template<typename T>
	bool JSExportClass<T>::CallHasPropertyFunction(JSContextRef local_context, JSObjectRef thisObject, JSStringRef property_name_ref) {
		const auto js_property_name = JSString(property_name_ref);
		const auto js_context = JSContext(local_context);
		const auto js_object = JSObject(js_context, thisObject);
		const auto export_object_ptr = static_cast<T*>(js_object.GetPrivate());

		assert(export_object_ptr);
		assert(has_property_callback__);
		if (export_object_ptr && has_property_callback__) {
			return has_property_callback__(*export_object_ptr, js_property_name);
		}
		return false;
	}

	template<typename T>
	JSValueRef JSExportClass<T>::CallGetPropertyFunction(JSContextRef local_context, JSObjectRef thisObject, JSStringRef property_name_ref, JSValueRef* exception) {
		const auto js_property_name = JSString(property_name_ref);
		const auto js_context = JSContext(local_context);

		try {
			const auto js_object = JSObject(js_context, thisObject);
			const auto export_object_ptr = static_cast<T*>(js_object.GetPrivate());

			assert(export_object_ptr);
			assert(get_property_callback__);
			if (export_object_ptr && get_property_callback__) {
				return static_cast<JSValueRef>(get_property_callback__(*export_object_ptr, js_property_name));
			} else {
				*exception = static_cast<JSValueRef>(js_context.CreateError("Logical error while getting " + static_cast<std::string>(js_property_name)));
			}
		} catch (const detail::js_runtime_error& e) {
			*exception = static_cast<JSValueRef>(js_context.CreateError(e));
		} catch (const std::exception& e) {
			*exception = static_cast<JSValueRef>(js_context.CreateError(e.what()));
		} catch (...) {
			*exception = static_cast<JSValueRef>(js_context.CreateError("Error while getting " + static_cast<std::string>(js_property_name)));
		}
		return JSValueMakeUndefined(local_context);
	}

	template<typename T>
	bool JSExportClass<T>::CallSetPropertyFunction(JSContextRef local_context, JSObjectRef thisObject, JSStringRef property_name_ref, JSValueRef value, JSValueRef* exception) {
		const auto js_property_name = JSString(property_name_ref);
		const auto js_context = JSContext(local_context);

		try {
			const auto js_object = JSObject(js_context, thisObject);
			const auto export_object_ptr = static_cast<T*>(js_object.GetPrivate());

			assert(export_object_ptr);
			assert(set_property_callback__);
			if (export_object_ptr && set_property_callback__) {
				return set_property_callback__(*export_object_ptr, js_property_name, JSValue(js_context, value));
			} else {
				*exception = static_cast<JSValueRef>(js_context.CreateError("Logical error while setting " + static_cast<std::string>(js_property_name)));
			}
		} catch (const detail::js_runtime_error& e) {
			*exception = static_cast<JSValueRef>(js_context.CreateError(e));
		} catch (const std::exception& e) {
			*exception = static_cast<JSValueRef>(js_context.CreateError(e.what()));
		} catch (...) {
			*exception = static_cast<JSValueRef>(js_context.CreateError("Error while setting " + static_cast<std::string>(js_property_name)));
		}
		return false;
	}

	template<typename T>
	void JSExportClass<T>::Initialize() {

		assert(static_values__.empty());
		assert(static_functions__.empty());

		js_class_definition__.initialize = CallInitializeFunction;
		js_class_definition__.finalize   = CallFinalizeFunction;
		js_class_definition__.callAsConstructor = CallAsConstructorFunction;

		js_class_definition__.staticValues = nullptr;
		if (!name_to_getter_map__.empty()) {
			for (auto pair : name_to_getter_map__) {
				const auto property_name = pair.first;

				const auto cptr_size = property_name.size() + 1;
				auto property_name_cptr = new char[cptr_size];
				std::char_traits<char>::copy(property_name_cptr, property_name.c_str(), cptr_size);

				JSStaticValue static_value;
				static_value.name = property_name_cptr;
				static_value.getProperty = CallGetterFunction;
				static_value.setProperty = name_to_setter_map__.find(property_name) != name_to_setter_map__.end() ? CallSetterFunction : nullptr;
				static_value.attributes = is_dontenum_set__.find(property_name) != is_dontenum_set__.end() ? kJSPropertyAttributeDontEnum : kJSPropertyAttributeNone;
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

				const auto cptr_size = function_name.size() + 1;
				auto function_name_cptr = new char[cptr_size];
				std::char_traits<char>::copy(function_name_cptr, function_name.c_str(), cptr_size);

				JSStaticFunction static_function;
				static_function.name = function_name_cptr;
				static_function.callAsFunction = CallNamedFunction;
				static_function.attributes = kJSPropertyAttributeNone;
				static_functions__.push_back(static_function);
			}

			static_functions__.push_back({ nullptr, nullptr, kJSPropertyAttributeNone });
			js_class_definition__.staticFunctions = &static_functions__[0];
		}

		if (has_property_callback__ != nullptr) {
			js_class_definition__.hasProperty = CallHasPropertyFunction;
		}

		if (get_property_callback__ != nullptr) {
			js_class_definition__.getProperty = CallGetPropertyFunction;
		}

		if (set_property_callback__ != nullptr) {
			js_class_definition__.setProperty = CallSetPropertyFunction;
		}

		if (js_class_ref__) {
			JSClassRelease(js_class_ref__);
		}

		js_class_ref__ = JSClassCreate(&js_class_definition__);
	}

	template<typename T>
	std::string JSExportClass<T>::GetComponentName(const std::string& function_name) {
		std::ostringstream os;
		os << "JSExportClass<" << typeid(T).name() << ">::" << function_name;
		return os.str();
	}
} // namespace HAL {

#endif // _HAL_JSCLASS_HPP_
