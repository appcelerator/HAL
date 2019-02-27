/**
 * HAL
 *
 * Copyright (c) 2019 by Axway. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _HAL_JSFUNCTION_HPP_
#define _HAL_JSFUNCTION_HPP_

#include "HAL/JSObject.hpp"
#include <deque> 

namespace HAL {

	/*!
	  @class

	  @discussion A JavaScript object of the Function type.

	  The only way to create a Function is by using the
	  JSContext::CreateFunction member function.
	*/
	class HAL_EXPORT JSFunction final : public JSObject {
	 public:

	 private:

		 // Only JSContext and JSObject can create a JSError.
		 friend JSContext;

		 JSFunction(JSValueRef js_object_ref);

	};

} // namespace HAL {

#endif // _HAL_JSFUNCTION_HPP_
