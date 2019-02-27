/**
 * HAL
 *
 * Copyright (c) 2018 by Axway. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _HAL_JSCONTEXT_HPP_
#define _HAL_JSCONTEXT_HPP_

#include "HAL/detail/JSBase.hpp"
#include "HAL/JSContextGroup.hpp"

#include <vector>
#include <unordered_map>
#include <functional>

namespace HAL {
  
  class JSClass;
  class JSString;
  class JSValue;
  class JSObject;
  class JSArray;
  class JSError;
  class JSExportObject;
  
}

namespace HAL {
  
  /*!
   @class
   
   @discussion A JSContext is an RAII wrapper around a JsContextRef,
   the JavaScriptCore C API representation of a JavaScript execution
   context that holds the global object and other execution state.
   
   JSContexts are created by the JSContextGroup::CreateContext member
   function.
   
   Scripts may execute concurrently with scripts executing in other
   contexts, and contexts within the same context group may share and
   exchange their JavaScript objects with one another.
   
   When JavaScript objects within the same context group are used in
   multiple threads, explicit synchronization is required.
   */
  class HAL_EXPORT JSContext final {
    
  public:
    
    /*!
     @method
     
     @abstract Return the global object of this JavaScript execution
     context.
     
     @result The global object of this JavaScript execution context.
     */
    JSObject get_global_object() const HAL_NOEXCEPT;
        
    /*!
     @method
     
     @abstract Create a JavaScript value from parsing a JSON formatted
     string.
     
     @param string The JSString that contains the JSON data to be
     parsed.
     
     @result A JavaScript value containing the result of parsing the
     JSON data.
     
     @throws std::invalid_argument exception if the string isn't a
     valid JSON formatted string.
     */
    JSValue CreateValueFromJSON(const JSString& js_string) const;
    
    /*!
     @method
     
     @abstract Create a JavaScript value of the string type.
     
     @param string The string to assign to the newly created JSValue.
     
     @result A JSValue of the string type that represents the value of
     string.
     */
    JSValue CreateString()                          const HAL_NOEXCEPT;
    JSValue CreateString(const JSString& js_string) const HAL_NOEXCEPT;
    JSValue CreateString(const char*        string) const HAL_NOEXCEPT;
    JSValue CreateString(const std::string& string) const HAL_NOEXCEPT;
    
    /*!
     @method
     
     @abstract Create a JavaScript value of the undefined type.
     
     @result The unique undefined value.
     */
    JSValue CreateUndefined() const HAL_NOEXCEPT;
    
    /*!
     @method
     
     @abstract Create a JavaScript value of the null type.
     
     @result The unique null value.
     */
	JSValue CreateNull() const HAL_NOEXCEPT;
		
    /*!
     @method
     
     @abstract Create a JavaScript value of the boolean type.
     
     @param boolean The bool to pass to the JSBoolean constructor.
     
     @result A JavaScript value of the boolean type, representing the
     value of boolean.
     */
	JSValue CreateBoolean(bool boolean) const HAL_NOEXCEPT;
    
    /*!
     @method
     
     @abstract Create a JavaScript value of the number type from a
     double.
     
     @param number The double to pass to the JSNumber constructor.
     
     @result A JavaScript value of the number type, representing the
     value of number.
     */
	JSValue CreateNumber(double number) const HAL_NOEXCEPT;
    
    /*!
     @method
     
     @abstract Create a JavaScript value of the number type an
     int32_t.
     
     @param number The int32_t to pass to the JSNumber constructor.
     
     @result A JavaScript value of the number type, representing the
     value of number.
     */
	JSValue CreateNumber(int32_t number) const HAL_NOEXCEPT;
    
    /*!
     @method
     
     @abstract Create a JavaScript value of the number type from a
     uint32_t.
     
     @param number The uint32_t to pass to the JSNumber constructor.
     
     @result A JavaScript value of the number type, representing the
     value of number.
     */
	JSValue CreateNumber(uint32_t number) const HAL_NOEXCEPT;
    
    /*!
     @method
     
     @abstract Create a JavaScript object in this execution
     context. An empty JavaScript object is returned if you do not
     provide a JSClass.
     
     @param js_class An optional custom JSClass to pass to the
     JSObject constructor.

     @param properties A set of properties to be applied to the object
     
     @result A JavaScript object.
     */
    JSObject CreateObject() const HAL_NOEXCEPT;
    JSObject CreateObject(const JSClass& js_class) const HAL_NOEXCEPT;
    JSObject CreateObject(const std::unordered_map<std::string, JSValue>& properties) const HAL_NOEXCEPT;
    JSObject CreateObject(const JSClass& js_class, const std::unordered_map<std::string, JSValue>& properties) const HAL_NOEXCEPT;

    /*!
     @method
     
     @abstract Create a JavaScript Array object.
     
     @discussion The behavior of this constructor does not exactly
     match the behavior of the built-in Array constructor in that if
     the vector of arguments contains one element then the JSArray is
     also created with on element.
     
     @param arguments Optional JavaScript values to populate the
     array. Otherwise an empty array is created.
     
     @result A JavaScript object that is an Array, populated with the
     given JavaScript values.
     */
    JSArray CreateArray() const HAL_NOEXCEPT;
    JSArray CreateArray(const std::vector<JSValue>& arguments) const;
    
    /*!
     @method
     
     @abstract Create a JavaScript Date object, as if by invoking the
     built-in Date constructor.
     
     @param arguments Optional JavaScript values to pass to the Date
     Constructor.
     
     @result A JSObject that is a Date.
     */
    JSObject CreateDate() const HAL_NOEXCEPT;
    JSObject CreateDate(const std::vector<JSValue>& arguments) const;
    
    /*!
     @method
     
     @abstract Create a JavaScript Error object, as if by invoking the
     built-in Error constructor.
     
     @param arguments Optional JavaScript values to pass to the Error
     Constructor.
     
     @result A JSObject that is a Error.
     */
    JSError CreateError() const HAL_NOEXCEPT;
	JSError CreateError(const std::string& message) const HAL_NOEXCEPT;

    /* Script Evaluation */
    
    /*!
     @method
     
     @abstract Evaluate a string of JavaScript code.
     
     @param script A JSString containing the script to evaluate.
     
     @param this_object An optional JavaScript object to use as
     "this". The default is the global object.
     
     @param source_url An optional JSString containing a URL for the
     script's source file. This is used by debuggers and when
     reporting exceptions.
     
     @param starting_line_number An optional integer value specifying
     the script's starting line number in the file located at
     source_url. This is only used when reporting exceptions. The
     value is one-based, so the first line is line 1 and invalid
     values are clamped to 1.
     
     @result The JSValue that results from evaluating script.
     
     @throws std::runtime_error exception if the evaluated script
     threw an exception.
     
     */
    JSValue JSEvaluateScript(const std::string& script, const std::string& source_url = "") const;
    JSValue JSEvaluateScript(const std::string& script, JSObject this_object, const std::string& source_url = "") const;
    
    /*!
     @method
     
     @abstract Check for syntax errors in a string of JavaScript.
     
     @param script A JSString containing the script to check for
     syntax errors.

     @result true if the script is syntactically correct, otherwise
     false.
     */
    bool JSCheckScriptSyntax(const std::string& script, const std::string& source_url = "") const HAL_NOEXCEPT;
    
    
    /*!
     @method
     
     @abstract Perform a JavaScript garbage collection.
     
     @discussion JavaScript values that are on the machine stack, in a
     register, protected by JSValueProtect, set as the global object
     of an execution context, or reachable from any such value will
     not be collected.
     
     During JavaScript execution, you are not required to call this
     function; the JavaScript engine will garbage collect as
     needed. JavaScript values created within a context group are
     automatically destroyed when the last reference to the context
     group is released.
     */
    void GarbageCollect() const HAL_NOEXCEPT;
        
    JSContext() = delete;
    ~JSContext()                    HAL_NOEXCEPT;
    JSContext(const JSContext&)     HAL_NOEXCEPT;
    JSContext(JSContext&&)          HAL_NOEXCEPT;
    JSContext& operator=(JSContext) HAL_NOEXCEPT;
    void swap(JSContext&)           HAL_NOEXCEPT;

    // For interoperability with the JavaScriptCore C API.
    explicit operator JsContextRef() const HAL_NOEXCEPT {
      return js_context_ref__;
    }
    
    explicit JSContext(JsContextRef js_context_ref) HAL_NOEXCEPT;
    
  private:
    
    // Only a JSContextGroup create a JSContext using the following
    // constructor.
    friend class JSContextGroup;
    
    HAL_EXPORT friend bool operator==(const JSValue& lhs, const JSValue& rhs) HAL_NOEXCEPT;
   
    // Prevent heap based objects.
    static void * operator new(std::size_t);       // #1: To prevent allocation of scalar objects
    static void * operator new [] (std::size_t);   // #2: To prevent allocation of array of objects
    
    HAL_EXPORT friend bool operator==(const JSContext& lhs, const JSContext& rhs);
    
    // Silence 4251 on Windows since private member variables do not
    // need to be exported from a DLL.
#pragma warning(push)
#pragma warning(disable: 4251)
	JsContextRef js_context_ref__ { nullptr };
#pragma warning(pop)    
  };
  
  inline
  void swap(JSContext& first, JSContext& second) HAL_NOEXCEPT {
    first.swap(second);
  }
  
  // Return true if the two JSContexts are equal.
  inline
  bool operator==(const JSContext& lhs, const JSContext& rhs) {
    return (lhs.js_context_ref__ == rhs.js_context_ref__);
  }
  
  // Return true if the two JSContexts are not equal.
  inline
  bool operator!=(const JSContext& lhs, const JSContext& rhs) {
    return ! (lhs == rhs);
  }

} // namespace HAL {

#endif // _HAL_JSCONTEXT_HPP_
