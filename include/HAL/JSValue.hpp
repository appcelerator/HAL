/**
 * HAL
 *
 * Copyright (c) 2018 by Axway. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _HAL_JSVALUE_HPP_
#define _HAL_JSVALUE_HPP_

#include "HAL/detail/JSBase.hpp"
#include "HAL/JSContext.hpp"
#include "HAL/detail/JSUtil.hpp"

namespace HAL {
  class JSObject;
}

namespace HAL {
  
  /*!
   @class
   
   @discussion A JSValue is an RAII wrapper around a JSValueRef, the
   JSRT API representation of a JavaScript value. This is
   the base class for all JavaScript values. These are the direct
   descendants of JSValue:
   
   JSUndefined
   JSNull
   JSBoolean
   JSNumber
   
   The only way to create a JSValue is by using the
   JSContext::CreateXXX member functions.
   */
  class HAL_EXPORT JSValue {
    
  public:
    
    /*!
     @enum Type
     @abstract An enum identifying the type of a JSValue.
     @constant Undefined  The unique undefined value.
     @constant Null       The unique null value.
     @constant Boolean    A primitive boolean value, one of true or false.
     @constant Number     A primitive number value.
     @constant String     A primitive string value.
     @constant Object     An object value (meaning that this JSValue is a JSObject).
     */
    enum class Type {
      Undefined,
      Null,
      Boolean,
      Number,
      String,
      Object
    };
    
    /*!
     @method
     
     @abstract Return a JSString containing the JSON serialized
     representation of this JavaScript value.
     
     @param indent The number of spaces to indent when nesting. If 0
     (the default), the resulting JSON will not contain newlines. The
     size of the indent is clamped to 10 spaces.
     
     @result A JSString containing the JSON serialized representation
     of this JavaScript value.
     */
    virtual JSString ToJSONString(unsigned indent = 0) const final;
    
    /*!
     @method
     
     @abstract Convert this JSValue to a JSString.
     
     @result A JSString with the result of conversion.
     */
    explicit operator JSString() const;
    
    /*!
     @method
     
     @abstract Convert this JSValue to a std::string.
     
     @result A std::string with the result of conversion.
     */
    explicit operator std::string() const;
    
    /*!
     @method
     
     @abstract Convert a JSValue to a boolean.
     
     @result The boolean result of conversion.
     */
    explicit operator bool() const HAL_NOEXCEPT;
    
    /*!
     @method
     
     @abstract Convert a JSValue to a double.
     
     @result The double result of conversion.
     */
    explicit operator double() const;
    
    /*!
     @method
     
     @abstract Convert a JSValue to an int32_t according to the rules
     specified by the JavaScript language.
     
     @result The int32_t result of conversion.
     */
    explicit operator int32_t() const;
    
    /*!
     @method
     
     @abstract Convert a JSValue to an uint32_t according to the rules
     specified by the JavaScript language.
     
     @discussion The JSValue is converted to an uint32_t according to
     the rules specified by the JavaScript language (implements
     ToUInt32, defined in ECMA-262 9.6).
     
     @result The uint32_t result of the conversion.
     */
    explicit operator uint32_t() const{
      // As commented in the spec, the operation of ToInt32 and
      // ToUint32 only differ in how the result is interpreted; see
      // NOTEs in sections 9.5 and 9.6.
      return operator int32_t();
    }
    
    /*!
     @method
     
     @abstract Convert this JSValue to a JSObject.
     
     @result A JSObject with the result of conversion.
     
     @throws std::runtime_error if this JSValue could not be converted
     to a JSObject.
     */
    explicit operator JSObject() const;
    
    /*!
     @method
     
     @abstract Determine whether this JavaScript value's type is the
     undefined type.
     
     @result true if this JavaScript value's type is the undefined
     type.
     */
    virtual bool IsUndefined() const HAL_NOEXCEPT final;
    
    /*!
     @method
     
     @abstract Determine whether this JavaScript value's type is the
     null type.
     
     @result true if this JavaScript value's type is the null type.
     */
    virtual bool IsNull() const HAL_NOEXCEPT final;
		
    /*!
     @method
     
     @abstract Determine whether this JavaScript value's type is the
     boolean type.
     
     @result true if this JavaScript value's type is the boolean type.
     */
    virtual bool IsBoolean() const HAL_NOEXCEPT final;

    /*!
     @method
     
     @abstract Determine whether this JavaScript value's type is the
     number type.
     
     @result true if this JavaScript value's type is the number type.
     */
    virtual bool IsNumber() const HAL_NOEXCEPT final;
    
    /*!
     @method
     
     @abstract Determine whether this JavaScript value's type is the
     string type.
     
     @result true if this JavaScript value's type is the string type.
     */
    virtual bool IsString() const HAL_NOEXCEPT final;
    
    /*!
     @method
     
     @abstract Determine whether this JavaScript value's type is the
     object type.
     
     @result true if this JavaScript value's type is the object type.
     */
    virtual bool IsObject() const HAL_NOEXCEPT final;
    
    /*!
     @method
     
     @abstract Return the execution context of this JavaScript value.
     
     @result The the execution context of this JavaScript value.
     */
    virtual JSContext get_context() const HAL_NOEXCEPT final {
      return JSContext(detail::GetContextRef());
    }
    
    virtual ~JSValue()           HAL_NOEXCEPT;
    JSValue(const JSValue&)      HAL_NOEXCEPT;
    JSValue(JSValue&&)           HAL_NOEXCEPT;
    JSValue& operator=(JSValue);
    void swap(JSValue&)          HAL_NOEXCEPT;

    // For interoperability with the JSRT API.
    JSValue(JsValueRef js_value_ref) HAL_NOEXCEPT;
    
    // For interoperability with the JSRT API.
    explicit operator JsValueRef() const HAL_NOEXCEPT {
      return js_value_ref__;
    }
     
  protected:
    
    // A JSContext can create a JSValue.
    friend class JSContext;
    
    JSValue(const JSString& js_string, bool parse_as_json = false);
  
    // These classes and functions need access to operator
    // JSValueRef().
    HAL_EXPORT friend bool operator==(const JSValue& lhs, const JSValue& rhs) HAL_NOEXCEPT;
    
  private:
    
    // Prevent heap based objects.
    static void * operator new(std::size_t);     // #1: To prevent allocation of scalar objects
    static void * operator new [] (std::size_t); // #2: To prevent allocation of array of objects

    // Silence 4251 on Windows since private member variables do not
    // need to be exported from a DLL.
#pragma warning(push)
#pragma warning(disable: 4251)
    JsValueRef js_value_ref__ { nullptr };
#pragma warning(pop)
  };
  
  inline
  void swap(JSValue& first, JSValue& second) HAL_NOEXCEPT {
    first.swap(second);
  }
  
  /*!
   @function
   
   @abstract Determine whether two JavaScript values are strict equal,
   as compared by the JS === operator.
   
   @param lhs The first value to test.
   
   @param rhs The second value to test.
   
   @result true if the two values are equal, false if they are not
   equal.
   */
  HAL_EXPORT bool operator==(const JSValue& lhs, const JSValue& rhs) HAL_NOEXCEPT;
  
  // Return true if the two JSValues are not strict equal, as compared by the JS === operator.
  inline
  bool operator!=(const JSValue& lhs, const JSValue& rhs) HAL_NOEXCEPT {
    return ! (lhs == rhs);
  }
  
} // namespace HAL {

#endif // _HAL_JSVALUE_HPP_
