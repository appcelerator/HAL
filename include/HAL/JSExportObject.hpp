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

		}
		virtual void postInitialize(JSObject& js_object);
		virtual void postCallAsConstructor(const JSContext&, const std::vector<JSValue>&);

		virtual JSObject get_object() HAL_NOEXCEPT;

		virtual JSContext get_context() {
			return js_context__;
		}

		static void JSExportInitialize();

	protected:
		JSContext js_context__;
	};

} // namespace HAL {

#endif // _HAL_JSEXPORTOBJECT_HPP_
