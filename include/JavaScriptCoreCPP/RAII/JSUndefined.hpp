/**
 * JavaScriptCoreCPP
 * Author: Matthew D. Langston
 *
 * Copyright (c) 2014 by Appcelerator, Inc. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _TITANIUM_MOBILE_WINDOWS_JAVASCRIPTCORECPP_RAII_JSUNDEFINED_HPP_
#define _TITANIUM_MOBILE_WINDOWS_JAVASCRIPTCORECPP_RAII_JSUNDEFINED_HPP_

#include "JavaScriptCoreCPP/RAII/JSValue.hpp"

namespace JavaScriptCoreCPP { namespace RAII {

/*!
  @class
  
  @discussion A JavaScript value of the undefined type.

  The only way to create a JSUndefined is by using the
  JSContext::CreateUndefined member function.
*/
class JSUndefined final : public JSValue {
	
private:
	
	// Only a JSContext can create a JSUndefined.
	JSUndefined(const JSContext& js_context) : JSValue(js_context, JSValueMakeUndefined(js_context)) {
	}

	friend JSContext;
};

}} // namespace JavaScriptCoreCPP { namespace RAII {

#endif // _TITANIUM_MOBILE_WINDOWS_JAVASCRIPTCORECPP_RAII_JSUNDEFINED_HPP_
