/**
 * HAL
 *
 * Copyright (c) 2019 by Axway. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _HAL_DETAIL_JSPROPERTYNAMEARRAY_HPP_
#define _HAL_DETAIL_JSPROPERTYNAMEARRAY_HPP_

#include "HAL/detail/JSBase.hpp"
#include <vector>

namespace HAL {

	class JSString;
	class JSObject;

	/*!
	 @class

	 @discussion A JSPropertyNameArray is an RAII wrapper around a
	 JSPropertyNameArrayRef, the JavaScriptCore C API representation of
	 an array of JavaScript property names.
	 */
	class HAL_EXPORT JSPropertyNameArray {

	public:

		/*!
		 @method

		 @abstract Return the number of items in this JavaScript property
		 name array.

		 @result The number of names in this JavaScript property name
		 array.
		 */
		std::size_t GetCount() const HAL_NOEXCEPT;

		/*!
		 @method

		 @abstract Gets a property name at a given index in this
		 JavaScript property name array.

		 @param index The index of the property name to retrieve.

		 @result A JSString containing the property name.
		 */
		JSString GetNameAtIndex(std::size_t index) const HAL_NOEXCEPT;

		/*!
		 @method

		 @abstract Convert this JSPropertyNameArray into a
		 std::vector<JSString>.

		 @result A std::vector<JSString> containing the property names in
		 this JSPropertyNameArray.
		 */
		operator std::vector<JSString>() const HAL_NOEXCEPT;

		JSPropertyNameArray() = delete;;
		~JSPropertyNameArray()                              HAL_NOEXCEPT;
		JSPropertyNameArray(const JSPropertyNameArray&)     HAL_NOEXCEPT;
		JSPropertyNameArray(JSPropertyNameArray&&)          HAL_NOEXCEPT;
		JSPropertyNameArray& operator=(JSPropertyNameArray) HAL_NOEXCEPT;
		void swap(JSPropertyNameArray&)                     HAL_NOEXCEPT;

	private:

		// Only a JSObject can create a JSPropertyNameArray.
		friend class JSObject;

		/*!
		 @method

		 @abstract Gets the names of an object's enumerable properties.

		 @param object The object whose property names you want to get.

		 @result A JSPropertyNameArray containing the names of the
		 object's enumerable properties.
		 */
		explicit JSPropertyNameArray(const JSObject& js_object) HAL_NOEXCEPT;

		// For interoperability with the JavaScriptCore C API.
		explicit operator JSPropertyNameArrayRef() const HAL_NOEXCEPT {
			return js_property_name_array_ref__;
		}

		// Prevent heap based objects.
		static void * operator new(std::size_t);       // #1: To prevent allocation of scalar objects
		static void * operator new[](std::size_t);   // #2: To prevent allocation of array of objects

		// Silence 4251 on Windows since private member variables do not
		// need to be exported from a DLL.
#pragma warning(push)
#pragma warning(disable: 4251)
		JSPropertyNameArrayRef js_property_name_array_ref__;
#pragma warning(pop)

	};

	inline
		void swap(JSPropertyNameArray& first, JSPropertyNameArray& second) HAL_NOEXCEPT {
		first.swap(second);
	}

} // namespace HAL {

#endif // _HAL_DETAIL_JSPROPERTYNAMEARRAY_HPP_
