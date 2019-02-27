/**
 * HAL
 *
 * Copyright (c) 2019 by Axway. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "HAL/JSContextGroup.hpp"
#include "HAL/JSContext.hpp"
#include "HAL/JSClass.hpp"

#include <cassert>

namespace HAL {

	JSContextGroup::JSContextGroup() HAL_NOEXCEPT
		: js_context_group_ref__(JSContextGroupCreate()) {
	}

	JSContext JSContextGroup::CreateContext(const JSClass& global_object_class) const HAL_NOEXCEPT {
		return JSContext(JSGlobalContextCreateInGroup(js_context_group_ref__, static_cast<JSClassRef>(global_object_class)));
	}

	JSContextGroup::JSContextGroup(JSContextGroupRef js_context_group_ref) HAL_NOEXCEPT
		: js_context_group_ref__(js_context_group_ref) {

	}

	JSContextGroup::~JSContextGroup() HAL_NOEXCEPT {
		JSContextGroupRelease(js_context_group_ref__);
	}

	JSContextGroup::JSContextGroup(const JSContextGroup& rhs) HAL_NOEXCEPT
		: js_context_group_ref__(rhs.js_context_group_ref__) {
	}

	JSContextGroup::JSContextGroup(JSContextGroup&& rhs) HAL_NOEXCEPT
		: js_context_group_ref__(rhs.js_context_group_ref__) {
	}

	JSContextGroup& JSContextGroup::operator=(JSContextGroup rhs) HAL_NOEXCEPT {
		swap(rhs);
		return *this;
	}

	void JSContextGroup::swap(JSContextGroup& other) HAL_NOEXCEPT {
		using std::swap;

		// By swapping the members of two classes, the two classes are
		// effectively swapped.
		swap(js_context_group_ref__, other.js_context_group_ref__);
	}

} // namespace HAL {
