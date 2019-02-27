/**
 * HAL
 *
 * Copyright (c) 2018 by Axway. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _HAL_JSERROR_HPP_
#define _HAL_JSERROR_HPP_

#include "HAL/JSObject.hpp"
#include <deque> 

namespace HAL {

/*!
  @class
  
  @discussion A JavaScript object of the Error type.

  The only way to create a JSError is by using the
  JSContext::CreateError member function.
*/
class HAL_EXPORT JSError final : public JSObject {
 public:
#pragma warning(push)
#pragma warning(disable : 4251)
 	static std::deque<std::string> NativeStack__;
 	static std::string GetNativeStack();
 	static void ClearNativeStack();
#pragma warning(pop)

 	std::string message() const;
 	std::string name() const;
 	std::string filename() const;
 	std::uint32_t linenumber() const;
 	std::string stack() const;
 	std::string nativeStack() const;
	
 private:
	
	// Only JSContext and JSObject can create a JSError.
	friend JSContext;
	friend JSObject;
	
	JSError(JsValueRef js_object_ref);
	JSError(JsValueRef js_object_ref, const std::vector<JSValue>& arguments);

};

} // namespace HAL {

#endif // _HAL_JSERROR_HPP_
