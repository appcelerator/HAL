/**
 * HAL
 *
 * Copyright (c) 2019 by Axway. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _HAL_JSPROPERTYATTRIBUTE_HPP_
#define _HAL_JSPROPERTYATTRIBUTE_HPP_

#include "HAL/detail/JSBase.hpp"
#include <cstddef>
#include <functional>

namespace HAL {

	/*!
	  @enum

	  @constant None Specifies that a property has no special attributes.

	  @constant ReadOnly Specifies that a property is read-only.

	  @constant DontEnum Specifies that a property should not be
	  enumerated by JSPropertyEnumerators and JavaScript for...in loops.

	  @constant DontDelete Specifies that the delete operation should fail
	  on a property.
	*/
	enum class HAL_EXPORT JSPropertyAttribute : std::uint32_t /* typedef unsigned JSPropertyAttributes; */ {
		None,
		ReadOnly,
		DontEnum,
		DontDelete
	};

} // namespace HAL {


// Provide a hash function so that a JSPropertyAttribute can be
// stored in an unordered container.
namespace std {

	using HAL::JSPropertyAttribute;

	template<>
	struct hash<JSPropertyAttribute> {
		using argument_type = JSPropertyAttribute;
		using result_type = std::size_t;
		using underlying_type = std::underlying_type<argument_type>::type;
		std::hash<underlying_type> hash_function = std::hash<underlying_type>();

		result_type operator()(const argument_type& property_attribute) const {
			return hash_function(static_cast<underlying_type>(property_attribute));
		}
	};

}  // namespace std {

#endif // _HAL_JSPROPERTYATTRIBUTE_HPP_
