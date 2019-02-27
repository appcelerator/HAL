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
		: js_context__(js_context) {
		// NOTHING TO DO
	}

	void JSExportObject::postInitialize(JSObject& js_object) {
		// NOTHING TO DO
	}

	void JSExportObject::postCallAsConstructor(const JSContext&, const std::vector<JSValue>&) {
		// NOTHING TO DO
	}

	void JSExportObject::JSExportInitialize() {
		// NOTHING TO DO
	}

	JSObject JSExportObject::get_object() HAL_NOEXCEPT {
		return JSObject::GetObject(this);
	}
} // namespace HAL {
