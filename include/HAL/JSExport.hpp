/**
* HAL
*
* Copyright (c) 2019 by Axway. All Rights Reserved.
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
	class JSExport {

	public:
		static JSExportClass<T> Class();
		virtual ~JSExport() HAL_NOEXCEPT {}

	protected:
		static JSExportClass<T> js_class__;
		static void AddFunctionProperty(const std::string& name, CallNamedFunctionCallback<T> callback);
		static void AddValueProperty(const std::string& name, GetNamedValuePropertyCallback<T> getter, SetNamedValuePropertyCallback<T> setter = nullptr, bool enumerable = true);
		static void AddConstantProperty(const std::string& name, GetNamedValuePropertyCallback<T> getter);
		static void AddHasPropertyCallback(HasPropertyCallback<T> callback);
		static void AddGetPropertyCallback(GetPropertyCallback<T> callback);
		static void AddSetPropertyCallback(SetPropertyCallback<T> callback);
		static void SetParent(const JSClass& js_class);
		static void SetClassVersion(const std::uint32_t& class_version);
	};

	template<typename T>
	JSExportClass<T> JSExport<T>::js_class__;

	template<typename T>
	void JSExport<T>::AddFunctionProperty(const std::string& name, CallNamedFunctionCallback<T> callback) {
		js_class__.AddFunctionProperty(name, callback);
	}

	template<typename T>
	void JSExport<T>::AddValueProperty(const std::string& name, GetNamedValuePropertyCallback<T> getter, SetNamedValuePropertyCallback<T> setter, bool enumerable) {
		js_class__.AddValueProperty(name, getter, setter, enumerable);
	}

	template<typename T>
	void JSExport<T>::AddConstantProperty(const std::string& name, GetNamedValuePropertyCallback<T> getter) {
		js_class__.AddConstantProperty(name, getter);
	}

	template<typename T>
	void JSExport<T>::AddHasPropertyCallback(HasPropertyCallback<T> callback) {
		js_class__.AddHasPropertyCallback(callback);
	}

	template<typename T>
	void JSExport<T>::AddGetPropertyCallback(GetPropertyCallback<T> callback) {
		js_class__.AddGetPropertyCallback(callback);
	}

	template<typename T>
	void JSExport<T>::AddSetPropertyCallback(SetPropertyCallback<T> callback) {
		js_class__.AddSetPropertyCallback(callback);
	}

	template<typename T>
	JSExportClass<T> JSExport<T>::Class() {
		static std::once_flag of;
		std::call_once(of, []() {
			T::JSExportInitialize();
			js_class__.InitializePropertyCallbacks();
		});
		return js_class__;
	}

	template<typename T>
	void JSExport<T>::SetParent(const JSClass& js_class) {
		js_class__.SetParent(js_class);
	}

	template<typename T>
	void JSExport<T>::SetClassVersion(const std::uint32_t& class_version) {
		js_class__.SetClassVersion(class_version);
	}

} // namespace HAL {

#endif // _HAL_JSEXPORT_HPP_
