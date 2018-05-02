// Copyright (C) 2013 Sony Computer Entertainment Inc.
// All Rights Reserved.

#ifndef JSMemoryStatsSCE_h
#define JSMemoryStatsSCE_h

#include <JavaScriptCore/JSContextRef.h>

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
//
// Retrieve memory stats about a JS context
//

struct JSMemoryStatsSCE {
    size_t size;
    size_t capacity;
    size_t objectCount;
};

JS_EXPORT void JSMemoryStatsQuerySCE(JSContextRef, JSMemoryStatsSCE&);

struct JSMemoryActivitySettingsSCE {
    int gcActivityFrequency; // how often to print GC activity in seconds. 0 to disable
    int objectCountActivityFrequency; // how often to print object count in seconds. 0 to disable
};

JS_EXPORT void JSMemoryActivitySettingsConfigSCE(JSMemoryActivitySettingsSCE) __attribute__((deprecated));

////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif /* JSMemoryStatsSCE_h */
