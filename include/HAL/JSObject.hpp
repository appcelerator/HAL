/**
 * HAL
 *
 * Copyright (c) 2018 by Axway. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _HAL_JSOBJECT_HPP_
#define _HAL_JSOBJECT_HPP_

#include "HAL/detail/JSBase.hpp"
#include "HAL/JSClass.hpp"
#include "HAL/JSContext.hpp"
#include "HAL/JSExportObject.hpp"

#include <memory>
#include <unordered_map>

namespace HAL {
	class JSValue;
	class JSArray;
	class JSError;
	class JSExportObject;
}

namespace HAL {

	/*!
	 @class

	 @discussion A JSObject is an RAII wrapper around a JsValueRef, the
	 JavaScriptCore C API representation of a JavaScript object. This is
	 the base class for all JavaScript objects JSObject:

	 JSFunction
	 JSArray
	 JSDate
	 JSError

	 The only way to create a JSObject is by using the
	 JSContext::CreateObject member functions.
	 */
	class HAL_EXPORT JSObject {

	public:

		/*!
		 @method

		 @abstract Determine whether this JavaScript object has a
		 property.

		 @param property_name The name of the property to set.

		 @result true if this JavaScript object has the property.
		 */
		virtual bool HasProperty(const std::string& property_name) const HAL_NOEXCEPT final;

		/*!
		 @method

		 @abstract Return a property of this JavaScript object.

		 @param property_name The name of the property to get.

		 @result The property's value if this JavaScript object has the
		 property, otherwise JSUndefined.

		 @throws std::runtime_error if getting the property threw a
		 JavaScript exception.
		 */
		virtual JSValue GetProperty(const std::string& property_name) const final;

		/*!
		 @method

		 @abstract Return a property of this JavaScript object by numeric
		 index. This method is equivalent to calling GetProperty with a
		 string containing the numeric index, but provides optimized
		 access for numeric properties.

		 @param property_index An integer value that is the property's
		 name.

		 @result The property's value if this JavaScript object has the
		 property, otherwise JSUndefined.

		 @throws std::runtime_error if getting the property threw a
		 JavaScript exception.
		 */
		virtual JSValue GetProperty(unsigned property_index) const final;

		/*!
		 @method

		 @abstract Set a property on this JavaScript object with an
		 optional set of attributes.

		 @param property_name The name of the property to set.

		 @param value The value of the the property to set.

		 @param attributes An optional set of property attributes to give
		 to the property.

		 @result true if the the property was set.

		 @throws std::runtime_error if setting the property threw a
		 JavaScript exception.
		 */
		virtual void SetProperty(const std::string& property_name, const JSValue& property_value) final;

		/*!
		 @method

		 @abstract Set a property on this JavaScript object by numeric
		 index. This method is equivalent to calling SetProperty with a
		 string containing the numeric index, but provides optimized
		 access for numeric properties.

		 @param property_index An integer value that is the property's
		 name.

		 @param value The value of the the property to set.

		 @throws std::runtime_error if setting the property threw a
		 JavaScript exception.
		 */
		virtual void SetProperty(unsigned property_index, const JSValue& property_value) final;

		/*!
		 @method

		 @abstract Delete a property from this JavaScript object.

		 @param property_name The name of the property to delete.

		 @result true if the property was deleted.

		 @throws std::runtime_error if deleting the property threw a
		 JavaScript exception.
		 */
		virtual bool DeleteProperty(const std::string& property_name) final;

		/*!
		 @method

		 @abstract Return the names of this JavaScript object's enumerable
		 properties.

		 @result A JSPropertyNameArray containing the names object's
		 enumerable properties.
		 */
		virtual JSArray GetPropertyNames() const HAL_NOEXCEPT final;

		/*!
		 @method

		 @abstract Return the set of this JavaScript object's enumerable properties.

		 @result A unordered_map containing the names and values of object's enumerable properties.
		 */
		virtual std::unordered_map<std::string, JSValue> GetProperties() const HAL_NOEXCEPT final;


		/*!
		 @method

		 @abstract Determine whether this object can be called as a
		 function.

		 @result true if this object can be called as a function.
		 */
		virtual bool IsFunction() const HAL_NOEXCEPT final;

		/*!
		 @method

		 @abstract Determine whether this JavaScript object is an
		 Array.

		 @result true if this JavaScript object is an Array.
		 */
		virtual bool IsArray() const HAL_NOEXCEPT final;

		/*!
		 @method

		 @abstract Determine whether this JavaScript object is an
		 Error.

		 @result true if this JavaScript object is an Error.
		 */
		virtual bool IsError() const HAL_NOEXCEPT final;

		/*!
		 @method

		 @abstract Call this JavaScript object as a function. A
		 std::runtime_error exception is thrown if this JavaScript object
		 can't be called as a function.

		 @discussion In the JavaScript expression 'myObject.myFunction()',
		 the 'this_object' parameter will be set to 'myObject' and this
		 JavaScript object is 'myFunction'.

		 @param arguments Optional JSValue argument(s) to pass to the
		 function.

		 @param this_object An optional JavaScript object to use as
		 'this'. The default value is the global object.

		 @result Return the function's return value.

		 @throws std::runtime_error if either this JavaScript object can't
		 be called as a function, or calling the function itself threw a
		 JavaScript exception.
		 */

		virtual JSValue operator()(JSObject this_object) final;
		virtual JSValue operator()(JSValue&                     argument, JSObject this_object) final;
		virtual JSValue operator()(const std::vector<JSValue>&  arguments, JSObject this_object) final;

		/*!
		 @method

		 @abstract Determine whether this object can be called as a
		 constructor.

		 @result true if this object can be called as a constructor.
		 */
		virtual bool IsConstructor() const HAL_NOEXCEPT final;

		/*!
		 @method

		 @abstract Call this JavaScript object as a constructor as if in a
		 'new' expression.

		 @param arguments Optional JSValue argument(s) to pass to the
		 constructor.

		 @result The JavaScript object of the constructor's return value.

		 @throws std::runtime_error if either this JavaScript object can't
		 be called as a constructor, or calling the constructor itself
		 threw a JavaScript exception.
		 */
		virtual JSObject CallAsConstructor() final;
		virtual JSObject CallAsConstructor(const JSValue&               argument) final;
		virtual JSObject CallAsConstructor(const std::vector<JSValue>&  arguments) final;

		/*!
		 @method

		 @abstract Return the execution context of this JavaScript value.

		 @result The the execution context of this JavaScript value.
		 */
		virtual JSContext get_context() const HAL_NOEXCEPT final {
			return JSContext(detail::GetContextRef());
		}

		/*!
		 @method

		 @abstract Convert this JSObject to a JSValue.

		 @result A JSValue with the result of conversion.
		 */
		virtual operator JSValue() const final;

		/*!
		 @method

		 @abstract Convert this JSObject to a JSArray.

		 @result A JSArray with the result of conversion.
		 */
		virtual operator JSArray() const final;

		/*!
		 @method

		 @abstract Convert this JSObject to a JSError

		 @result A JSError with the result of conversion.
		 */
		virtual operator JSError() const final;

		/*!
		 @method

		 @abstract Return a std::shared_ptr<T> to this object's private
		 data.

		 @result A std::shared_ptr<T> to this object's private data if the
		 object has private data of type T*, otherwise nullptr.
		 */
		template<typename T>
		std::shared_ptr<T> GetPrivate() const HAL_NOEXCEPT;

		virtual ~JSObject()            HAL_NOEXCEPT;
		JSObject(const JSObject&)      HAL_NOEXCEPT;
		JSObject(JSObject&&)           HAL_NOEXCEPT;
		JSObject& operator=(JSObject);
		void swap(JSObject&)           HAL_NOEXCEPT;

		// For interoperability with the JavaScriptCore C API.
		JSObject(JsValueRef js_object_ref);

		// For interoperability with the JavaScriptCore C API.
		explicit operator JsValueRef() const HAL_NOEXCEPT {
			return js_object_ref__;
		}

		static JsValueRef GetUndefinedRef() {
			JsValueRef undefinedValue;
			JsGetUndefinedValue(&undefinedValue);
			return undefinedValue;
		}

		static JSObject GetObject(const JSExportObject* jsexport_ptr);
		static void RegisterJSExportObject(const JSExportObject* jsexport_ptr, const JsValueRef);
		static void UnregisterJSExportObject(const JSExportObject* jsexport_ptr);

		static JSExportConstructObjectCallback GetObjectInitializerCallback(const JsValueRef js_ctor_ref);
		static void RemoveObjectConstructorCallback(const JsValueRef js_ctor_ref);

		static JsPropertyIdRef GetJsPropertyIdRef(const std::string& name) HAL_NOEXCEPT;

	protected:

		// Prevent heap based objects.
		void* operator new(std::size_t) = delete;   // #1: To prevent allocation of scalar objects
		void* operator new[](std::size_t) = delete; // #2: To prevent allocation of array of objects

		/*!
		 @method

		 @abstract Call this JavaScript object as a function.

		 @discussion In the JavaScript expression 'myObject.myFunction()',
		 the 'this_object' parameter will be set to 'myObject' and this
		 JavaScript object is 'myFunction'.

		 @param arguments Optional JSValue argument(s) to pass to the
		 function.

		 @param this_object The JavaScript object to use as 'this'.

		 @result Return the function's return value.

		 @throws std::runtime_error if either this JavaScript object can't
		 be called as a function, or calling the function itself threw a
		 JavaScript exception.
		 */
		virtual JSValue CallAsFunction(const std::vector<JSValue>&  arguments, JSObject this_object);

		/*!
		 @method

		 @abstract Gets this object's private data.

		 @result A void* that is this object's private data, if the object
		 has private data, otherwise nullptr.
		 */
		virtual void* GetPrivate() const HAL_NOEXCEPT final;

		/*!
		 @method

		 @abstract Set this object's private data. Note that you must use
		 the JSObject constructor taking a custom JSClass in order to use
		 this method because the default JSObject constructor does not
		 allocate storage for private data.

		 @param data A void* to set as this object's private data.

		 @result true if this object can store private data.
		 */
		virtual bool SetPrivate(void* data) const HAL_NOEXCEPT final;

		// JSContext (and already friended JSExportClass) use the
		// following constructor.
		friend class JSContext;

		JSObject(const JSClass& js_class);

		// Silence 4251 on Windows since private member variables do not
		// need to be exported from a DLL.
#pragma warning(push)
#pragma warning(disable: 4251)
		JsValueRef js_object_ref__{ nullptr };
		static std::unordered_map<std::uintptr_t, const JsValueRef> js_private_data_to_js_object_ref_map__;
		static std::unordered_map<std::uintptr_t, JSExportConstructObjectCallback> js_ctor_ref_to_constructor_map__;
		static std::unordered_map<std::string, const JsPropertyIdRef> name_to_property_id_map__;
#pragma warning(pop)

	};

	inline
		void swap(JSObject& first, JSObject& second) HAL_NOEXCEPT {
		first.swap(second);
	}

	template<typename T>
	std::shared_ptr<T> JSObject::GetPrivate() const HAL_NOEXCEPT {
		const auto private_ptr = GetPrivate();
		if (private_ptr) {
			return std::shared_ptr<T>(std::make_shared<JSObject>(*this), dynamic_cast<T*>(static_cast<JSExportObject*>(private_ptr)));
		}

		// When objects can't have private data (like global object and functions),
		// it might have __C property that holds private object.
		if (HasProperty("__C")) {
			const auto ctor_ptr = static_cast<JSObject>(GetProperty("__C")).GetPrivate<T>();
			if (ctor_ptr) {
				return ctor_ptr;
			}
		}
		return nullptr;
	}

} // namespace HAL {

#endif // _HAL_JSOBJECT_HPP_
