//
//  JSStringTests.m
//  JavaScriptCoreCPP
//
//  Created by Matt Langston on 10/18/14.
//  Copyright (c) 2014 Appcelerator. All rights reserved.
//

#include "JavaScriptCoreCPP/RAII/JSString.hpp"
#import <XCTest/XCTest.h>

using namespace JavaScriptCoreCPP;

JSString makeJSString() {
  JSString js_string;
  return js_string;
}

@interface JSStringTests2 : XCTestCase
@end

@implementation JSStringTests2

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testJSString {
  JSString js_string_1;
  JSString js_string_2;
  XCTAssertEqual(js_string_1, js_string_2);
  
  // Test copy constructor.
  JSString js_string_3(js_string_1);
  XCTAssertEqual(js_string_1, js_string_3);
  
  // Test copy assignment.
  JSString js_string_4 = js_string_1;
  XCTAssertEqual(js_string_1, js_string_4);
  
  // Test move constructor.
  JSString js_string_5(makeJSString());
  
  // Test unified assignment operator
  JSString js_string_6 = js_string_1;
  XCTAssertEqual(js_string_1, js_string_6);
}

- (void)testEmptyString {
  JSString string;
  XCTAssertEqual(0, string.length());
  
  std::string stdString = string;
  XCTAssertEqual(0, stdString.length());
  XCTAssertTrue(stdString.empty());
}

- (void)testEqual {
  JSString string1 { "hello, world" };
  JSString string2 = string1;
  XCTAssertEqual(string1, string2);
  
  JSString string3 { "hello" };
  XCTAssertNotEqual(string1, string3);
}

- (void)testStdString {
  JSString string1 { "hello, JSString" };
  XCTAssertEqual("hello, JSString", static_cast<std::string>(string1));
  XCTAssertEqual(std::string("hello, JSString"), string1);
  
  std::string string2 { "hello, std::string" };
  XCTAssertEqual("hello, std::string", static_cast<std::string>(JSString(string2)));
  XCTAssertEqual(std::string("hello, std::string"), JSString(string2));
}

- (void)testJSStringCreationPerformance {
  [self measureBlock:^{
    // How long does it take to create a JSString?
    JSString string;
  }];
}

@end