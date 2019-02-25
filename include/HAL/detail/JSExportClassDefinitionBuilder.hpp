/**
 * HAL
 *
 * Copyright (c) 2014 by Appcelerator, Inc. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _HAL_DETAIL_JSEXPORTCLASSDEFINITIONBUILDER_HPP_
#define _HAL_DETAIL_JSEXPORTCLASSDEFINITIONBUILDER_HPP_

#include "HAL/detail/JSBase.hpp"
#include "HAL/detail/JSExportClassDefinition.hpp"
#include "HAL/detail/JSExportClass.hpp"
#include "HAL/detail/JSUtil.hpp"

#include <string>
#include <cstdint>

#undef HAL_DETAIL_JSEXPORTCLASSDEFINITIONBUILDER_MUTEX
#undef HAL_DETAIL_JSEXPORTCLASSDEFINITIONBUILDER_LOCK_GUARD
#ifdef HAL_THREAD_SAFE
#define HAL_DETAIL_JSEXPORTCLASSDEFINITIONBUILDER_MUTEX                      std::recursive_mutex       mutex__
#define HAL_DETAIL_JSEXPORTCLASSDEFINITIONBUILDER_LOCK_GUARD std::lock_guard<std::recursive_mutex> lock(mutex__)
#else
#define HAL_DETAIL_JSEXPORTCLASSDEFINITIONBUILDER_MUTEX
#define HAL_DETAIL_JSEXPORTCLASSDEFINITIONBUILDER_LOCK_GUARD
#endif  // HAL_THREAD_SAFE


namespace HAL { namespace detail {
  
  /*!
   @class
   
   @abstract This is a builder that creates a JSExportClassDefinition
   for defining a JSClass backed by a C++ class.
   
   @discussion All properties on this builder except 'ClassName' are
   optional, and all callbacks may be nullptr. By default the
   JSClassVersion number is initialized to 0, the Parent JSClass is
   initialized to the default JavaScript object class, The
   JSClassAttribute defaults to 'AutomaticPrototype', and all
   callbacks are initialized to nullptr.
   
   Using the AddValueProperty and AddFunctionProperty methods are the
   simplest and most efficient means for vending custom properties
   since they autmatically service requests like GetProperty,
   SetProperty and GetPropertyNames. The other property access
   callbacks are required only to implement unusual properties, like
   array indexes, whose names are not known at compile-time.
   
   Standard JavaScript practice calls for storing JavaScript function
   objects in prototypes so that they can be shared with JavaScript
   objects having that prototype. The default JSExportClassDefinition
   created by a JSExportClassDefinitionBuilder follows this idiom,
   instantiating JavaScript objects with a shared, automatically
   generated prototype containing the JSClass's JavaScript function
   objects.
   
   To override this behavior set the JSClassAttribute to
   'NoAutomaticPrototype', which specifies that a JSClass should not
   automatically generate such a prototype. The resulting JSClass will
   then instantiate JavaScript objects with the default JavaScript
   object prototype, and give each JavaScript object its own copy of
   the JSClass's JavaScript function objects.
   
   Setting any callback to nullptr specifies that the default object
   callback should substitute, except in the case of HasProperty,
   where it specifies that GetProperty should substitute.
   */
  template<typename T>
  class JSExportClassDefinitionBuilder final HAL_PERFORMANCE_COUNTER1(JSExportClassDefinitionBuilder<T>) {
    
  public:
    
    /*!
     @method
     
     @abstract Create a builder with the given JSClass name. The
     JSClassVersion number is initialized to 0, the Parent is
     initialized to the default JavaScript object class, the
     JSClassAttribute defaults to 'AutomaticPrototype' and all
     callbacks are initialized to nullptr.
     */
    explicit JSExportClassDefinitionBuilder(const std::string& class_name) HAL_NOEXCEPT
    : name__(class_name) {
      js_class_definition__ = kJSClassDefinitionEmpty;
    }
    
    JSExportClassDefinitionBuilder()                                                       = delete;
    ~JSExportClassDefinitionBuilder()                                                      = default;
    JSExportClassDefinitionBuilder(const JSExportClassDefinitionBuilder<T>&)               = default;
    JSExportClassDefinitionBuilder<T>& operator=(const JSExportClassDefinitionBuilder<T>&) = default;
    
#ifdef HAL_MOVE_CTOR_AND_ASSIGN_DEFAULT_ENABLE
    JSExportClassDefinitionBuilder(JSExportClassDefinitionBuilder<T>&&)                    = default;
    JSExportClassDefinitionBuilder<T>& operator=(JSExportClassDefinitionBuilder<T>&&)      = default;
#endif
    
    /*!
     @method
     
     @abstract Return your JSClass's name.
     
     @result Your JSClass's name.
     */
    std::string ClassName() const HAL_NOEXCEPT {
      return name__;
    }
    
    /*!
     @method
     
     @abstract Set your JSClass's name.
     
     @result A reference to the builder for chaining.
     */
    JSExportClassDefinitionBuilder<T>& ClassName(const std::string& class_name) {
      HAL_DETAIL_JSEXPORTCLASSDEFINITIONBUILDER_LOCK_GUARD;
      if (class_name.empty()) {
        ThrowInvalidArgument("JSExportClassDefinitionBuilder::ClassName", "The class name cannot be emoty.");
      }

      name__ = class_name;
      return *this;
    }
    
    /*!
     @method
     
     @abstract Return your JSClass's version.
     
     @result Your JSClass's version.
     */
    std::uint32_t Version() const HAL_NOEXCEPT {
      return js_class_definition__.version;
    }
    
    /*!
     @method
     
     @abstract Set your JSClass's version.
     
     @result A reference to the builder for chaining.
     */
    JSExportClassDefinitionBuilder<T>& Version(std::uint32_t class_version) HAL_NOEXCEPT {
      HAL_DETAIL_JSEXPORTCLASSDEFINITIONBUILDER_LOCK_GUARD;
      js_class_definition__.version = class_version;
      return *this;
    }
    
    /*!
     @method
     
     @abstract Return your JSClass's JSClassAttribute.
     
     @result Your JSClass's JSClassAttribute.
     */
    JSClassAttribute ClassAttribute() const HAL_NOEXCEPT {
      return FromJSClassAttributes(js_class_definition__.attributes);
    }
    
    /*!
     @method
     
     @abstract Set your JSClass's JSClassAttribute.
     
     @result A reference to the builder for chaining.
     */
    JSExportClassDefinitionBuilder<T>& ClassAttribute(JSClassAttribute class_attribute) HAL_NOEXCEPT {
      HAL_DETAIL_JSEXPORTCLASSDEFINITIONBUILDER_LOCK_GUARD;
      js_class_definition__.attributes = ToJSClassAttribute(class_attribute);
      return *this;
    }
    
    /*!
     @method
     
     @abstract Return the parent of your JSClass created by this
     builder.
     
     @result The parent of your JSClass created by this builder.
     */
    JSClass Parent() const HAL_NOEXCEPT {
      return parent__;
    }
    
    /*!
     @method
     
     @abstract Set the parent of your JSClass created by this builder.
     The default value is the default JavaScript object class.
     
     @result A reference to the builder for chaining.
     */
    JSExportClassDefinitionBuilder<T>& Parent(const JSClass& parent) HAL_NOEXCEPT {
      HAL_DETAIL_JSEXPORTCLASSDEFINITIONBUILDER_LOCK_GUARD;
      parent__ = parent;
      return *this;
    }
    
    /*!
     @method
     
     @abstract Add callbacks to invoke when getting and/or setting a
     value property on your JavaScript object. The property will
     always have the 'DontDelete' attribute. If a setter callback is
     not provided then the property will also have the 'ReadOnly'
     attribute. By default the property is enumerable unless you
     specify otherwise.
     
     @discussion For example, given this class definition:
     
     class Foo {
     JSValue GetName() const;
     bool SetName(JSValue& value);
     };
     
     You would call the builer like this:
     
     JSExportClassDefinitionBuilder<Foo> builder("Foo");
     builder.AddValueProperty("name", &Foo::GetName, &Foo::SetName);
     
     If you wanted the property ReadOnly, then you would call the
     builder like this:
     
     builder.AddValueProperty("name", &Foo::GetName);
     
     @param property_name A JSString containing the property's name.
     
     @param get_callback The callback to invoke when getting a
     property's value from your JavaScript object.
     
     @param set_callback The callback to invoke when setting a
     property's value on your JavaScript object. This may be nullptr,
     in which case the 'ReadOnly' attribute for this property is
     automatically set. If this callback returns false, then this
     indicates that the value was not set.
     
     @param enumerable An optional property attribute that specifies
     whether the property is enumerable. The default value is true,
     which means the property is enumerable.
     
     @throws std::invalid_argument exception under these preconditions:
     
     1. If property_name is empty.
     
     2. If both get_callback and set_callback are missing.
     
     @result A reference to the builder for chaining.
     */
    JSExportClassDefinitionBuilder<T>& AddValueProperty(const JSString& property_name, GetNamedValuePropertyCallback<T> get_callback, SetNamedValuePropertyCallback<T> set_callback = nullptr, bool enumerable = true) {
      std::unordered_set<JSPropertyAttribute> attributes { JSPropertyAttribute::DontDelete };
      static_cast<void>(!enumerable   && attributes.insert(JSPropertyAttribute::DontEnum).second);
      static_cast<void>(!set_callback && attributes.insert(JSPropertyAttribute::ReadOnly).second);
      HAL_DETAIL_JSEXPORTCLASSDEFINITIONBUILDER_LOCK_GUARD;
      AddValuePropertyCallback(JSExportNamedValuePropertyCallback<T>(property_name, get_callback, set_callback, attributes));
      return *this;
    }

    /*!
     @method
     
     @abstract Add callbacks to invoke when getting a constant property
     on your JavaScript object. The getter callback will be called only once
     because constant is cached and reused.

     The property will always have the 'DontDelete' and 'ReadOnly' attribute.
     By default the property is enumerable unless you specify otherwise.
     
     @discussion For example, given this class definition:
     
     class Foo {
     JSValue GetName() const;
     bool SetName(JSValue& value);
     };
     
     You would call the builer like this:
     
     JSExportClassDefinitionBuilder<Foo> builder("Foo");
     builder.AddConstantProperty("name", &Foo::GetName);
     
     If you wanted the property non-enumerable, then you would call the
     builder like this:
     
     builder.AddValueProperty("name", &Foo::GetName, false);
     
     @param property_name A JSString containing the property's name.
     
     @param get_callback The callback to invoke when getting a
     property's value from your JavaScript object. Note that this callback
     will be called only once, because constant value will be cached.
     
     @param enumerable An optional property attribute that specifies
     whether the property is enumerable. The default value is true,
     which means the property is enumerable.
     
     @throws std::invalid_argument exception under these preconditions:
     
     1. If property_name is empty.
     
     2. If get_callback are missing.
     
     @result A reference to the builder for chaining.
     */
    JSExportClassDefinitionBuilder<T>& AddConstantProperty(const JSString& property_name, GetNamedValuePropertyCallback<T> get_callback, bool enumerable = true) {
      std::unordered_set<JSPropertyAttribute> attributes { JSPropertyAttribute::DontDelete, JSPropertyAttribute::ReadOnly };
      static_cast<void>(!enumerable   && attributes.insert(JSPropertyAttribute::DontEnum).second);
      HAL_DETAIL_JSEXPORTCLASSDEFINITIONBUILDER_LOCK_GUARD;
      AddConstantPropertyCallback(JSExportNamedValuePropertyCallback<T>(property_name, get_callback, nullptr, attributes));
      return *this;
    }   
    
    /*!
     @method
     
     @abstract Add a function property to your JavaScript object with
     the 'None' attribute. By default the
     property is enumerable unless you specify otherwise.
     
     @discussion For example, given this class definition:
     
     class Foo {
     JSValue Hello(const std::vector<JSValue>& arguments);
     };
     
     You would call the builer like this:
     
     JSExportClassDefinitionBuilder<Foo> builder("Foo");
     builder.AddFunctionProperty("hello", &Foo::Hello);
     
     @param function_name A JSString containing the function's name.
     
     @param enumerable An optional property attribute that specifies
     whether the property is enumerable. The default value is true,
     which means the property is enumerable.
     
     @throws std::invalid_argument exception under these preconditions:
     
     1. If function_name is empty.
     
     2. If function_callback is not provided.
     
     @result A reference to the builder for chaining.
     */
    JSExportClassDefinitionBuilder<T>& AddFunctionProperty(const JSString& function_name, CallNamedFunctionCallback<T> function_callback, bool enumerable = true) {
      std::unordered_set<JSPropertyAttribute> attributes { JSPropertyAttribute::None };
      static_cast<void>(!enumerable && attributes.insert(JSPropertyAttribute::DontEnum).second);
      HAL_DETAIL_JSEXPORTCLASSDEFINITIONBUILDER_LOCK_GUARD;
      AddFunctionPropertyCallback(JSExportNamedFunctionPropertyCallback<T>(function_name, function_callback, attributes));
      return *this;
    }
    
    /*!
     @method
     
     @abstract Return the callback to invoke when determining whether
     your JavaScript object has a property.
     
     @result The callback to invoke when determining whether your
     JavaScript object has a property.
     */
    
    HasPropertyCallback<T> HasProperty() const HAL_NOEXCEPT {
      return JSExportClassDefinition<T>::has_property_callback__;
    }
    
    /*!
     @method
     
     @abstract Set the callback to invoke when determining whether
     your JavaScript object has a property. If this callback is
     missing then your JavaScript object will delegate to it's
     GetPropertyCallback.
     
     @discussion The HasPropertyCallback enables optimization in cases
     where only a property's existence needs to be known, not its
     value, and computing its value is expensive. If the
     HasPropertyCallback doesn't exist, then the GetPropertyCallback
     will be used instead.
     
     If this callback returns false then the reqeust forwards to
     properties added by the AddValueProperty and AddFunctionProperty
     methods (if any), then properties vended by your class' parent
     class chain, then properties belonging to your JavaScript
     object's prototype chain.
     
     For example, given this class definition:
     
     class Foo {
     bool HasProperty(const JSString& property_name) const;
     };
     
     You would call the builer like this:
     
     JSClassBuilder<Foo> builder("Foo");
     builder.HasProperty(&Foo::HasProperty);
     
     @result A reference to the builder for chaining.
     */
    JSExportClassDefinitionBuilder<T>& HasProperty(const HasPropertyCallback<T>& has_property_callback) HAL_NOEXCEPT {
      HAL_DETAIL_JSEXPORTCLASSDEFINITIONBUILDER_LOCK_GUARD;
	  JSExportClassDefinition<T>::has_property_callback__ = has_property_callback;
      return *this;
    }
    
    /*!
     @method
     
     @abstract Return the callback to invoke when getting a property's
     value from your JavaScript object.
     
     @result The callback to invoke when getting a property's value
     from your JavaScript object.
     */
    GetPropertyCallback<T> GetProperty() const HAL_NOEXCEPT {
      return JSExportClassDefinition<T>::get_property_callback__;
    }
    
    /*!
     @method
     
     @abstract Set the callback to invoke when getting a property's
     value from your JavaScript object.
     
     @discussion If this callback returns JSUndefined then the get
     request forwards to properties added by the AddValueProperty and
     AddFunctionProperty methods (if any), then properties vended by
     your class' parent class chain, then properties belonging to your
     JavaScript object's prototype chain.
     
     For example, given this class definition:
     
     class Foo {
     JSValue GetProperty(const JSString& property_name) const;
     };
     
     You would call the builer like this:
     
     JSClassBuilder<Foo> builder("Foo");
     builder.GetProperty(&Foo::GetProperty);
     
     @result A reference to the builder for chaining.
     */
    JSExportClassDefinitionBuilder<T>& GetProperty(const GetPropertyCallback<T>& get_property_callback) HAL_NOEXCEPT {
      HAL_DETAIL_JSEXPORTCLASSDEFINITIONBUILDER_LOCK_GUARD;
	  JSExportClassDefinition<T>::get_property_callback__ = get_property_callback;
      return *this;
    }
    
    /*!
     @method
     
     @abstract Return the callback to invoke when setting a property's
     value on your JavaScript object.
     
     @result The callback to invoke when setting a property's value on
     your JavaScript object.
     */
    SetPropertyCallback<T> SetProperty() const HAL_NOEXCEPT {
      return JSExportClassDefinition<T>::set_property_callback__;
    }
    
    /*!
     @method
     
     @abstract Set the callback to invoke when setting a property's
     value on your JavaScript object.
     
     @discussion If this callback returns false then the request
     forwards to properties added by the AddValueProperty method (if
     any), then properties vended by your class' parent class chain,
     then properties belonging to your JavaScript object's prototype
     chain.
     
     For example, given this class definition:
     
     class Foo {
     bool SetProperty(const JSString& property_name, const JSValue& value);
     };
     
     You would call the builer like this:
     
     JSClassBuilder<Foo> builder("Foo");
     builder.SetProperty(&Foo::SetProperty);
     
     @result A reference to the builder for chaining.
     */
    JSExportClassDefinitionBuilder<T>& SetProperty(const SetPropertyCallback<T>& set_property_callback) HAL_NOEXCEPT {
      HAL_DETAIL_JSEXPORTCLASSDEFINITIONBUILDER_LOCK_GUARD;
	  JSExportClassDefinition<T>::set_property_callback__ = set_property_callback;
      return *this;
    }
    
    /*!
     @method
     
     @abstract Return the callback to invoke when deleting a property
     from your JavaScript object.
     
     @result The callback to invoke when deleting a property from your
     JavaScript object.
     */
    DeletePropertyCallback<T> DeleteProperty() const HAL_NOEXCEPT {
      return JSExportClassDefinition<T>::delete_property_callback__;
    }
    
    /*!
     @method
     
     @abstract Set the callback to invoke when deleting a property
     from your JavaScript object.
     
     @discussion If this callback returns false then the request
     forwards to properties added by the AddValueProperty and
     AddFunctionProperty methods (if any), then properties vended by
     your class' parent class chain, then properties belonging to your
     JavaScript object's prototype chain.
     
     For example, given this class definition:
     
     class Foo {
     bool DeleteProperty(const JSString& property_name);
     };
     
     You would call the builer like this:
     
     JSClassBuilder<Foo> builder("Foo");
     builder.DeleteProperty(&Foo::DeleteProperty);
     
     @result A reference to the builder for chaining.
     */
    JSExportClassDefinitionBuilder<T>& DeleteProperty(const DeletePropertyCallback<T>& delete_property_callback) HAL_NOEXCEPT {
      HAL_DETAIL_JSEXPORTCLASSDEFINITIONBUILDER_LOCK_GUARD;
	  JSExportClassDefinition<T>::delete_property_callback__ = delete_property_callback;
      return *this;
    }
    
    /*!
     @method
     
     @abstract Return the callback to invoke when collecting the names
     of your JavaScript object's properties.
     
     @result The callback to invoke when collecting the names of your
     JavaScript object's properties
     */
    GetPropertyNamesCallback<T> GetPropertyNames() const HAL_NOEXCEPT {
      return JSExportClassDefinition<T>::get_property_names_callback__;
    }
    
    /*!
     @method
     
     @abstract Set the callback to invoke when collecting the names of
     your JavaScript object's properties provided by the GetProperty
     and SetProperty methods. This callback is part of a chain of
     callbacks that are automatically invoked when your JavaScript
     object is used in a JavaScript for...in loop.
     
     @discussion The GetPropertyNamesCallback only needs to provide
     the property names provided by the GetProperty and SetProperty
     methods. Other property names are automatically added from
     properties provided by the AddValueProperty and
     AddFunctionProperty methods (if any), then properties vended by
     your class' parent class chain, then properties belonging to your
     JavaScript object's prototype chain.
     
     For example, given this class definition:
     
     class Foo {
     void GetPropertyNames(JSPropertyNameAccumulator& accumulator) const;
     };
     
     You would call the builer like this:
     
     JSClassBuilder<Foo> builder("Foo");
     builder.GetPropertyNames(&Foo::GetPropertyNames);
     
     @result A reference to the builder for chaining.
     */
    JSExportClassDefinitionBuilder<T>& GetPropertyNames(const GetPropertyNamesCallback<T>& get_property_names_callback) HAL_NOEXCEPT {
      HAL_DETAIL_JSEXPORTCLASSDEFINITIONBUILDER_LOCK_GUARD;
	  JSExportClassDefinition<T>::get_property_names_callback__ = get_property_names_callback;
      return *this;
    }
    
    /*!
     @method
     
     @abstract Return the callback to invoke when your JavaScript
     object is called as a function.
     
     @result The callback to invoke when your JavaScript object is
     called as a function.
     */
    CallAsFunctionCallback<T> CallAsFunction() const HAL_NOEXCEPT {
      return JSExportClassDefinition<T>::call_as_function_callback__;
    }
    
    /*!
     @method
     
     @abstract Set the callback to invoke when your JavaScript object
     is called as a function.
     
     @discussion If this callback does not exist, then calling your
     JavaScript object as a function will throw a JavaScript
     exception.
     
     For example, given this class definition:
     
     class Foo {
     JSValue DoSomething(const std::vector<JSValue>& arguments, JSObject& this_object);
     };
     
     You would call the builer like this:
     
     JSExportClassDefinitionBuilder<Foo> builder("Foo");
     builder.Function(&Foo::DoSomething);
     
     In the JavaScript expression 'myObject.myFunction()', then
     'myFunction' is the instance of Foo being called, and 'myObject'
     is the context of the function call (i.e. the 'this' JavaScript
     object).
     
     @result A reference to the builder for chaining.
     */
    JSExportClassDefinitionBuilder<T>& CallAsFunction(const CallAsFunctionCallback<T>& call_as_function_callback) HAL_NOEXCEPT {
      HAL_DETAIL_JSEXPORTCLASSDEFINITIONBUILDER_LOCK_GUARD;
	  JSExportClassDefinition<T>::call_as_function_callback__ = call_as_function_callback;
      return *this;
    }
    
    /*!
     @method
     
     @abstract Return the callback to invoke when converting your
     JavaScript object to another JavaScript type.
     
     @result The callback to invoke when converting your JavaScript
     object to another JavaScript type.
     */
    ConvertToTypeCallback<T> ConvertToType() const HAL_NOEXCEPT {
      return JSExportClassDefinition<T>::convert_to_type_callback__;
    }
    
    /*!
     @method
     
     @abstract Set the callback to invoke when converting your
     JavaScript object to another JavaScript type. This callback is
     only invoked when converting your object to a number or a
     string. An object converted to boolean is 'true.' An object
     converted to object is itself.
     
     @discussion If this callback returns JSUndefined then the
     conversion request forwards to your your class' parent class
     chain, then your JavaScript object's prototype chain.
     
     For example, given this class definition:
     
     class Foo {
     JSValue ConvertToType(JSValue::Type type) const;
     };
     
     You would call the builer like this:
     
     JSExportClassDefinitionBuilder<Foo> builder("Foo");
     builder.ConvertToType(&Foo::ConvertToType);
     
     @result A reference to the builder for chaining.
     */
    JSExportClassDefinitionBuilder<T>& ConvertToType(const ConvertToTypeCallback<T>& convert_to_type_callback) HAL_NOEXCEPT {
      HAL_DETAIL_JSEXPORTCLASSDEFINITIONBUILDER_LOCK_GUARD;
      JSExportClassDefinition<T>::convert_to_type_callback__ = convert_to_type_callback;
      return *this;
    }
    
    /*!
     @method
     
     @abstract Create and return a JSClass instance with all of the
     properties and callbacks specified in this builder.
     
     @result A JSClass instance with all of the properties and
     callbacks specified in this builder.
     */
    JSExportClassDefinition<T> build();
    
  private:
    
    void AddConstantPropertyCallback(const JSExportNamedValuePropertyCallback<T>& value_property_callback);
    void AddValuePropertyCallback(const JSExportNamedValuePropertyCallback<T>& value_property_callback);
    void AddFunctionPropertyCallback(const JSExportNamedFunctionPropertyCallback<T>& function_property_callback);
    
    // JSExportClassDefinition needs access to js_class_definition__ in
    // accordance with the Builder Pattern.
    template<typename U>
    friend class JSExportClassDefinition;
    
    ::JSClassDefinition                           js_class_definition__;
    std::string                                   name__;
    JSClass                                       parent__;

    HAL_DETAIL_JSEXPORTCLASSDEFINITIONBUILDER_MUTEX;
  };

  template<typename T>
  void JSExportClassDefinitionBuilder<T>::AddConstantPropertyCallback(const JSExportNamedValuePropertyCallback<T>& value_property_callback) {
    const std::string internal_component_name = "JSExportClassDefinitionBuilder<" + name__ + ">::AddConstantPropertyCallback";
    const auto property_name                  = value_property_callback.get_name();
    const auto position                       = JSExportClassDefinition<T>::named_constants__.find(property_name);
    const bool found                          = position != JSExportClassDefinition<T>::named_constants__.end();
    
    if (found) {
      const std::string message = "Constant property " + property_name + " already added";
      ThrowInvalidArgument(internal_component_name, message);
    }
    
    const auto callback_insert_result = JSExportClassDefinition<T>::named_constants__.emplace(property_name);
    const bool callback_inserted      = callback_insert_result.second;
    
    assert(callback_inserted);

    AddValuePropertyCallback(value_property_callback);
  } 

  template<typename T>
  void JSExportClassDefinitionBuilder<T>::AddValuePropertyCallback(const JSExportNamedValuePropertyCallback<T>& value_property_callback) {
    const std::string internal_component_name = "JSExportClassDefinitionBuilder<" + name__ + ">::AddValuePropertyCallback";
    const auto property_name                  = value_property_callback.get_name();
    const auto position                       = JSExportClassDefinition<T>::named_value_property_callback_map__.find(property_name);
    const bool found                          = position != JSExportClassDefinition<T>::named_value_property_callback_map__.end();
    
    if (found) {
      const std::string message = "Value property " + property_name + " already added";
      ThrowInvalidArgument(internal_component_name, message);
    }
    
    const auto callback_insert_result = JSExportClassDefinition<T>::named_value_property_callback_map__.emplace(property_name, value_property_callback);
    const bool callback_inserted      = callback_insert_result.second;
    
    // HAL_LOG_DEBUG(internal_component_name, ": value property ", property_name, " inserted = ", callback_inserted);
    assert(callback_inserted);
  }
  
  template<typename T>
  void JSExportClassDefinitionBuilder<T>::AddFunctionPropertyCallback(const JSExportNamedFunctionPropertyCallback<T>& function_property_callback) {
    const std::string internal_component_name = "JSExportClassDefinitionBuilder<" + name__ + ">::AddFunctionPropertyCallback";
    const auto property_name                  = function_property_callback.get_name();
    const auto position                       = JSExportClassDefinition<T>::named_function_property_callback_map__.find(property_name);
    const bool found                          = position != JSExportClassDefinition<T>::named_function_property_callback_map__.end();
    
    if (found) {
      const std::string message = "Function property " + property_name + " already added.";
      ThrowInvalidArgument(internal_component_name, message);
    }
    
    const auto callback_insert_result = JSExportClassDefinition<T>::named_function_property_callback_map__.emplace(property_name, function_property_callback);
    const bool callback_inserted      = callback_insert_result.second;
    
    // HAL_LOG_DEBUG(internal_component_name, ": function property ", property_name, " inserted = ", callback_inserted);
    assert(callback_inserted);
  }
  
  template<typename T>
  JSExportClassDefinition<T> JSExportClassDefinitionBuilder<T>::build() {
    HAL_DETAIL_JSEXPORTCLASSDEFINITIONBUILDER_LOCK_GUARD;
    const std::string internal_component_name = "JSExportClassDefinitionBuilder<" + name__ + ">::build()";
    
    js_class_definition__.className         = name__.c_str();
    js_class_definition__.parentClass       = static_cast<JSClassRef>(parent__);
    js_class_definition__.initialize        = JSExportClass<T>::JSObjectInitializeCallback;
    js_class_definition__.finalize          = JSExportClass<T>::JSObjectFinalizeCallback;
    js_class_definition__.callAsConstructor = JSExportClass<T>::JSObjectCallAsConstructorCallback;
    js_class_definition__.hasInstance       = JSExportClass<T>::JSObjectHasInstanceCallback;
    
    if (JSExportClassDefinition<T>::has_property_callback__) {
      js_class_definition__.hasProperty = JSExportClass<T>::JSObjectHasPropertyCallback;
    }
    
    if (JSExportClassDefinition<T>::get_property_callback__) {
      js_class_definition__.getProperty = JSExportClass<T>::JSObjectGetPropertyCallback;
    }
    
    if (JSExportClassDefinition<T>::set_property_callback__) {
      js_class_definition__.setProperty = JSExportClass<T>::JSObjectSetPropertyCallback;
    }
    
    if (JSExportClassDefinition<T>::delete_property_callback__) {
      js_class_definition__.deleteProperty = JSExportClass<T>::JSObjectDeletePropertyCallback;
    }
    
    if (JSExportClassDefinition<T>::get_property_names_callback__) {
      js_class_definition__.getPropertyNames = JSExportClass<T>::JSObjectGetPropertyNamesCallback;
    }
    
    if (JSExportClassDefinition<T>::call_as_function_callback__) {
      js_class_definition__.callAsFunction = JSExportClass<T>::JSObjectCallAsFunctionCallback;
    }
    
    if (JSExportClassDefinition<T>::convert_to_type_callback__) {
      js_class_definition__.convertToType = JSExportClass<T>::JSObjectConvertToTypeCallback;
    }
    
    return JSExportClassDefinition<T>(*this);
  }

  template<typename T>
  std::unordered_set<std::string> JSExportClassDefinition<T>::named_constants__;

  template<typename T>
  JSExportNamedValuePropertyCallbackMap_t<T> JSExportClassDefinition<T>::named_value_property_callback_map__;

  template<typename T>
  JSExportNamedFunctionPropertyCallbackMap_t<T> JSExportClassDefinition<T>::named_function_property_callback_map__;

  template<typename T>
  HasPropertyCallback<T> JSExportClassDefinition<T>::has_property_callback__{ nullptr };

  template<typename T>
  GetPropertyCallback<T> JSExportClassDefinition<T>::get_property_callback__{ nullptr };

  template<typename T>
  SetPropertyCallback<T> JSExportClassDefinition<T>::set_property_callback__{ nullptr };

  template<typename T>
  DeletePropertyCallback<T> JSExportClassDefinition<T>::delete_property_callback__{ nullptr };

  template<typename T>
  GetPropertyNamesCallback<T> JSExportClassDefinition<T>::get_property_names_callback__{ nullptr };

  template<typename T>
  CallAsFunctionCallback<T> JSExportClassDefinition<T>::call_as_function_callback__{ nullptr };

  template<typename T>
  ConvertToTypeCallback<T> JSExportClassDefinition<T>::convert_to_type_callback__{ nullptr };

  template<typename T>
  JSExportClassDefinition<T>::JSExportClassDefinition(const JSExportClassDefinitionBuilder<T>& builder)
  : JSClassDefinition(builder.js_class_definition__) {
	  InitializeNamedPropertyCallbacks();
  }
  
}} // namespace HAL { namespace detail {

#endif // _HAL_DETAIL_JSEXPORTCLASSDEFINITIONBUILDER_HPP_
