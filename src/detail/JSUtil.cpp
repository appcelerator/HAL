/**
 * HAL
 *
 * Copyright (c) 2019 by Axway. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "HAL/detail/JSUtil.hpp"
#include <codecvt>

namespace HAL {
	namespace detail {

		std::wstring to_wstring(const std::string& src) {
			std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
			return converter.from_bytes(src);
		}

		void ThrowRuntimeError(const std::string& internal_component_name, const std::string& message) {
			throw std::runtime_error(message);
		}

		void ThrowRuntimeError(const std::string& message) {
			throw std::runtime_error(message);
		}

	}
} // namespace HAL { namespace detail {
