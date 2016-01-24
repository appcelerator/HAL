/**
 * HAL
 *
 * Copyright (c) 2015 by Appcelerator, Inc. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _HAL_EXAMPLES_CHILDWIDGET_HPP_
#define _HAL_EXAMPLES_CHILDWIDGET_HPP_

#include "HAL/HAL.hpp"
#include <string>
#include "Widget.hpp"

using namespace HAL;

/*!
 @class
 
 @discussion This is an example of how to create a JavaScript object
 implemented by a C++ class.
 */
class ChildWidget : public Widget, public JSExport<ChildWidget> {
  
public:
  
  ChildWidget(const JSContext& js_context) HAL_NOEXCEPT;
  virtual ~ChildWidget() HAL_NOEXCEPT;
  
  static void JSExportInitialize();
  
  virtual void postInitialize(JSObject& js_object) override;
  virtual void postCallAsConstructor(const JSContext& js_context, const std::vector<JSValue>& arguments) override;
  
  JSValue js_get_name() const HAL_NOEXCEPT;
  JSValue js_get_my_name() const HAL_NOEXCEPT;
  JSValue js_get_pi() HAL_NOEXCEPT;
  
  std::uint32_t get_count_for_child_pi() {
    return count_for_child_pi__;
  }
private:
  std::uint32_t count_for_child_pi__ { 0 };
};

inline
void swap(ChildWidget& first, ChildWidget& second) HAL_NOEXCEPT {
  first.swap(second);
}

#endif // _HAL_EXAMPLES_CHILDWIDGET_HPP_
