/**
 * HAL
 *
 * Copyright (c) 2014 by Appcelerator, Inc. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "HAL/HAL.hpp"
#include <unordered_set>
#include <limits>
#include <cstdint>
#import <XCTest/XCTest.h>

using namespace HAL;

namespace UnitTestConstants {
  static const double pi { 3.141592653589793 };
}

@interface JSObjectTests : XCTestCase
@end

@implementation JSObjectTests {
  JSContextGroup js_context_group;
}

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testObjectSizes {
  XCTAssertEqual(sizeof(std::intptr_t)                         , sizeof(JSContextGroup));
  XCTAssertEqual(sizeof(JSContextGroup) + sizeof(std::intptr_t), sizeof(JSContext));
  
  // JSValue and JSObject are base classes, so have an extra pointer for the
  // virtual function table.
  XCTAssertEqual(sizeof(JSContext) + sizeof(std::intptr_t) + sizeof(std::intptr_t) + sizeof(std::intptr_t), sizeof(JSValue));
  XCTAssertEqual(sizeof(JSContext) + sizeof(std::intptr_t) + sizeof(std::intptr_t), sizeof(JSObject));
}

- (void)testJSPropertyAttribute {
  std::unordered_set<JSPropertyAttribute> attributes;
  XCTAssertEqual(0, attributes.count(JSPropertyAttribute::None));
  XCTAssertEqual(0, attributes.count(JSPropertyAttribute::ReadOnly));
  XCTAssertEqual(0, attributes.count(JSPropertyAttribute::DontEnum));
  XCTAssertEqual(0, attributes.count(JSPropertyAttribute::DontDelete));
  
  const auto insert_result = attributes.insert(JSPropertyAttribute::DontDelete);
  XCTAssertTrue(insert_result.second);
  XCTAssertEqual(1, attributes.count(JSPropertyAttribute::DontDelete));
  XCTAssertEqual(*insert_result.first, JSPropertyAttribute::DontDelete);

  XCTAssertEqual(1, attributes.size());
}

- (void)testAPI {
  JSContext js_context = js_context_group.CreateContext();
  JSObject js_object = js_context.CreateObject();
  JSValue prototype = js_object.GetPrototype();
  XCTAssertFalse(prototype.IsUndefined());
  XCTAssertFalse(prototype.IsNull());
  XCTAssertFalse(prototype.IsBoolean());
  XCTAssertFalse(prototype.IsNumber());
  XCTAssertFalse(prototype.IsString());
  XCTAssertTrue(prototype.IsObject());

  XCTAssertFalse(js_object.HasProperty("foo"));
  XCTAssertTrue(js_object.GetProperty("foo").IsUndefined());
  
  js_object.SetProperty("foo", js_context.CreateNumber(42));
  XCTAssertTrue(js_object.HasProperty("foo"));
  JSValue foo = js_object.GetProperty("foo");
  XCTAssertTrue(foo.IsNumber());
  XCTAssertEqual(42, static_cast<int32_t>(foo));
  XCTAssertTrue(js_object.DeleteProperty("foo"));

  js_object.SetProperty("bar", js_context.CreateBoolean(true), {JSPropertyAttribute::DontDelete});
  XCTAssertTrue(js_object.HasProperty("bar"));
  JSValue bar = js_object.GetProperty("bar");
  XCTAssertTrue(bar.IsBoolean());
  XCTAssertTrue(static_cast<bool>(bar));
  XCTAssertFalse(js_object.DeleteProperty("bar"));
  XCTAssertTrue(js_object.HasProperty("bar"));
  bar = js_object.GetProperty("bar");
  XCTAssertTrue(static_cast<bool>(bar));

  XCTAssertTrue(js_object.GetProperty(42).IsUndefined());
  js_object.SetProperty(42, js_context.CreateNumber(UnitTestConstants::pi));
  JSValue pi = js_object.GetProperty(42);
  XCTAssertTrue(pi.IsNumber());
  XCTAssertEqualWithAccuracy(UnitTestConstants::pi, static_cast<double>(pi), std::numeric_limits<double>::epsilon());
  
  // You can't call an JSObject as a function that isn't a function.
  XCTAssertFalse(js_object.IsFunction());
  try {
    js_object(js_object);
    XCTFail("js_object was called as a function but did not throw a std::runtime_error exception");
  } catch (const std::runtime_error& exception) {
    XCTAssert(YES, @"Caught expected std::runtime_error exception.");
  } catch (...) {
    XCTFail("Caught unexpected unknown exception, but we expected a std::runtime_error exception.");
  }
  
  // You can't call a JSObject as a constructor that isn't a constructor.
  XCTAssertFalse(js_object.IsConstructor());
  try {
    js_object.CallAsConstructor();
    XCTFail("js_object was called as a constructor but did not throw a std::runtime_error exception");
  } catch (const std::runtime_error& exception) {
    XCTAssert(YES, @"Caught expected std::runtime_error exception.");
  } catch (...) {
    XCTFail("Caught unexpected unknown exception, but we expected a std::runtime_error exception.");
  }

  XCTAssertFalse(js_object.IsArray());
  XCTAssertFalse(js_object.IsError());
  
  auto js_value  = js_context.JSEvaluateScript("new Object()");
  XCTAssertTrue(js_value.IsObject());
  js_object = static_cast<JSObject>(js_value);
  XCTAssertFalse(js_object.IsArray());
  XCTAssertFalse(js_object.IsError());

  js_value  = js_context.JSEvaluateScript("Object()");
  XCTAssertTrue(js_value.IsObject());
  js_object = static_cast<JSObject>(js_value);
  XCTAssertFalse(js_object.IsArray());
  XCTAssertFalse(js_object.IsError());
  
  // It is surprising to me that an object literal, "{}", is not an object.
  js_value  = js_context.JSEvaluateScript("{}");
  XCTAssertTrue(js_value.IsUndefined());

  // But this is an object.
  js_value  = js_context.JSEvaluateScript("var a = {}; a");
  XCTAssertTrue(js_value.IsObject());
  js_object = static_cast<JSObject>(js_value);
  XCTAssertFalse(js_object.IsArray());
  XCTAssertFalse(js_object.IsError());
  
  // This is not a primitive string.
  js_value  = js_context.JSEvaluateScript("new String()");
  XCTAssertTrue(js_value.IsObject());
  XCTAssertFalse(js_value.IsString());
  js_object = static_cast<JSObject>(js_value);
  XCTAssertFalse(js_object.IsArray());
  XCTAssertFalse(js_object.IsError());

  // Yet this is a primitive string (i.e. without new).
  js_value  = js_context.JSEvaluateScript("String()");
  XCTAssertFalse(js_value.IsObject());
  XCTAssertTrue(js_value.IsString());

  js_value  = js_context.JSEvaluateScript("new Date()");
  XCTAssertTrue(js_value.IsObject());
  js_object = static_cast<JSObject>(js_value);
  XCTAssertFalse(js_object.IsArray());
  XCTAssertFalse(js_object.IsError());

  js_value  = js_context.JSEvaluateScript("new Array()");
  XCTAssertTrue(js_value.IsObject());
  js_object = static_cast<JSObject>(js_value);
  XCTAssertTrue(js_object.IsArray());
  XCTAssertFalse(js_object.IsError());
  
  js_value  = js_context.JSEvaluateScript("new Error()");
  XCTAssertTrue(js_value.IsObject());
  js_object = static_cast<JSObject>(js_value);
  XCTAssertFalse(js_object.IsArray());
  XCTAssertTrue(js_object.IsError());

  // An array literal is an Object, as expected. Why isn't an object literal,
  // "{}", an Object?
  js_value  = js_context.JSEvaluateScript("[]");
  XCTAssertTrue(js_value.IsObject());
  js_object = static_cast<JSObject>(js_value);
  XCTAssertTrue(js_object.IsArray());

  js_value  = js_context.JSEvaluateScript("[1, 3, 5, 7]");
  XCTAssertTrue(js_value.IsObject());
  js_object = static_cast<JSObject>(js_value);
  XCTAssertTrue(js_object.IsArray());
}

- (void)testProperty {
  JSContext js_context = js_context_group.CreateContext();
  auto js_value  = js_context.JSEvaluateScript("[1, 3, 5, 7]");
  XCTAssertTrue(js_value.IsObject());
  JSObject js_object = static_cast<JSObject>(js_value);
  XCTAssertTrue(js_object.IsArray());
  XCTAssertTrue(js_object.HasProperty("length"));
  js_value = js_object.GetProperty("length");
  XCTAssertEqual(4, static_cast<int32_t>(js_value));

  js_value  = js_context.JSEvaluateScript("'hello, JavaScript'");
  XCTAssertTrue(js_value.IsString());
  js_object = static_cast<JSObject>(js_value);
  XCTAssertTrue(js_object.HasProperty("length"));
  js_value = js_object.GetProperty("length");
  XCTAssertEqual(17, static_cast<int32_t>(js_value));
  
  // Trying to reference an undefined property returns undefined.
  XCTAssertFalse(js_object.HasProperty("foo"));
  js_value = js_object.GetProperty("foo");
  XCTAssertTrue(js_value.IsUndefined());

  js_value = js_context.JSEvaluateScript("new Object()");
  XCTAssertTrue(js_value.IsObject());
  js_object = static_cast<JSObject>(js_value);
  XCTAssertFalse(js_object.HasProperty("foo"));
  
  js_object.SetProperty("foo", js_context.CreateNumber(42));
  XCTAssertTrue(js_object.HasProperty("foo"));
  js_value = js_object.GetProperty("foo");
  XCTAssertEqual(42, static_cast<int32_t>(js_value));

  // Should be able to delete a property we created.
  XCTAssertTrue(js_object.DeleteProperty("foo"));

  // It is legal to delete a non-existent property.
  XCTAssertTrue(js_object.DeleteProperty("bar"));
  
  const std::string quoteString = "Life can only be understood backwards; but it must be lived forwards.";
  
  // You can set a custom propery on a string.
  js_value = js_context.CreateString("hello, world");
  js_object = static_cast<JSObject>(js_value);
  XCTAssertFalse(js_object.IsArray());
  js_object.SetProperty("quote", js_context.CreateString(quoteString));
  XCTAssertTrue(js_object.HasProperty("quote"));
  js_value = js_object.GetProperty("quote");
  XCTAssertEqual(quoteString, static_cast<std::string>(js_value));

  // You can set a custom propery on a bool.
  js_value = js_context.CreateBoolean(true);
  js_object = static_cast<JSObject>(js_value);
  XCTAssertFalse(js_object.IsArray());
  js_object.SetProperty("quote", js_context.CreateString(quoteString));
  XCTAssertTrue(js_object.HasProperty("quote"));
  js_value = js_object.GetProperty("quote");
  XCTAssertEqual(quoteString, static_cast<std::string>(js_value));

  // You can set a custom propery on a number.
  js_value = js_context.CreateNumber(42);
  js_object = static_cast<JSObject>(js_value);
  XCTAssertFalse(js_object.IsArray());
  js_object.SetProperty("quote", js_context.CreateString(quoteString));
  XCTAssertTrue(js_object.HasProperty("quote"));
  js_value = js_object.GetProperty("quote");
  XCTAssertEqual(quoteString, static_cast<std::string>(js_value));

  // You can set a custom propery on an array.
  js_value = js_context.CreateArray();
  js_object = static_cast<JSObject>(js_value);
  XCTAssertTrue(js_object.IsArray());
  js_object.SetProperty("quote", js_context.CreateString(quoteString));
  XCTAssertTrue(js_object.HasProperty("quote"));
  js_value = js_object.GetProperty("quote");
  XCTAssertEqual(quoteString, static_cast<std::string>(js_value));

  // You can set a custom propery on an regexp.
  js_value = js_context.CreateRegExp();
  js_object = static_cast<JSObject>(js_value);
  XCTAssertFalse(js_object.IsArray());
  js_object.SetProperty("quote", js_context.CreateString(quoteString));
  XCTAssertTrue(js_object.HasProperty("quote"));
  js_value = js_object.GetProperty("quote");
  XCTAssertEqual(quoteString, static_cast<std::string>(js_value));

  // You can set a custom propery on an error.
  js_value = js_context.CreateError();
  js_object = static_cast<JSObject>(js_value);
  XCTAssertFalse(js_object.IsArray());
  js_object.SetProperty("quote", js_context.CreateString(quoteString));
  XCTAssertTrue(js_object.HasProperty("quote"));
  js_value = js_object.GetProperty("quote");
  XCTAssertEqual(quoteString, static_cast<std::string>(js_value));

  js_object.SetProperty("quote", js_context.CreateString(quoteString));
  XCTAssertTrue(js_object.HasProperty("quote"));
  js_value = js_object.GetProperty("quote");
  XCTAssertEqual(quoteString, static_cast<std::string>(js_value));
}

- (void)testJSArray {
  JSContext js_context = js_context_group.CreateContext();
  JSArray js_array = js_context.CreateArray();
  XCTAssertTrue(js_array.IsArray());
  XCTAssertFalse(js_array.IsError());
}

- (void)testGetProperties {
  JSContext js_context = js_context_group.CreateContext();

  std::unordered_map<std::string, JSValue> properties = { 
    { "str",    js_context.CreateString("Hello") },
    { "num",    js_context.CreateNumber(123) },
    { "bool",   js_context.CreateBoolean(true) },
    { "object", js_context.CreateObject() }
  };

  JSObject js_object = js_context.CreateObject(properties);

  auto js_properties = js_object.GetProperties();

  XCTAssertTrue(js_properties.find("str")    != js_properties.end());
  XCTAssertTrue(js_properties.find("num")    != js_properties.end());
  XCTAssertTrue(js_properties.find("bool")   != js_properties.end());
  XCTAssertTrue(js_properties.find("object") != js_properties.end());

  XCTAssertTrue(js_properties.at("str").IsString());
  XCTAssertTrue(js_properties.at("num").IsNumber());
  XCTAssertTrue(js_properties.at("bool").IsBoolean());
  XCTAssertTrue(js_properties.at("object").IsObject());
}

- (void)testJSObjectToJSArray {
  JSContext js_context = js_context_group.CreateContext();

  std::vector<JSValue> args = { 
    js_context.CreateString("Hello"),
    js_context.CreateNumber(123),
    js_context.CreateBoolean(true),
    js_context.CreateObject()
  };

  JSObject js_object = js_context.CreateArray(args);
  XCTAssertTrue(js_object.IsArray());
  JSArray js_array = static_cast<JSArray>(js_object);
  XCTAssertTrue(js_array.IsArray());
  auto items = static_cast<std::vector<JSValue>>(js_array);

  XCTAssertEqual(4, js_array.GetLength());
  XCTAssertEqual(4, items.size());

  XCTAssertTrue(items.at(0).IsString());
  XCTAssertTrue(items.at(1).IsNumber());
  XCTAssertTrue(items.at(2).IsBoolean());
  XCTAssertTrue(items.at(3).IsObject());

  auto export_items = js_array.GetPrivateItems<JSExportObject>();
  XCTAssertEqual(4, export_items.size());
  XCTAssertEqual(nullptr, export_items.at(0));
  XCTAssertEqual(nullptr, export_items.at(1));
  XCTAssertEqual(nullptr, export_items.at(2));
  XCTAssertEqual(nullptr, export_items.at(3));
}

- (void)testStringVectorFromJSArray {
  JSContext js_context = js_context_group.CreateContext();

  std::vector<JSValue> args = { 
    js_context.CreateString("Hello 1"),
    js_context.CreateString("Hello 2"),
    js_context.CreateString("Hello 3")
  };

  JSObject js_object = js_context.CreateArray(args);
  XCTAssertTrue(js_object.IsArray());
  JSArray js_array = static_cast<JSArray>(js_object);
  XCTAssertTrue(js_array.IsArray());
  auto items = static_cast<std::vector<std::string>>(js_array);

  XCTAssertEqual(3, js_array.GetLength());
  XCTAssertEqual(3, items.size());

  XCTAssertEqual("Hello 1", items.at(0));
  XCTAssertEqual("Hello 2", items.at(1));
  XCTAssertEqual("Hello 3", items.at(2));
}

- (void)testBoolVectorFromJSArray {
  JSContext js_context = js_context_group.CreateContext();

  std::vector<JSValue> args = { 
    js_context.CreateBoolean(true),
    js_context.CreateBoolean(false),
    js_context.CreateBoolean(true)
  };

  JSObject js_object = js_context.CreateArray(args);
  XCTAssertTrue(js_object.IsArray());
  JSArray js_array = static_cast<JSArray>(js_object);
  XCTAssertTrue(js_array.IsArray());
  auto items = static_cast<std::vector<bool>>(js_array);

  XCTAssertEqual(3, js_array.GetLength());
  XCTAssertEqual(3, items.size());

  XCTAssertTrue(items.at(0));
  XCTAssertFalse(items.at(1));
  XCTAssertTrue(items.at(2));
}

- (void)testDoubleVectorFromJSArray {
  JSContext js_context = js_context_group.CreateContext();

  std::vector<JSValue> args = { 
    js_context.CreateNumber(1.1),
    js_context.CreateNumber(1.12),
    js_context.CreateNumber(1.123)
  };

  JSObject js_object = js_context.CreateArray(args);
  XCTAssertTrue(js_object.IsArray());
  JSArray js_array = static_cast<JSArray>(js_object);
  XCTAssertTrue(js_array.IsArray());
  auto items = static_cast<std::vector<double>>(js_array);

  XCTAssertEqual(3, js_array.GetLength());
  XCTAssertEqual(3, items.size());

  XCTAssertEqual(1.1, items.at(0));
  XCTAssertEqual(1.12, items.at(1));
  XCTAssertEqual(1.123, items.at(2));
}

- (void)testIntVectorFromJSArray {
  JSContext js_context = js_context_group.CreateContext();

  std::vector<JSValue> args = { 
    js_context.CreateNumber(123),
    js_context.CreateNumber(123.4),
    js_context.CreateNumber(-123)
  };

  JSObject js_object = js_context.CreateArray(args);
  XCTAssertTrue(js_object.IsArray());
  JSArray js_array = static_cast<JSArray>(js_object);
  XCTAssertTrue(js_array.IsArray());
  auto items = static_cast<std::vector<std::int32_t>>(js_array);

  XCTAssertEqual(3, js_array.GetLength());
  XCTAssertEqual(3, items.size());

  XCTAssertEqual(123, items.at(0));
  XCTAssertEqual(123, items.at(1));
  XCTAssertEqual(-123, items.at(2));
}

- (void)testUIntVectorFromJSArray {
  JSContext js_context = js_context_group.CreateContext();

  std::vector<JSValue> args = { 
    js_context.CreateNumber(123),
    js_context.CreateNumber(123.4)
  };

  JSObject js_object = js_context.CreateArray(args);
  XCTAssertTrue(js_object.IsArray());
  JSArray js_array = static_cast<JSArray>(js_object);
  XCTAssertTrue(js_array.IsArray());
  auto items = static_cast<std::vector<std::uint32_t>>(js_array);

  XCTAssertEqual(2, js_array.GetLength());
  XCTAssertEqual(2, items.size());

  XCTAssertEqual(123, items.at(0));
  XCTAssertEqual(123, items.at(1));
}

- (void)testJSDate {
  JSContext js_context = js_context_group.CreateContext();
  JSDate js_date = js_context.CreateDate();
  XCTAssertFalse(js_date.IsArray());
  XCTAssertFalse(js_date.IsError());
}

- (void)testJSError {
  JSContext js_context = js_context_group.CreateContext();
  JSError js_error = js_context.CreateError();
  XCTAssertFalse(js_error.IsArray());
  XCTAssertTrue(js_error.IsError());
}

- (void)testJSRegExp {
  JSContext js_context = js_context_group.CreateContext();
  JSRegExp js_regexp = js_context.CreateRegExp();
  XCTAssertFalse(js_regexp.IsArray());
  XCTAssertFalse(js_regexp.IsError());
}

- (void)testJSFunction {
  JSContext js_context = js_context_group.CreateContext();
  JSFunction js_function = js_context.CreateFunction("return 'Hello, ' + name;", {"name"});
  XCTAssertTrue(js_function.IsFunction());
  //std::clog << "MDL: js_function(\"world\") = " << js_function("world") << std::endl;
  XCTAssertEqual("Hello, world", static_cast<std::string>(js_function("world", js_function)));
  XCTAssertFalse(js_function.IsArray());
  XCTAssertFalse(js_function.IsError());
}

- (void)testJSFunctionCallback {
  JSContext js_context = js_context_group.CreateContext();
  JSFunctionCallback callback = [js_context](const std::vector<JSValue> arguments, JSObject& this_object) {
    return js_context.CreateString("Hello, "+static_cast<std::string>(arguments.at(0)));
  };

  JSFunction js_function = js_context.CreateFunction(callback);

  XCTAssertTrue(js_function.IsFunction());
  XCTAssertEqual("Hello, world", static_cast<std::string>(js_function("world", js_function)));
  XCTAssertFalse(js_function.IsArray());
  XCTAssertFalse(js_function.IsError());

  auto global_object = js_context.get_global_object();
  global_object.SetProperty("testJSFunctionCallback", js_function);

  XCTAssertEqual("Hello, JavaScript", static_cast<std::string>(js_context.JSEvaluateScript("testJSFunctionCallback('JavaScript');")));

  // testing NOOP function
  JSFunction noop_function = js_context.CreateFunction();
  XCTAssertTrue(noop_function(noop_function).IsUndefined());

}

@end
