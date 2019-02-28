/**
 * HAL
 *
 * Copyright (c) 2019 by Axway. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _HAL_DETAIL_JSUTIL_HPP_
#define _HAL_DETAIL_JSUTIL_HPP_

#include "HAL/detail/JSBase.hpp"
#include "HAL/JSPropertyAttribute.hpp"
#include <unordered_set>
#include <vector>

namespace HAL {

	class JSValue;

	namespace detail {

		class js_runtime_error : public std::runtime_error {
		public:
			js_runtime_error() HAL_NOEXCEPT = default;
			virtual ~js_runtime_error() = default;

			std::string js_name() const {
				return js_name__;
			}
			std::string js_message() const {
				return js_message__;
			}
			std::string js_filename() const {
				return js_filename__;
			}
			std::uint32_t js_linenumber() const {
				return js_linenumber__;
			}
			std::string js_stack() const {
				return js_stack__;
			}
			std::string js_nativeStack() const {
				return js_nativeStack__;
			}
		private:
			std::string js_name__;
			std::string js_message__;
			std::string js_filename__;
			std::uint32_t js_linenumber__;
			std::string js_stack__;
			std::string js_nativeStack__;
		};

		HAL_EXPORT void ThrowRuntimeError(const std::string& message);
		HAL_EXPORT void ThrowRuntimeError(const std::string& internal_component_name, const std::string& message);
		HAL_EXPORT std::vector<JSValueRef> to_vector(const std::vector<JSValue>&);
		HAL_EXPORT std::int32_t to_int32_t(double number);
		HAL_EXPORT unsigned ToJSPropertyAttributes(const std::unordered_set<JSPropertyAttribute>& attributes) HAL_NOEXCEPT;

	}
} // namespace HAL { namespace detail {

#endif // _HAL_DETAIL_JSUTIL_HPP_
