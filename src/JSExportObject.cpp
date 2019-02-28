/**
* HAL
*
* Copyright (c) 2018 by Axway. All Rights Reserved.
* Licensed under the terms of the Apache Public License.
* Please see the LICENSE included with this distribution for details.
*/

#include "HAL/JSExportObject.hpp"
#include "HAL/JSExport.hpp"

namespace HAL {
	JSExportObject::JSExportObject(const JSContext& js_context) HAL_NOEXCEPT
		: js_context_ref__(static_cast<JSContextRef>(js_context)) {
	}

	void JSExportObject::postInitialize(JSObject& js_object) {
		js_object_ref__ = static_cast<JSObjectRef>(js_object);
	}

	void JSExportObject::postCallAsConstructor(const JSContext&, const std::vector<JSValue>&) {
		// NOTHING TO DO
	}

	void JSExportObject::JSExportInitialize() {
		// NOTHING TO DO
	}

	JSContext JSExportObject::get_context() const {
		return JSContext(js_context_ref__);
	}

	JSObject JSExportObject::get_object() HAL_NOEXCEPT {
		assert(js_context_ref__ != nullptr);
		assert(js_object_ref__  != nullptr);
		return JSObject(get_context(), js_object_ref__);
	}
} // namespace HAL {
