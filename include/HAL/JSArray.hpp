/**
 * HAL
 *
 * Copyright (c) 2018 by Axway. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _HAL_JSARRAY_HPP_
#define _HAL_JSARRAY_HPP_

#include "HAL/JSObject.hpp"
#include "HAL/JSValue.hpp"
#include <vector>

namespace HAL {

/*!
  @class
  
  @discussion A JavaScript object of the Array type.
  
  The only way to create a JSArray is by using the
  JSContext::CreateArray member function.
*/
class HAL_EXPORT JSArray final : public JSObject {

public:
    /*!
     @method
     
     @abstract Convert this JSArray to a std::vector<JSValue>
     
     @result An array of JSValue with the result of conversion.
     */
    virtual operator std::vector<JSValue>() const final;

    /*!
     @method
     
     @abstract Convert this JSArray to a std::vector<bool>
     
     @result An array of bool with the result of conversion.
     */
    virtual operator std::vector<bool>() const final;

    /*!
     @method
     
     @abstract Convert this JSArray to a std::vector<std::string>
     
     @result An array of std::string with the result of conversion.
     */
    virtual operator std::vector<std::string>() const final;

    /*!
     @method
     
     @abstract Convert this JSArray to a std::vector<double>
     
     @result An array of double with the result of conversion.
     */
    virtual operator std::vector<double>() const final;

    /*!
     @method
     
     @abstract Convert this JSArray to a std::vector<int32_t>
     
     @result An array of int32_t with the result of conversion.
     */
    virtual operator std::vector<int32_t>() const final;

    /*!
     @method
     
     @abstract Convert this JSArray to a std::vector<uint32_t>
     
     @result An array of uint32_t with the result of conversion.
     */
    virtual operator std::vector<uint32_t>() const final;

    /*!
     @method
     
     @abstract Return length of this JSArray
     The length is converted to an uint32_t according to
     the rules specified in ECMA-262 15.4.
     
     @result Length of this JSArray
     */
    virtual uint32_t GetLength() const HAL_NOEXCEPT final;
	virtual uint32_t GetCount()  const HAL_NOEXCEPT final;

    /*!
     @method
     
     @abstract Return an array of std::shared_ptr<T> to this object's private data.
     
     @result An array of std::shared_ptr<T> to this object's private data if the
     object has private data of type T*, otherwise nullptr.
     */
    template<typename T>
    std::vector<std::shared_ptr<T>> GetPrivateItems() const HAL_NOEXCEPT;

private:

	// Only JSContext and JSObject can create a JSArray.
	friend JSContext;
	friend JSObject;
	
	JSArray(JsValueRef js_object_ref);
	JSArray(JsValueRef js_object_ref, const std::vector<JSValue>& arguments);
};

template<typename T>
std::vector<std::shared_ptr<T>> JSArray::GetPrivateItems() const HAL_NOEXCEPT {
	const uint32_t length = static_cast<uint32_t>(GetProperty("length"));
	std::vector<std::shared_ptr<T>> items(length);
	for (uint32_t i = 0; i < length; i++) {
		const JSValue js_item_prop = GetProperty(i);
		if (js_item_prop.IsObject()) {
			const JSObject js_item = static_cast<JSObject>(js_item_prop);
			items.at(i) = js_item.GetPrivate<T>();
		} else {
			items.at(i) = nullptr;
		}
	}
	return items;
}

} // namespace HAL {

#endif // _HAL_JSARRAY_HPP_
