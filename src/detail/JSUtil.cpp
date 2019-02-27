/**
 * HAL
 *
 * Copyright (c) 2018 by Axway. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "HAL/detail/JSUtil.hpp"
#include "HAL/JSValue.hpp"
#include "HAL/JSObject.hpp"
#include <algorithm>
#include <locale>
#include <codecvt>
#include <mutex>

namespace HAL {
	namespace detail {

		std::wstring to_wstring(const std::string& src) {
			std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
			return converter.from_bytes(src);
		}

		JsContextRef GetContextRef() {
			static JsContextRef context;
			static std::once_flag of;
			std::call_once(of, [=]() {
				JsGetCurrentContext(&context);
			});
			return context;
		}

		std::wstring GetJSObjectPropertyAsString(const JsValueRef js_object_ref, const std::string& property_name) {
			std::wstring output = L"";

			JsPropertyIdRef propertyId = JSObject::GetJsPropertyIdRef(property_name);

			bool hasProperty = false;
			JsHasProperty(js_object_ref, propertyId, &hasProperty);

			if (hasProperty) {
				JsValueRef js_value_ref;
				JsGetProperty(js_object_ref, propertyId, &js_value_ref);
				if (js_value_ref) {
					JsValueType js_value_type;
					JsGetValueType(js_value_ref, &js_value_type);
					if (js_value_type == JsValueType::JsString) {
						LPCWSTR value = nullptr;
						std::size_t stringLength = 0;
						JsStringToPointer(js_value_ref, &value, &stringLength);
						if (value && stringLength > 0) {
							output += value;
							output += L"\r\n";
						}
					}
				}
			}
			return output;
		}

		void CheckAndThrowChakraRuntimeError(const JsErrorCode& errorCode) {
			std::wstring output = L"";

			switch (errorCode) {
			case JsNoError: return;
			case JsErrorCategoryUsage: output += L"Category of errors that relates to incorrect usage of the API itself."; break;
			case JsErrorInvalidArgument: output += L"An argument to a hosting API was invalid."; break;
			case JsErrorNullArgument: output += L"An argument to a hosting API was null in a context where null is not allowed."; break;
			case JsErrorNoCurrentContext: output += L"The hosting API requires that a context be current, but there is no current context."; break;
			case JsErrorInExceptionState: output += L"The engine is in an exception state and no APIs can be called until the exception is cleared."; break;
			case JsErrorNotImplemented: output += L"A hosting API is not yet implemented."; break;
			case JsErrorWrongThread: output += L"A hosting API was called on the wrong thread."; break;
			case JsErrorRuntimeInUse: output += L"A runtime that is still in use cannot be disposed."; break;
			case JsErrorBadSerializedScript: output += L"A bad serialized script was used, or the serialized script was serialized by a different version of the Chakra engine."; break;
			case JsErrorInDisabledState: output += L"The runtime is in a disabled state."; break;
			case JsErrorCannotDisableExecution: output += L"Runtime does not support reliable script interruption."; break;
			case JsErrorHeapEnumInProgress: output += L"A heap enumeration is currently underway in the script context."; break;
			case JsErrorArgumentNotObject: output += L"A hosting API that operates on object values was called with a non-object value."; break;
			case JsErrorInProfileCallback: output += L"A script context is in the middle of a profile callback."; break;
			case JsErrorInThreadServiceCallback: output += L"A thread service callback is currently underway."; break;
			case JsErrorCannotSerializeDebugScript: output += L"Scripts cannot be serialized in debug contexts."; break;
			case JsErrorAlreadyDebuggingContext: output += L"The context cannot be put into a debug state because it is already in a debug state."; break;
			case JsErrorAlreadyProfilingContext: output += L"The context cannot start profiling because it is already profiling."; break;
			case JsErrorIdleNotEnabled: output += L"Idle notification given when the host did not enable idle processing."; break;
			case JsCannotSetProjectionEnqueueCallback: output += L"The context did not accept the enqueue callback."; break;
			case JsErrorCannotStartProjection: output += L"Failed to start projection."; break;
			case JsErrorInObjectBeforeCollectCallback: output += L"The operation is not supported in an object before collect callback."; break;
			case JsErrorObjectNotInspectable: output += L"Object cannot be unwrapped to IInspectable pointer."; break;
			case JsErrorPropertyNotSymbol: output += L"A hosting API that operates on symbol property ids but was called with a non-symbol property id. The error code is returned by JsGetSymbolFromPropertyId if the function is called with non-symbol property id."; break;
			case JsErrorPropertyNotString: output += L"A hosting API that operates on string property ids but was called with a non-string property id. The error code is returned by existing JsGetPropertyNamefromId if the function is called with non-string property id."; break;
			case JsErrorCategoryEngine: output += L"Category of errors that relates to errors occurring within the engine itself."; break;
			case JsErrorOutOfMemory: output += L"The Chakra engine has run out of memory."; break;
			case JsErrorCategoryScript: output += L"Category of errors that relates to errors in a script."; break;
			case JsErrorScriptException: output += L"A JavaScript exception occurred while running a script."; break;
			case JsErrorScriptCompile: output += L"JavaScript failed to compile."; break;
			case JsErrorScriptTerminated: output += L"A script was terminated due to a request to suspend a runtime."; break;
			case JsErrorScriptEvalDisabled: output += L"A script was terminated because it tried to use eval or function and eval was disabled."; break;
			case JsErrorCategoryFatal: output += L"Category of errors that are fatal and signify failure of the engine."; break;
			case JsErrorFatal: output += L"A fatal error in the engine has occurred."; break;
			case JsErrorWrongRuntime: output += L"A hosting API was called with object created on different javascript runtime."; break;
			default:
				break;
			}

			output += L"\r\n";

			bool hasException = false;
			JsHasException(&hasException);
			if (hasException) {
				JsValueRef js_exception_ref = nullptr;
				JsGetAndClearException(&js_exception_ref);
				if (js_exception_ref != nullptr) {
					output += GetJSObjectPropertyAsString(js_exception_ref, "message");
					output += GetJSObjectPropertyAsString(js_exception_ref, "stack");
				}
			}

#ifndef NDEBUG
			OutputDebugString(output.c_str());
#endif
			std::string str_message = "Unknown error";
			if (!output.empty()) {
				std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
				str_message = std::string(converter.to_bytes(output));
			}

			throw std::runtime_error(str_message);
		}

		void ThrowRuntimeError(const std::string& internal_component_name, const std::string& message) {
			throw std::runtime_error(message);
		}

		void ThrowRuntimeError(const std::string& message) {
			throw std::runtime_error(message);
		}

		std::vector<JsValueRef> to_arguments(const std::vector<JSValue>& js_value_vector, JsValueRef this_object) {
			std::vector<JsValueRef> js_value_ref_vector{ this_object };
			std::transform(js_value_vector.begin(),
				js_value_vector.end(),
				std::back_inserter(js_value_ref_vector),
				[](const JSValue& js_value) { return static_cast<JsValueRef>(js_value); });
			return js_value_ref_vector;
		}

		std::vector<JSValue> to_arguments(JsValueRef *arguments, unsigned short argumentCount) {
			std::vector<JSValue> js_value_vector;
			for (unsigned short i = 1; i < argumentCount; i++) {
				js_value_vector.push_back(JSValue(arguments[i]));
			}
			return js_value_vector;
		}

	}
} // namespace HAL { namespace detail {
