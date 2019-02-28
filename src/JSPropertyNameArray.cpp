/**
 * HAL
 *
 * Copyright (c) 2019 by Axway. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "HAL/JSPropertyNameArray.hpp"
#include "HAL/JSString.hpp"
#include "HAL/JSObject.hpp"

#include <algorithm>

namespace HAL {

	std::size_t JSPropertyNameArray::GetCount() const HAL_NOEXCEPT {
		return JSPropertyNameArrayGetCount(js_property_name_array_ref__);
	}

	JSString JSPropertyNameArray::GetNameAtIndex(std::size_t index) const HAL_NOEXCEPT {
		return JSString(JSPropertyNameArrayGetNameAtIndex(js_property_name_array_ref__, index));
	}

	JSPropertyNameArray::operator std::vector<JSString>() const HAL_NOEXCEPT {
		std::vector<JSString> property_names;
		for (std::size_t i = 0, count = GetCount(); i < count; ++i) {
			property_names.emplace_back(GetNameAtIndex(i));
		}

		return property_names;
	}

	JSPropertyNameArray::~JSPropertyNameArray() HAL_NOEXCEPT {
		JSPropertyNameArrayRelease(js_property_name_array_ref__);
	}

	JSPropertyNameArray::JSPropertyNameArray(const JSPropertyNameArray& rhs) HAL_NOEXCEPT
		: js_property_name_array_ref__(rhs.js_property_name_array_ref__) {
		JSPropertyNameArrayRetain(js_property_name_array_ref__);
	}

	JSPropertyNameArray::JSPropertyNameArray(JSPropertyNameArray&& rhs) HAL_NOEXCEPT
		: js_property_name_array_ref__(rhs.js_property_name_array_ref__) {
		JSPropertyNameArrayRetain(js_property_name_array_ref__);
	}

	JSPropertyNameArray& JSPropertyNameArray::operator=(JSPropertyNameArray rhs) HAL_NOEXCEPT {
		swap(rhs);
		return *this;
	}

	void JSPropertyNameArray::swap(JSPropertyNameArray& other) HAL_NOEXCEPT {
		using std::swap;

		// By swapping the members of two classes, the two classes are
		// effectively swapped.
		swap(js_property_name_array_ref__, other.js_property_name_array_ref__);
	}

	JSPropertyNameArray::JSPropertyNameArray(const JSObject& js_object) HAL_NOEXCEPT
		: js_property_name_array_ref__(JSObjectCopyPropertyNames(static_cast<JSContextRef>(js_object.get_context()), static_cast<JSObjectRef>(js_object))) {
	}

} // namespace HAL {
