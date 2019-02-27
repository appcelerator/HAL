/**
 * HAL
 *
 * Copyright (c) 2018 by Axway. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "HAL/JSContextGroup.hpp"
#include "HAL/JSContext.hpp"
#include "HAL/JSClass.hpp"
#include "HAL/JSObject.hpp"
#include "HAL/JSArray.hpp"
#include "HAL/detail/JSUtil.hpp"

#include <cassert>

namespace HAL {
  
  JSContextGroup::JSContextGroup() HAL_NOEXCEPT {
	  ASSERT_AND_THROW_JS_ERROR(JsCreateRuntime(JsRuntimeAttributeNone, nullptr, &js_runtime_handle__));
  }

  JSContext JSContextGroup::CreateContext() const HAL_NOEXCEPT {
	  return CreateContext(JSClass());
  }

  JSContext JSContextGroup::CreateContext(const JSClass& js_class) const HAL_NOEXCEPT {
	JsContextRef context;
	  
	// HAL assumes only one context at a time.
	ASSERT_AND_THROW_JS_ERROR(JsGetCurrentContext(&context));
	assert(context == nullptr);

	ASSERT_AND_THROW_JS_ERROR(JsCreateContext(js_runtime_handle__, &context));
	ASSERT_AND_THROW_JS_ERROR(JsSetCurrentContext(context));

	const auto js_context = JSContext(context);
	const auto baseObject = js_context.CreateObject(js_class);
	auto globalObject = js_context.get_global_object();

	const auto properties = baseObject.GetProperties();
	for (const auto pair : properties) {
		globalObject.SetProperty(pair.first, pair.second);
	}

	// Retain base object of global object
	globalObject.SetProperty("__C", baseObject);

	return js_context;
  }
  
  JSContextGroup::JSContextGroup(JsRuntimeHandle js_runtime_handle) HAL_NOEXCEPT
  : js_runtime_handle__(js_runtime_handle) {

  }
  
  JSContextGroup::~JSContextGroup() HAL_NOEXCEPT {
	  JsContextRef context;
	  ASSERT_AND_THROW_JS_ERROR(JsGetCurrentContext(&context));
	  assert(context != nullptr);

	  ASSERT_AND_THROW_JS_ERROR(JsSetCurrentContext(nullptr));
	  ASSERT_AND_THROW_JS_ERROR(JsDisposeRuntime(js_runtime_handle__));
  }
  
  JSContextGroup::JSContextGroup(const JSContextGroup& rhs) HAL_NOEXCEPT
  : js_runtime_handle__(rhs.js_runtime_handle__) {
  }
  
  JSContextGroup::JSContextGroup(JSContextGroup&& rhs) HAL_NOEXCEPT
  : js_runtime_handle__(rhs.js_runtime_handle__) {
  }
  
  JSContextGroup& JSContextGroup::operator=(JSContextGroup rhs) HAL_NOEXCEPT {
    swap(rhs);
    return *this;
  }
  
  void JSContextGroup::swap(JSContextGroup& other) HAL_NOEXCEPT {
    using std::swap;
    
    // By swapping the members of two classes, the two classes are
    // effectively swapped.
    swap(js_runtime_handle__, other.js_runtime_handle__);
  }
  
} // namespace HAL {
