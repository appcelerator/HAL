/**
 * JavaScriptCoreCPP
 * Author: Matthew D. Langston
 *
 * Copyright (c) 2014 by Appcelerator, Inc. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _JAVASCRIPTCORECPP_JSNUMBER_HPP_
#define _JAVASCRIPTCORECPP_JSNUMBER_HPP_

#include "JavaScriptCoreCPP/JSValue.hpp"
#include <cstdint>
#include <stdexcept>

namespace JavaScriptCoreCPP {

/*!
  @class
  
  @discussion A JavaScript value of the number type.

  The only way to create a JSNumber is by using the
  JSContext::CreateNumber member function.
*/
#ifdef JAVASCRIPTCORECPP_PERFORMANCE_COUNTER_ENABLE
class JSNumber final : public JSValue, public detail::JSPerformanceCounter<JSNumber> {
#else
class JSNumber final : public JSValue {
#endif
	
public:
	
	/*!
    @method
    
    @abstract Assign a double to this JSNumber.
    
    @param number The double to assign to this JSNumber
    
    @result The JSNumber with a new value of the given number.
  */
	JSNumber& operator=(double number) {
		return operator=(JSNumber(get_context(), number));
  }

	/*!
    @method
    
    @abstract Assign an int32_t to this JSNumber.
    
    @param number The int32_t to assign to this JSNumber
    
    @result The JSNumber with the new value of the given number.
  */
	JSNumber& operator=(int32_t number) {
		return operator=(JSNumber(get_context(), number));
  }

	/*!
    @method
    
    @abstract Assign an uint32_t to this JSNumber.
    
    @param number The uint32_t to assign to this JSNumber
    
    @result The JSNumber with the new value of the given number.
  */
	JSNumber& operator=(uint32_t number) {
		return operator=(JSNumber(get_context(), number));
  }

	/*!
    @method
    
    @abstract Convert the given JavaScript value to a JavaScript
    number and assign it to this JSNumber.
    
    @param js_value The JavaScript value to assign to this JSNumber
    
    @result The JSNumber with the new value of the given JavaScript
    value.
    
	  @throws std::invalid_argument if the given JavaScript value could
	  not be converted to a JavaScript number.
	*/
	JSNumber& operator=(const JSValue& js_value) {
		//JSValue::operator=(JSNumber(js_value));
		JSValue::operator JSNumber();
		return *this;
  }

	/*!
	  @method
	  
	  @abstract Convert a JSNumber to a double.
	  
	  @result The double result of conversion.
	*/
	operator double() const;
	
	/*!
	  @method
	  
	  @abstract Convert a JSNumber to an int32_t according to the rules
	  specified by the JavaScript language.
	  
	  @result The int32_t result of conversion.
	*/
	operator int32_t() const;
	
	/*!
	  @method
	  
	  @abstract Convert a JSNumber to an uint32_t according to the rules
	  specified by the JavaScript language.
	  
	  @discussion The JSNumber is converted to an uint32_t according to
	  the rules specified by the JavaScript language (implements
	  ToUInt32, defined in ECMA-262 9.6).
	  
	  @result The uint32_t result of the conversion.
	*/
	operator uint32_t() const  {
		// As commented in the spec, the operation of ToInt32 and ToUint32
		// only differ in how the result is interpreted; see NOTEs in
		// sections 9.5 and 9.6.
		return operator int32_t();
	}

 private:
	
	JSNumber(const JSContext& js_context) : JSNumber(js_context, 0) {
	}
	
	JSNumber(const JSContext& js_context, double number) : JSValue(js_context, JSValueMakeNumber(js_context, number)) {
	}
	
	JSNumber(const JSContext& js_context, int32_t number) : JSNumber(js_context, static_cast<double>(number)) {
	}
	
	JSNumber(const JSContext& js_context, uint32_t number) : JSNumber(js_context, static_cast<double>(number)) {
	}
	
	// Only JSContext and JSValue can create a JSNumber.
	friend JSContext;
	friend JSValue;
};

}} // namespace JavaScriptCoreCPP {

#endif // _JAVASCRIPTCORECPP_JSNUMBER_HPP_