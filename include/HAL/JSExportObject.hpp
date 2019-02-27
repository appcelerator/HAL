/**
* HAL
*
* Copyright (c) 2018 by Axway. All Rights Reserved.
* Licensed under the terms of the Apache Public License.
* Please see the LICENSE included with this distribution for details.
*/

#ifndef _HAL_JSEXPORTOBJECT_HPP_
#define _HAL_JSEXPORTOBJECT_HPP_

#include "HAL/detail/JSBase.hpp"
#include "HAL/JSContext.hpp"
#include "HAL/JSObject.hpp"

namespace HAL {

	class HAL_EXPORT JSExportObject {
	public:
		JSExportObject(const JSContext& js_context) HAL_NOEXCEPT;

		virtual ~JSExportObject() HAL_NOEXCEPT {
			if (ctor_func__) {
				JsRelease(ctor_func__, nullptr);
			}
		}
		virtual void postInitialize(JSObject& js_object);
		virtual void postCallAsConstructor(const JSContext&, const std::vector<JSValue>&);

		virtual JSObject get_object() HAL_NOEXCEPT;

		virtual JSContext get_context() const {
			return js_context__;
		}

		virtual JsValueRef get_constructor() {
			return ctor_func__;
		}

		virtual void set_constructor(const JsValueRef ctor_func) {
			ctor_func__ = ctor_func;
			JsAddRef(ctor_func__, nullptr);
		}

		static void JSExportInitialize();

	protected:
		JSContext js_context__;

		//
		// Constructor function for this object.
		// Used only when this object is acting as a constructor.
		// 
		JsValueRef ctor_func__{ nullptr };
	};

} // namespace HAL {

#endif // _HAL_JSEXPORTOBJECT_HPP_
