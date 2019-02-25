/**
 * Javascriptcorecpp
 *
 * Copyright (c) 2014 by Appcelerator, Inc. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _HAL_DETAIL_JSEXPORTCLASSDEFINITION_HPP_
#define _HAL_DETAIL_JSEXPORTCLASSDEFINITION_HPP_

#include "HAL/detail/JSBase.hpp"
#include "HAL/JSClassDefinition.hpp"
#include "HAL/JSClassAttribute.hpp"

#include "HAL/detail/JSExportNamedValuePropertyCallback.hpp"
#include "HAL/detail/JSExportNamedFunctionPropertyCallback.hpp"
#include "HAL/detail/JSExportCallbacks.hpp"

#include <string>
#include <unordered_map>

namespace HAL { namespace detail {
  
  template<typename T>
  using JSExportNamedValuePropertyCallbackMap_t    = std::unordered_map<std::string, JSExportNamedValuePropertyCallback<T>>;
  
  template<typename T>
  using JSExportNamedFunctionPropertyCallbackMap_t = std::unordered_map<std::string, JSExportNamedFunctionPropertyCallback<T>>;
  
  template<typename T>
  class JSExportClassDefinitionBuilder;
  
  template<typename T>
  class JSExportClass;

  /*!
   @class
   
   @discussion A JSExportClassDefinition defines the properties and
   callbacks of a JavaScript object implemented by a C++ classed
   derived from JSExport.
   
   The only way to create a JSExportClassDefinition is by using a
   JSExportClassDefinitionBuilder.
   
   This class is thread safe and immutable by design.
   */
  template<typename T>
  class JSExportClassDefinition final : public JSClassDefinition HAL_PERFORMANCE_COUNTER2(JSExportClassDefinition<T>) {
  public:
    
    JSExportClassDefinition(const JSExportClassDefinitionBuilder<T>& builder);
    JSExportClassDefinition()                                          = default;
    ~JSExportClassDefinition()                                         = default;
    JSExportClassDefinition(const JSExportClassDefinition&)            HAL_NOEXCEPT;
    JSExportClassDefinition(JSExportClassDefinition&&)                 HAL_NOEXCEPT;
    JSExportClassDefinition& operator=(const JSExportClassDefinition&) HAL_NOEXCEPT;
    JSExportClassDefinition& operator=(JSExportClassDefinition&&)      HAL_NOEXCEPT;
    void swap(JSExportClassDefinition&)                                HAL_NOEXCEPT;
    
  private:
    
    void InitializeNamedPropertyCallbacks() HAL_NOEXCEPT;

    // Only JSExportClass can access our private member variables.
    template<typename U>
    friend class JSExportClass;

	friend class JSExportClassDefinitionBuilder<T>;

    static std::unordered_set<std::string>               named_constants__;
	static JSExportNamedValuePropertyCallbackMap_t<T>    named_value_property_callback_map__;
	static JSExportNamedFunctionPropertyCallbackMap_t<T> named_function_property_callback_map__;
    static HasPropertyCallback<T>                        has_property_callback__;
	static GetPropertyCallback<T>                        get_property_callback__ ;
	static SetPropertyCallback<T>                        set_property_callback__;
	static DeletePropertyCallback<T>                     delete_property_callback__;
	static GetPropertyNamesCallback<T>                   get_property_names_callback__;
	static CallAsFunctionCallback<T>                     call_as_function_callback__;
	static ConvertToTypeCallback<T>                      convert_to_type_callback__;
  };

  template<typename T>
  JSExportClassDefinition<T>::JSExportClassDefinition(const JSExportClassDefinition<T>& rhs) HAL_NOEXCEPT
  : JSClassDefinition(rhs) {
    InitializeNamedPropertyCallbacks();
  }
  
  template<typename T>
  JSExportClassDefinition<T>::JSExportClassDefinition(JSExportClassDefinition<T>&& rhs) HAL_NOEXCEPT
  : JSClassDefinition(rhs) {
    InitializeNamedPropertyCallbacks();
  }
  
  template<typename T>
  JSExportClassDefinition<T>& JSExportClassDefinition<T>::operator=(const JSExportClassDefinition<T>& rhs) HAL_NOEXCEPT {
    HAL_JSCLASSDEFINITION_LOCK_GUARD;
    JSClassDefinition::operator=(rhs);
    InitializeNamedPropertyCallbacks();
    
    return *this;
    }
    
    template<typename T>
    JSExportClassDefinition<T>& JSExportClassDefinition<T>::operator=(JSExportClassDefinition<T>&& rhs) HAL_NOEXCEPT {
      HAL_JSCLASSDEFINITION_LOCK_GUARD;
      swap(rhs);
      InitializeNamedPropertyCallbacks();
      
      return *this;
    }
    
    template<typename T>
    void JSExportClassDefinition<T>::swap(JSExportClassDefinition<T>& other) HAL_NOEXCEPT {
      HAL_JSCLASSDEFINITION_LOCK_GUARD;
      JSClassDefinition::swap(other);
    }
    
    template<typename T>
    void swap(JSExportClassDefinition<T>& first, JSExportClassDefinition<T>& second) HAL_NOEXCEPT {
      first.swap(second);
    }
    
    template<typename T>
    void JSExportClassDefinition<T>::InitializeNamedPropertyCallbacks() HAL_NOEXCEPT {
      
      // Initialize staticValues.
      static_values__.clear();
      js_class_definition__.staticValues = nullptr;
      if (!named_value_property_callback_map__.empty()) {
        for (const auto& entry : named_value_property_callback_map__) {
          const auto& property_name       = entry.first;
          const auto& property_attributes = entry.second.get_attributes();
          ::JSStaticValue static_value;
          static_value.name        = property_name.c_str();
          static_value.getProperty = JSExportClass<T>::GetNamedValuePropertyCallback;
          static_value.setProperty = JSExportClass<T>::SetNamedValuePropertyCallback;
          static_value.attributes  = ToJSPropertyAttributes(property_attributes);
          static_values__.push_back(static_value);
          // HAL_LOG_DEBUG("JSExportClassDefinition<", name__, "> added value property ", static_values__.back().name);
        }
        static_values__.push_back({nullptr, nullptr, nullptr, kJSPropertyAttributeNone});
        js_class_definition__.staticValues = &static_values__[0];
      }
      
      // Initialize staticFunctions.
      static_functions__.clear();
      js_class_definition__.staticFunctions = nullptr;
      if (!named_function_property_callback_map__.empty()) {
        for (const auto& entry : named_function_property_callback_map__) {
          const auto& function_name = entry.first;
          const auto& property_attributes = entry.second.get_attributes();
          ::JSStaticFunction static_function;
          static_function.name           = function_name.c_str();
          static_function.callAsFunction = JSExportClass<T>::CallNamedFunctionCallback;
          static_function.attributes     = ToJSPropertyAttributes(property_attributes);
          static_functions__.push_back(static_function);
          // HAL_LOG_DEBUG("JSExportClassDefinition<", name__, "> added function property ", static_functions__.back().name);
        }
        static_functions__.push_back({nullptr, nullptr, kJSPropertyAttributeNone});
        js_class_definition__.staticFunctions = &static_functions__[0];
      }
    }
    
    }} // namespace HAL { namespace detail {
    
#endif // _HAL_DETAIL_JSEXPORTCLASSDEFINITION_HPP_
