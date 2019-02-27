/**
* HAL
*
* Copyright (c) 2018 by Axway. All Rights Reserved.
* Licensed under the terms of the Apache Public License.
* Please see the LICENSE included with this distribution for details.
*/

#ifndef _HAL_JSEXPORT_HPP_
#define _HAL_JSEXPORT_HPP_

#include "HAL/detail/JSBase.hpp"
#include "HAL/JSContext.hpp"
#include "HAL/JSValue.hpp"
#include "HAL/JSObject.hpp"
#include "HAL/JSClass.hpp"

namespace HAL {

	template<typename T>
	using GetNamedValuePropertyCallback = std::function<JSValue(T&)>;

	template<typename T>
	using SetNamedValuePropertyCallback = std::function<bool(T&, const JSValue&)>;

	template<typename T>
	using CallNamedFunctionCallback = std::function<JSValue(T&, const std::vector<JSValue>&, JSObject&)>;

	template<typename T>
	class JSExport {

	public:
		static JSExportClass<T> Class();
		virtual ~JSExport() HAL_NOEXCEPT {}

	protected:
		static JSExportClass<T> js_class__;
		static void AddFunctionProperty(const std::string& name, CallNamedFunctionCallback<T> callback);
		static void AddValueProperty(const std::string& name, GetNamedValuePropertyCallback<T> getter, SetNamedValuePropertyCallback<T> setter);
		static void AddConstantProperty(const std::string& name, GetNamedValuePropertyCallback<T> getter);
		static void SetParent(const JSClass& js_class);
		static void SetClassVersion(const std::uint32_t& class_version);
	};

	template<typename T>
	JSExportClass<T> JSExport<T>::js_class__;

	template<typename T>
	void JSExport<T>::AddFunctionProperty(const std::string& name, CallNamedFunctionCallback<T> callback) {
		js_class__.AddFunctionProperty(name, [name, callback](JSObject function_object, JSObject this_object, const std::vector<JSValue>& arguments) {
			auto t = this_object.GetPrivate<T>();

			// When there's no private data assigned to this_object, it probablly means this function is assigned to different parent.
			// In that case we try to rescue private data from constructor so that we call it as static function.
			if (t == nullptr && function_object.HasProperty("__C")) {
				t = static_cast<JSObject>(function_object.GetProperty("__C")).GetPrivate<T>();
			}

			const auto js_context = this_object.get_context();
			const auto undefined = js_context.CreateUndefined();

			if (t) {
				try {
					return callback(*t, arguments, this_object);
				} catch (const std::runtime_error& ex) {
					const auto error = js_context.CreateError(ex.what());
					JsSetException(static_cast<JsValueRef>(error));
					return undefined;
				} catch (...) {
					const auto error = js_context.CreateError("Unknown error at " + name);
					JsSetException(static_cast<JsValueRef>(error));
					return undefined;
				}
			}

			// If we still can't find the parent, we just throw an error.
			const auto error = js_context.CreateError("Callback for the function " + name + " is not found.");
			JsSetException(static_cast<JsValueRef>(error));

			return undefined;
		});
	}

	template<typename T>
	void JSExport<T>::AddValueProperty(const std::string& name, GetNamedValuePropertyCallback<T> getter, SetNamedValuePropertyCallback<T> setter) {
		js_class__.AddValueProperty(name,
			[name, getter](JSObject this_object) {

			const auto js_context = this_object.get_context();
			const auto undefined = js_context.CreateUndefined();

			auto t = this_object.GetPrivate<T>();
			if (t && getter) {
				try {
					return getter(*t);
				} catch (const std::runtime_error& ex) {
					const auto error = js_context.CreateError(ex.what());
					JsSetException(static_cast<JsValueRef>(error));
					return undefined;
				} catch (...) {
					const auto error = js_context.CreateError("Unknown error at " + name);
					JsSetException(static_cast<JsValueRef>(error));
					return undefined;
				}
			}
			return undefined;
		},
			[name, setter](JSObject this_object, const JSValue& value) {

			auto t = this_object.GetPrivate<T>();
			if (t && setter) {
				try {
					return setter(*t, value);
				} catch (const std::runtime_error& ex) {
					const auto js_context = this_object.get_context();
					const auto error = js_context.CreateError(ex.what());
					JsSetException(static_cast<JsValueRef>(error));
					return false;
				} catch (...) {
					const auto js_context = this_object.get_context();
					const auto error = js_context.CreateError("Unknown error at " + name);
					JsSetException(static_cast<JsValueRef>(error));
					return false;
				}
			}
			return false;
		}
		);
	}

	template<typename T>
	void JSExport<T>::AddConstantProperty(const std::string& name, GetNamedValuePropertyCallback<T> getter) {
		js_class__.AddConstantProperty(name, [name, getter](JSObject this_object) {

			const auto js_context = this_object.get_context();
			const auto undefined = js_context.CreateUndefined();

			auto t = this_object.GetPrivate<T>();
			if (t && getter) {
				try {
					return getter(*t);
				} catch (const std::runtime_error& ex) {
					const auto error = js_context.CreateError(ex.what());
					JsSetException(static_cast<JsValueRef>(error));
					return undefined;
				} catch (...) {
					const auto error = js_context.CreateError("Unknown error at " + name);
					JsSetException(static_cast<JsValueRef>(error));
					return undefined;
				}
			}
			return undefined;
		});
	}

	template<typename T>
	JSExportClass<T> JSExport<T>::Class() {
		static std::once_flag of;
		std::call_once(of, []() {
			T::JSExportInitialize();
		});
		return js_class__;
	}

	template<typename T>
	void JSExport<T>::SetParent(const JSClass& js_class) {
		js_class__.SetParent(js_class);
	}

	template<typename T>
	void JSExport<T>::SetClassVersion(const std::uint32_t& class_version) {
		// Nothing to do for Chakra
	}

} // namespace HAL {

#endif // _HAL_JSEXPORT_HPP_
