/**
* HAL
*
* Copyright (c) 2018 by Axway. All Rights Reserved.
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
	typedef std::function<void(JsValueRef, bool, JsValueRef*, unsigned short, JsValueRef*)> JSExportConstructObjectCallback;
	typedef std::function<void(JsValueRef*)> JSExportInitializeConstructorCallback;
	typedef std::function<void(JsValueRef*)> JSExportInitializePropertiesCallback;

	class HAL_EXPORT JSClass {
	public:
		JSClass() HAL_NOEXCEPT { };
		virtual ~JSClass()          HAL_NOEXCEPT { }
		JSClass(const JSClass& rhs) HAL_NOEXCEPT
			: parent_initialize_ctor_callback__(rhs.parent_initialize_ctor_callback__)
			, parent_initialize_properties_callback__(rhs.parent_initialize_properties_callback__) {
			overloaded_initialize_ctor_callback__ = rhs.GetInitializeConstructorCallback();
			overloaded_construct_object_callback__ = rhs.GetConstructObjectCallback();
			overloaded_initialize_properties_callback__ = rhs.GetInitializePropertiesCallback();
		}
		JSClass(JSClass&& rhs) HAL_NOEXCEPT
			: parent_initialize_ctor_callback__(rhs.parent_initialize_ctor_callback__)
			, parent_initialize_properties_callback__(rhs.parent_initialize_properties_callback__) {
			overloaded_initialize_ctor_callback__ = rhs.GetInitializeConstructorCallback();
			overloaded_construct_object_callback__ = rhs.GetConstructObjectCallback();
			overloaded_initialize_properties_callback__ = rhs.GetInitializePropertiesCallback();
		}
		JSClass& operator=(JSClass rhs) HAL_NOEXCEPT {
			parent_initialize_ctor_callback__ = rhs.parent_initialize_ctor_callback__;
			parent_initialize_properties_callback__ = rhs.parent_initialize_properties_callback__;
			overloaded_initialize_ctor_callback__ = rhs.GetInitializeConstructorCallback();
			overloaded_construct_object_callback__ = rhs.GetConstructObjectCallback();
			overloaded_initialize_properties_callback__ = rhs.GetInitializePropertiesCallback();
			return *this;
		}

		virtual void AddFunctionProperty(const std::string& name, JSObjectCallAsFunctionCallback callback) { assert(false); }
		virtual void AddValueProperty(const std::string& name, JSObjectGetPropertyCallback, JSObjectSetPropertyCallback) { assert(false); };
		virtual void AddConstantProperty(const std::string& name, JSObjectGetPropertyCallback) { assert(false); };
		virtual void SetParent(const JSClass& js_class) { assert(false); }

		virtual JSExportInitializeConstructorCallback GetInitializeConstructorCallback() const {
			if (overloaded_initialize_ctor_callback__) {
				return overloaded_initialize_ctor_callback__;
			}
			return nullptr;
		}
		virtual JSExportConstructObjectCallback GetConstructObjectCallback() const {
			if (overloaded_construct_object_callback__) {
				return overloaded_construct_object_callback__;
			}
			return nullptr;
		}
		virtual JSExportInitializePropertiesCallback GetInitializePropertiesCallback() const {
			if (overloaded_initialize_properties_callback__) {
				return overloaded_initialize_properties_callback__;
			}
			return nullptr;
		}
	protected:

		// Prevent heap based objects.
		void* operator new(std::size_t) = delete;   // #1: To prevent allocation of scalar objects
		void* operator new[](std::size_t) = delete; // #2: To prevent allocation of array of objects

#pragma warning(push)
#pragma warning(disable: 4251)
		JSExportInitializeConstructorCallback parent_initialize_ctor_callback__{ nullptr };
		JSExportInitializePropertiesCallback parent_initialize_properties_callback__{ nullptr };

		// Only used for copy constructor and assignment operator
		JSExportInitializeConstructorCallback overloaded_initialize_ctor_callback__{ nullptr };
		JSExportConstructObjectCallback overloaded_construct_object_callback__{ nullptr };
		JSExportInitializePropertiesCallback overloaded_initialize_properties_callback__{ nullptr };
#pragma warning(pop)

	};

	struct NamedFunctionCallbackState {
		std::string name;
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
		virtual void SetParent(const JSClass& js_class) override;
		virtual JSExportInitializeConstructorCallback GetInitializeConstructorCallback() const override;
		virtual JSExportConstructObjectCallback GetConstructObjectCallback() const override;
		virtual JSExportInitializePropertiesCallback GetInitializePropertiesCallback() const override;

		static JsValueRef CallNamedFunction(JsValueRef callee, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
		static JsValueRef CallGetterFunction(JsValueRef callee, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
		static JsValueRef CallSetterFunction(JsValueRef callee, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);

	protected:
		// Prevent heap based objects.
		void* operator new(std::size_t) = delete;   // #1: To prevent allocation of scalar objects
		void* operator new[](std::size_t) = delete; // #2: To prevent allocation of array of objects

#pragma warning(push)
#pragma warning(disable: 4251)
		static std::unordered_map<std::string, JSObjectCallAsFunctionCallback> name_to_function_map__;
		static std::unordered_map<std::string, JSObjectGetPropertyCallback> name_to_getter_map__;
		static std::unordered_map<std::string, JSObjectSetPropertyCallback> name_to_setter_map__;
		static std::unordered_map<std::string, JsValueRef> name_to_constant_map__;
#pragma warning(pop)
	};

	template<typename T>
	JSExportClass<T>::JSExportClass() HAL_NOEXCEPT {

	}

	template<typename T>
	JSExportClass<T>::~JSExportClass() HAL_NOEXCEPT {
	}

	template<typename T>
	JsValueRef JSExportClass<T>::CallNamedFunction(JsValueRef callee, JsValueRef *arguments, unsigned short argumentCount, void *callbackState) {
		const auto state = static_cast<NamedFunctionCallbackState*>(callbackState);
		assert(state != nullptr);

		const auto js_arguments = detail::to_arguments(arguments, argumentCount);
		auto this_object = JSObject(arguments[0]);
		auto function_object = JSObject(callee);
		if (static_cast<JSValue>(this_object).IsUndefined()) {
			this_object = function_object.get_context().get_global_object();
		}

		const auto position = name_to_function_map__.find(state->name);
		const auto found = position != name_to_function_map__.end();
		assert(found);

		return static_cast<JsValueRef>(position->second(function_object, this_object, js_arguments));
	}

	template<typename T>
	JsValueRef JSExportClass<T>::CallGetterFunction(JsValueRef callee, JsValueRef *arguments, unsigned short argumentCount, void *callbackState) {
		const auto state = static_cast<NamedFunctionCallbackState*>(callbackState);
		assert(state != nullptr);

		auto this_object = JSObject(arguments[0]);
		auto function_object = JSObject(callee);
		if (static_cast<JSValue>(this_object).IsUndefined()) {
			this_object = function_object.get_context().get_global_object();
		}

		const auto name = state->name;

		const auto constant_position = name_to_constant_map__.find(name);
		const auto constant_found = constant_position != name_to_constant_map__.end();

		if (constant_found && constant_position->second != nullptr) {
			return constant_position->second;
		}

		const auto position = name_to_getter_map__.find(name);
		const auto found = position != name_to_getter_map__.end();
		assert(found);

		const auto js_value_ref = static_cast<JsValueRef>(position->second(this_object));
		if (constant_found) {
			JsAddRef(js_value_ref, nullptr);
			name_to_constant_map__[name] = js_value_ref;
		}
		return js_value_ref;
	}

	template<typename T>
	JsValueRef JSExportClass<T>::CallSetterFunction(JsValueRef callee, JsValueRef *arguments, unsigned short argumentCount, void *callbackState) {
		const auto state = static_cast<NamedFunctionCallbackState*>(callbackState);
		assert(state != nullptr);

		auto this_object = JSObject(arguments[0]);
		auto function_object = JSObject(callee);
		if (static_cast<JSValue>(this_object).IsUndefined()) {
			this_object = function_object.get_context().get_global_object();
		}

		const auto position = name_to_setter_map__.find(state->name);
		const auto found = position != name_to_setter_map__.end();
		assert(found);

		if (argumentCount > 1) {
			position->second(this_object, JSValue(arguments[1]));
		}

		return JSObject::GetUndefinedRef();
	}

	template<typename T>
	JsValueRef CALLBACK JSExportCreateNamedFunction(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState) {
		return JSExportClass<T>::CallNamedFunction(callee, arguments, argumentCount, callbackState);
	}

	template<typename T>
	JsValueRef CALLBACK JSExportCreateGetterFunction(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState) {
		return JSExportClass<T>::CallGetterFunction(callee, arguments, argumentCount, callbackState);
	}

	template<typename T>
	JsValueRef CALLBACK JSExportCreateSetterFunction(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState) {
		return JSExportClass<T>::CallSetterFunction(callee, arguments, argumentCount, callbackState);
	}

	template<typename T>
	void CALLBACK JSExportNamedFunctionBeforeCollect(JsRef ref, void* callbackState) {
		const auto state = static_cast<NamedFunctionCallbackState*>(callbackState);
		assert(!state->name.empty());
		state->name.clear();
		delete state;
	}

	template<typename T>
	JsValueRef CALLBACK JSExportCallConstructor(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState) {
		if (isConstructCall) {
			const auto ctor_object_ref = static_cast<JsValueRef>(callbackState);
			const auto callback = JSObject::GetObjectInitializerCallback(ctor_object_ref);
			if (callback) {
				JsValueRef js_object_ref = nullptr;
				callback(ctor_object_ref, isConstructCall, arguments, argumentCount, &js_object_ref);
				assert(js_object_ref);
				return js_object_ref;
			}
		}

		// This means JSExportObject is called as a function.
		return JSObject::GetUndefinedRef();
	}

	template<typename T>
	JSExportInitializeConstructorCallback JSExportClass<T>::GetInitializeConstructorCallback() const {
		static JSExportInitializeConstructorCallback callback;
		static std::once_flag of;
		std::call_once(of, [this]() {
			const auto parent_initialize_ctor_callback = parent_initialize_ctor_callback__;
			const auto initialize_properties_callback = GetInitializePropertiesCallback();
			callback = [=](JsValueRef* ctor_object_ref) {
				JSContext js_context = JSContext(detail::GetContextRef());

				// Create constructor
				if (*ctor_object_ref == nullptr) {
					auto js_export_object_ptr = new T(js_context);
					ASSERT_AND_THROW_JS_ERROR(JsCreateExternalObject(js_export_object_ptr, JSExportFinalize<T>, ctor_object_ref));
					JSObject::RegisterJSExportObject(js_export_object_ptr, *ctor_object_ref);

					JsValueRef ctor_func_ref;
					ASSERT_AND_THROW_JS_ERROR(JsCreateFunction(JSExportCallConstructor<T>, *ctor_object_ref, &ctor_func_ref));

					js_export_object_ptr->set_constructor(ctor_func_ref);
				}

				assert(*ctor_object_ref != nullptr);
				JSObject ctor_object = JSObject(*ctor_object_ref);

				if (parent_initialize_ctor_callback != nullptr) {
					parent_initialize_ctor_callback(ctor_object_ref);
				}

				for (const auto pair : name_to_function_map__) {
					const auto function_name = pair.first;

					const auto callbackState = new NamedFunctionCallbackState();
					callbackState->name = function_name;

					JsValueRef js_function_ref;
					const auto js_name = static_cast<JsValueRef>(JSString(function_name));
					ASSERT_AND_THROW_JS_ERROR(JsCreateNamedFunction(js_name, JSExportCreateNamedFunction<T>, callbackState, &js_function_ref));

					auto js_function = JSObject(js_function_ref);
					ctor_object.SetProperty(function_name, js_function);

					// We save the constructor object so that we call it as static function.
					js_function.SetProperty("__C", ctor_object);

					ASSERT_AND_THROW_JS_ERROR(JsSetObjectBeforeCollectCallback(js_function_ref, callbackState, JSExportNamedFunctionBeforeCollect<T>));
				}

				for (const auto pair : name_to_getter_map__) {
					const auto property_name = pair.first;

					// get + capitalized property name
					std::string getter_name = "get" + property_name;
					getter_name[3] = toupper(getter_name[3]);

					// if there is a function name that is already defined, there we just use different name.
					if (name_to_function_map__.find(getter_name) != name_to_function_map__.end()) {
						getter_name = "__" + getter_name;
					}

					const auto callbackState = new NamedFunctionCallbackState();
					callbackState->name = property_name;

					// define getter function
					JsValueRef js_function_ref;
					const auto js_name = static_cast<JsValueRef>(JSString(getter_name));
					ASSERT_AND_THROW_JS_ERROR(JsCreateNamedFunction(js_name, JSExportCreateGetterFunction<T>, callbackState, &js_function_ref));
					ctor_object.SetProperty(getter_name, JSValue(js_function_ref));

					ASSERT_AND_THROW_JS_ERROR(JsSetObjectBeforeCollectCallback(js_function_ref, callbackState, JSExportNamedFunctionBeforeCollect<T>));
				}

				for (const auto pair : name_to_setter_map__) {
					const auto property_name = pair.first;

					// set + capitalized property name
					std::string setter_name = "set" + property_name;
					setter_name[3] = toupper(setter_name[3]);

					// if there is a function name that is already defined, there we just use different name.
					if (name_to_function_map__.find(setter_name) != name_to_function_map__.end()) {
						setter_name = "__" + setter_name;
					}

					const auto callbackState = new NamedFunctionCallbackState();
					callbackState->name = property_name;

					// define setter function
					JsValueRef js_function_ref;
					const auto js_name = static_cast<JsValueRef>(JSString(setter_name));
					ASSERT_AND_THROW_JS_ERROR(JsCreateNamedFunction(js_name, JSExportCreateSetterFunction<T>, callbackState, &js_function_ref));
					ctor_object.SetProperty(setter_name, JSValue(js_function_ref));

					ASSERT_AND_THROW_JS_ERROR(JsSetObjectBeforeCollectCallback(js_function_ref, callbackState, JSExportNamedFunctionBeforeCollect<T>));
				}

				initialize_properties_callback(ctor_object_ref);
			};
		});
		return callback;
	}

	template<typename T>
	void CALLBACK JSExportFinalize(void *data) {
		const auto js_export_object_ptr = static_cast<T*>(data);
		JSObject::UnregisterJSExportObject(js_export_object_ptr);
		delete js_export_object_ptr;
	}

	template<typename T>
	JSExportInitializePropertiesCallback JSExportClass<T>::GetInitializePropertiesCallback() const {
		static JSExportInitializePropertiesCallback callback;
		static std::once_flag of;
		std::call_once(of, [this]() {
			const auto parent_initialize_properties_callback = parent_initialize_properties_callback__;
			callback = [=](JsValueRef* this_object_ptr) {
				if (parent_initialize_properties_callback) {
					parent_initialize_properties_callback(this_object_ptr);
				}

				const auto js_context = JSContext(detail::GetContextRef());
				auto this_object = JSObject(*this_object_ptr);

				// properties
				for (const auto pair : name_to_getter_map__) {
					const auto property_name = pair.first;

					if (this_object.HasProperty(property_name)) {
						continue;
					}

					// get + capitalized property name
					std::string getter_name = "get" + property_name;
					getter_name[3] = toupper(getter_name[3]);

					// if there is a function name that is already defined, there we just use different name.
					if (name_to_function_map__.find(getter_name) != name_to_function_map__.end()) {
						getter_name = "__" + getter_name;
					}

					const auto setter_found = name_to_setter_map__.find(property_name) != name_to_setter_map__.end();

					auto property_descriptor = js_context.CreateObject();
					const auto property_descriptor_ref = static_cast<JsValueRef>(property_descriptor);

					property_descriptor.SetProperty("enumerable", js_context.CreateBoolean(true));

					assert(this_object.HasProperty(getter_name));
					property_descriptor.SetProperty("get", this_object.GetProperty(getter_name));

					if (setter_found) {
						std::string setter_name = "set" + property_name;
						setter_name[3] = toupper(setter_name[3]);

						// if there is a function name that is already defined, there we just use different name.
						if (name_to_function_map__.find(setter_name) != name_to_function_map__.end()) {
							setter_name = "__" + setter_name;
						}

						assert(this_object.HasProperty(setter_name));
						property_descriptor.SetProperty("set", this_object.GetProperty(setter_name));
					} else {
						property_descriptor.SetProperty("configurable", js_context.CreateBoolean(false));
					}

					bool is_defined;
					ASSERT_AND_THROW_JS_ERROR(JsDefineProperty(*this_object_ptr, JSObject::GetJsPropertyIdRef(property_name), property_descriptor_ref, &is_defined));
					assert(is_defined == true);
				}
			};
		});
		return callback;
	}

	template<typename T>
	JSExportConstructObjectCallback JSExportClass<T>::GetConstructObjectCallback() const {
		static JSExportConstructObjectCallback callback;
		static std::once_flag of;
		std::call_once(of, [this]() {
			const auto initialize_properties_callback = GetInitializePropertiesCallback();
			callback = [=](JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, JsValueRef* this_object_ref) {
				assert(isConstructCall);

				JsValueRef js_context_ref;
				ASSERT_AND_THROW_JS_ERROR(JsGetCurrentContext(&js_context_ref));

				const auto js_context = JSContext(js_context_ref);
				auto js_export_object_ptr = new T(js_context);
				ASSERT_AND_THROW_JS_ERROR(JsCreateExternalObject(js_export_object_ptr, JSExportFinalize<T>, this_object_ref));
				ASSERT_AND_THROW_JS_ERROR(JsSetPrototype(*this_object_ref, callee));

				JSObject::RegisterJSExportObject(js_export_object_ptr, *this_object_ref);

				const auto js_arguments = detail::to_arguments(arguments, argumentCount);
				auto this_object = JSObject(*this_object_ref);

				js_export_object_ptr->postInitialize(this_object);
				js_export_object_ptr->postCallAsConstructor(js_context, js_arguments);
			};
		});
		return callback;
	}

	template<typename T>
	std::unordered_map<std::string, JSObjectCallAsFunctionCallback> JSExportClass<T>::name_to_function_map__;

	template<typename T>
	std::unordered_map<std::string, JSObjectGetPropertyCallback> JSExportClass<T>::name_to_getter_map__;

	template<typename T>
	std::unordered_map<std::string, JSObjectSetPropertyCallback> JSExportClass<T>::name_to_setter_map__;

	template<typename T>
	std::unordered_map<std::string, JsValueRef> JSExportClass<T>::name_to_constant_map__;

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
	void JSExportClass<T>::SetParent(const JSClass& js_class) {
		parent_initialize_ctor_callback__ = js_class.GetInitializeConstructorCallback();
		parent_initialize_properties_callback__ = js_class.GetInitializePropertiesCallback();
	};

} // namespace HAL {

#endif // _HAL_JSCLASS_HPP_
