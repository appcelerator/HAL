/**
 * HAL
 *
 * Copyright (c) 2019 by Axway. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "HAL/JSClass.hpp"
#include <cassert>

namespace HAL {
	JSClass::JSClass() HAL_NOEXCEPT
		: js_class_ref__(JSClassCreate(&kJSClassDefinitionEmpty))
		, js_class_definition__(kJSClassDefinitionEmpty) {
	};
	JSClass::~JSClass()          HAL_NOEXCEPT
	{
		JSClassRelease(js_class_ref__);
	}
	JSClass::JSClass(const JSClass& rhs) HAL_NOEXCEPT
		: js_class_ref__(rhs.js_class_ref__)
		, static_values__(rhs.static_values__)
		, static_functions__(rhs.static_functions__) {
		Copy(rhs.js_class_definition__);
		JSClassRetain(js_class_ref__);
	}
	JSClass::JSClass(JSClass&& rhs) HAL_NOEXCEPT
		: js_class_ref__(rhs.js_class_ref__)
		, static_values__(rhs.static_values__)
		, static_functions__(rhs.static_functions__) {
		Copy(rhs.js_class_definition__);
		JSClassRetain(js_class_ref__);
	}

	JSClass& JSClass::operator=(JSClass rhs) HAL_NOEXCEPT {
		Copy(rhs.js_class_definition__);
		static_values__    = rhs.static_values__;
		static_functions__ = rhs.static_functions__;
		js_class_ref__     = rhs.js_class_ref__;
		JSClassRetain(js_class_ref__);
		return *this;
	}

	void JSClass::Copy(const JSClassDefinition& other) {
		js_class_definition__.version = other.version;
		js_class_definition__.attributes = other.attributes;

		js_class_definition__.className = other.className;
		js_class_definition__.parentClass = other.parentClass;

		js_class_definition__.initialize = other.initialize;
		js_class_definition__.finalize = other.finalize;
		js_class_definition__.hasProperty = other.hasProperty;
		js_class_definition__.getProperty = other.getProperty;
		js_class_definition__.setProperty = other.setProperty;
		js_class_definition__.deleteProperty = other.deleteProperty;
		js_class_definition__.getPropertyNames = other.getPropertyNames;
		js_class_definition__.callAsFunction = other.callAsFunction;
		js_class_definition__.callAsConstructor = other.callAsConstructor;
		js_class_definition__.hasInstance = other.hasInstance;
		js_class_definition__.convertToType = other.convertToType;
		js_class_definition__.staticValues = other.staticValues;
		js_class_definition__.staticFunctions = other.staticFunctions;
	}

}
