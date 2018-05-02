/*
 * Copyright (C) 2016 Sony Interactive Entertainment Inc.
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
 * THIS SOFTWARE IS PROVIDED BY SONY COMPUTER ENTERTAINMENT INC. ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL SONY COMPUTER ENTERTAINMENT INC.
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WKBundleManx_h
#define WKBundleManx_h

#include <WebKit/WKBase.h>

#ifdef __cplusplus
extern "C" {
#endif

WK_EXPORT void WKBundleGetFastMallocStatistics(WKBundleRef, size_t* usedBytes, size_t* reservedBytes);
WK_EXPORT void WKBundleGetJavaScriptHeapStatistics(WKBundleRef, size_t* usedBytes, size_t* reservedBytes);

struct WKBundleDiskCacheStatistic {
    long count;
    long size;
    long deletedCount;
    long deletedSize;
    long deletedInitialIncompletedCount;
    long deletedInitialIncompletedSize;
    long deletedHasOldCount;
    long deletedHasOldSize;
    long deletedInvalidCount;
    long deletedInvalidSize;
    long deletedByPatternCount;
    long deletedByPatternSize;
    long deletedAllCount;
    long deletedAllSize;
    long deletedExpiredCount;
    long deletedExpiredSize;
    long deletedOldCount;
    long deletedOldSize;
};
typedef struct WKBundleDiskCacheStatistic WKBundleDiskCacheStatistic;

WK_EXPORT void WKBundleGetDiskCacheStatistics(WKBundleRef, WKBundleDiskCacheStatistic*);
WK_EXPORT void WKBundleClearAllDiskCaches(WKBundleRef);
WK_EXPORT void WKBundleClearDiskCachesByPattern(WKBundleRef, WKStringRef, WKStringRef);

WK_EXPORT void WKBundleShutdownProcess(WKBundleRef);

typedef void (*WKBundleWillGarbageCollectCallback)(WKBundleRef, double timestamp, const void* clientInfo);

typedef struct WKBundleGarbageCollectClientBase {
    int                                     version;
    const void*                             clientInfo;
} WKBundleGarbageCollectClientBase;

typedef struct WKBundleGarbageCollectClientV0 {
    WKBundleGarbageCollectClientBase        base;

    WKBundleWillGarbageCollectCallback      willGarbageCollect;
} WKBundleGarbageCollectClientV0;

enum { kWKBundleGarbageCollectClientCurrentVersion = 0 };

WK_EXPORT void WKBundleSetGarbageCollectClient(WKBundleRef, WKBundleGarbageCollectClientBase*);

struct WKBundleMemoryCacheTypeStatistic {
    int count;
    int size;
    int liveSize;
    int decodedSize;
    int purgeableSize;
    int purgedSize;
};
typedef struct WKBundleMemoryCacheTypeStatistic WKBundleMemoryCacheTypeStatistic;

struct WKBundleMemoryCacheStatistics {
    WKBundleMemoryCacheTypeStatistic images;
    WKBundleMemoryCacheTypeStatistic cssStyleSheets;
    WKBundleMemoryCacheTypeStatistic scripts;
    WKBundleMemoryCacheTypeStatistic xslStyleSheets;
    WKBundleMemoryCacheTypeStatistic fonts;
};
typedef struct WKBundleMemoryCacheStatistics WKBundleMemoryCacheStatistics;

WK_EXPORT void WKBundleGetMemoryCacheStatistics(WKBundleRef, WKBundleMemoryCacheStatistics*);

#ifdef __cplusplus
}
#endif

#endif // WKBundleManx_h
