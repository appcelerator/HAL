/**
 * HAL
 *
 * Copyright (c) 2014 by Appcelerator, Inc. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _HAL_JSCONTEXTGROUP_HPP_
#define _HAL_JSCONTEXTGROUP_HPP_

#include "HAL/detail/JSBase.hpp"

#include <utility>

namespace HAL {
  
  class JSContext;
  class JSClass;
  
  /*!
   @class
   
   @discussion A JSContextGroup is an RAII wrapper around a
   JSContextGroupRef, the JavaScriptCore C API representation of a
   group that associates JavaScript contexts with one another.
   
   Scripts may execute concurrently with scripts executing in other
   contexts, and contexts within the same context group may share and
   exchange their JavaScript objects with one another.
   
   When JavaScript objects within the same context group are used in
   multiple threads, explicit synchronization is required.
   
   JSContextGroups are the only way to create a JSContext which
   represents a JavaScript execution context.
   
   JSContextGroups may be created with either the default or custom
   global objects. See the individual JSContextGroup constructors for
   more details.
   */
  class HAL_EXPORT JSContextGroup final HAL_PERFORMANCE_COUNTER1(JSContextGroup) {
    
  public:
    
    /*!
     @method
     
     @abstract Create a JavaScript context group. JSContexts within
     this context group may share and exchange JavaScript objects with
     one another.
     */
    JSContextGroup() HAL_NOEXCEPT;
    
    /*!
     @method
     
     @abstract Create a JavaScript execution context within this
     context group. Scripts may execute in this context concurrently
     with scripts executing in other contexts.
     
     @discussion All JSContexts within this context group may share
     and exchange JavaScript values with one another.
     
     When JavaScript objects from the same context group are used in
     multiple threads, explicit synchronization is required.
     
     Providing an optional custom JSClass allows you to create a
     custom global object for this execution context. Not providing a
     JSClass will create the global object populated with all of the
     standard built-in JavaScript objects, such as Object, Function,
     String, and Array
     
     @param global_object_class An optional JSClass used to create the
     global object.
     */
    JSContext CreateContext() const HAL_NOEXCEPT;
    JSContext CreateContext(const JSClass& global_object_class) const HAL_NOEXCEPT;
    
    ~JSContextGroup()                         HAL_NOEXCEPT;
    JSContextGroup(const JSContextGroup&)     HAL_NOEXCEPT;
    JSContextGroup(JSContextGroup&&)          HAL_NOEXCEPT;
    JSContextGroup& operator=(JSContextGroup) HAL_NOEXCEPT;
    void swap(JSContextGroup&)                HAL_NOEXCEPT;

    // For interoperability with the JavaScriptCore C API.
    explicit JSContextGroup(JSContextGroupRef js_context_group_ref) HAL_NOEXCEPT;
    
    explicit operator JSContextGroupRef() const HAL_NOEXCEPT {
      return js_context_group_ref__;
    }
     
  private:
  
    // Prevent heap based objects.
    void* operator new(std::size_t)     = delete; // #1: To prevent allocation of scalar objects
    void* operator new [] (std::size_t) = delete; // #2: To prevent allocation of array of objects
    
    HAL_EXPORT friend bool operator==(const JSContextGroup& lhs, const JSContextGroup& rhs);
    
    // Silence 4251 on Windows since private member variables do not
    // need to be exported from a DLL.
#pragma warning(push)
#pragma warning(disable: 4251)
    bool managed__ { false };
    JSContextGroupRef js_context_group_ref__;
#pragma warning(pop)
    
#undef HAL_JSCONTEXTGROUP_LOCK_GUARD
#ifdef HAL_THREAD_SAFE
    std::recursive_mutex mutex__;
#define HAL_JSCONTEXTGROUP_LOCK_GUARD std::lock_guard<std::recursive_mutex> lock(mutex__)
#else
#define HAL_JSCONTEXTGROUP_LOCK_GUARD
#endif  // HAL_THREAD_SAFE
  };
  
  inline
  void swap(JSContextGroup& first, JSContextGroup& second) HAL_NOEXCEPT {
    first.swap(second);
  }
  
  // Return true if the two JSContextGroups are equal.
  inline
  bool operator==(const JSContextGroup& lhs, const JSContextGroup& rhs) {
    return lhs.js_context_group_ref__ == rhs.js_context_group_ref__;
  }
  
  // Return true if the two JSContextGroups are not equal.
  inline
  bool operator!=(const JSContextGroup& lhs, const JSContextGroup& rhs) {
    return ! (lhs == rhs);
  }
} // namespace HAL {

#endif // _HAL_JSCONTEXTGROUP_HPP_
