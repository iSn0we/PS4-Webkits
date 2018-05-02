/*
 * Copyright (C) 2009 Apple Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef JSContextRefPrivate_h
#define JSContextRefPrivate_h

#include <JavaScriptCore/JSObjectRef.h>
#include <JavaScriptCore/JSValueRef.h>
#include <JavaScriptCore/WebKitAvailability.h>
#include <JavaScriptCore/inspector/InspectorFrontendChannel.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*!
@function
@abstract Gets a Backtrace for the existing context
@param ctx The JSContext whose backtrace you want to get
@result A string containing the backtrace
*/
JS_EXPORT JSStringRef JSContextCreateBacktrace(JSContextRef ctx, unsigned maxStackSize) CF_AVAILABLE(10_6, 7_0);
    

/*! 
@typedef JSShouldTerminateCallback
@abstract The callback invoked when script execution has exceeded the allowed
 time limit previously specified via JSContextGroupSetExecutionTimeLimit.
@param ctx The execution context to use.
@param context User specified context data previously passed to
 JSContextGroupSetExecutionTimeLimit.
@discussion If you named your function Callback, you would declare it like this:

 bool Callback(JSContextRef ctx, void* context);

 If you return true, the timed out script will terminate.
 If you return false, the script will run for another period of the allowed
 time limit specified via JSContextGroupSetExecutionTimeLimit.

 Within this callback function, you may call JSContextGroupSetExecutionTimeLimit
 to set a new time limit, or JSContextGroupClearExecutionTimeLimit to cancel the
 timeout.
*/
typedef bool
(*JSShouldTerminateCallback) (JSContextRef ctx, void* context);

/*!
@function
@abstract Sets the script execution time limit.
@param group The JavaScript context group that this time limit applies to.
@param limit The time limit of allowed script execution time in seconds.
@param callback The callback function that will be invoked when the time limit
 has been reached. This will give you a chance to decide if you want to
 terminate the script or not. If you pass a NULL callback, the script will be
 terminated unconditionally when the time limit has been reached.
@param context User data that you can provide to be passed back to you
 in your callback.

 In order to guarantee that the execution time limit will take effect, you will
 need to call JSContextGroupSetExecutionTimeLimit before you start executing
 any scripts.
*/
JS_EXPORT void JSContextGroupSetExecutionTimeLimit(JSContextGroupRef, double limit, JSShouldTerminateCallback, void* context) CF_AVAILABLE(10_6, 7_0);

/*!
@function
@abstract Clears the script execution time limit.
@param group The JavaScript context group that the time limit is cleared on.
*/
JS_EXPORT void JSContextGroupClearExecutionTimeLimit(JSContextGroupRef) CF_AVAILABLE(10_6, 7_0);

/*!
@function
@abstract Drops all locks from JSContextRef and yields to other threads.
  Intended to be used as a JSShouldTerminateCallback (yields instead of terminates).
@param ctx The execution context to use.
@param unused
@discussion Drops all locks the context is currently holding and yields the thread
  to allow other threads to operate in the VM.
*/
JS_EXPORT bool JSContextThreadYield(JSContextRef ctx, void* unused);

/*!
@function
@abstract Connects an InspectorFrontendChannel to the context's inspector controller.
@param ctx The JSGlobalContext that you want to inspect.
@param frontendChannel The Inspector::FrontendChannel you want to connect.
@param isAutomaticInspection
*/
JS_EXPORT void JSGlobalObjectInspectorControllerConnectFrontend(JSContextRef ctx, Inspector::FrontendChannel*, bool isAutomaticInspection);

/*!
@function Disconnects the InspectorFrontendChannel from the context's inspector controller.
@param ctx The JSGlobalContext you are disconnecting from.
*/
JS_EXPORT void JSGlobalObjectInspectorControllerDisconnectFrontend(JSContextRef ctx);

/*!
@function
@abstract Dispatches a message from the frontend to the context's inspector controller.
@param ctx The JSGlobalContext that you want to send a message to.
@param data The message you want to dispatch.
@param data_len The length of the message.
*/
JS_EXPORT void JSGlobalObjectInspectorControllerDispatchMessageFromFrontend(JSContextRef ctx, const char* data, int length);

/*!
@function
@abstract Gets a whether or not remote inspection is enabled on the context.
@param ctx The JSGlobalContext whose setting you want to get.
@result The value of the setting, true if remote inspection is enabled, otherwise false.
@discussion Remote inspection is true by default.
*/
JS_EXPORT bool JSGlobalContextGetRemoteInspectionEnabled(JSGlobalContextRef ctx) CF_AVAILABLE(10_10, 8_0);

/*!
@function
@abstract Sets the remote inspection setting for a context.
@param ctx The JSGlobalContext that you want to change.
@param enabled The new remote inspection enabled setting for the context.
*/
JS_EXPORT void JSGlobalContextSetRemoteInspectionEnabled(JSGlobalContextRef ctx, bool enabled) CF_AVAILABLE(10_10, 8_0);    

/*!
@function
@abstract Gets the include native call stack when reporting exceptions setting for a context.
@param ctx The JSGlobalContext whose setting you want to get.
@result The value of the setting, true if remote inspection is enabled, otherwise false.
@discussion This setting is true by default.
*/
JS_EXPORT bool JSGlobalContextGetIncludesNativeCallStackWhenReportingExceptions(JSGlobalContextRef ctx) CF_AVAILABLE(10_10, 8_0);

/*!
@function
@abstract Sets the include native call stack when reporting exceptions setting for a context.
@param ctx The JSGlobalContext that you want to change.
@param includeNativeCallStack The new value of the setting for the context.
*/
JS_EXPORT void JSGlobalContextSetIncludesNativeCallStackWhenReportingExceptions(JSGlobalContextRef ctx, bool includesNativeCallStack) CF_AVAILABLE(10_10, 8_0);

#ifdef __cplusplus
}
#endif

#endif /* JSContextRefPrivate_h */
