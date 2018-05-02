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

#include "config.h"
#include "WKBundleManx.h"

#include "InjectedBundle.h"
#include "WKBundleAPICast.h"
#include <WebCore/MemoryCache.h>

#if USE(NTF)
#include <ntf/DiskCacheUtility.h>
#endif

using namespace WebKit;

void WKBundleGetFastMallocStatistics(WKBundleRef bundleRef, size_t* usedBytes, size_t* reservedBytes)
{
    toImpl(bundleRef)->getFastMallocStatistics(usedBytes, reservedBytes);
}

void WKBundleGetJavaScriptHeapStatistics(WKBundleRef bundleRef, size_t* usedBytes, size_t* reservedBytes)
{
    toImpl(bundleRef)->javaScriptHeapStatistics(usedBytes, reservedBytes);
}

void WKBundleGetDiskCacheStatistics(WKBundleRef bundleRef, WKBundleDiskCacheStatistic* stat)
{
#if USE(NTF)
    NTF::Cache::DiskCacheStatistics totalStat = { };
    NTF::Cache::DiskCacheUtility::getStatisticsTotal(&totalStat);
    stat->count = totalStat.entries;
    stat->size = totalStat.size;

    NTF::Cache::DiskCacheUtility::getStatisticsTotalDeleted(&totalStat);
    stat->deletedCount = totalStat.entries;
    stat->deletedSize = totalStat.size;

    NTF::Cache::DiskCacheUtility::getStatisticsTotalDeletedInitialIncompleted(&totalStat);
    stat->deletedInitialIncompletedCount = totalStat.entries;
    stat->deletedInitialIncompletedSize = totalStat.size;

    NTF::Cache::DiskCacheUtility::getStatisticsTotalDeletedHasOld(&totalStat);
    stat->deletedHasOldCount = totalStat.entries;
    stat->deletedHasOldSize = totalStat.size;

    NTF::Cache::DiskCacheUtility::getStatisticsTotalDeletedInvalid(&totalStat);
    stat->deletedInvalidCount = totalStat.entries;
    stat->deletedInvalidSize = totalStat.size;

    NTF::Cache::DiskCacheUtility::getStatisticsTotalDeletedByPattern(&totalStat);
    stat->deletedByPatternCount = totalStat.entries;
    stat->deletedByPatternSize = totalStat.size;

    NTF::Cache::DiskCacheUtility::getStatisticsTotalDeletedAll(&totalStat);
    stat->deletedAllCount = totalStat.entries;
    stat->deletedAllSize = totalStat.size;

    NTF::Cache::DiskCacheUtility::getStatisticsTotalDeletedExpired(&totalStat);
    stat->deletedExpiredCount = totalStat.entries;
    stat->deletedExpiredSize = totalStat.size;

    NTF::Cache::DiskCacheUtility::getStatisticsTotalDeletedOld(&totalStat);
    stat->deletedOldCount = totalStat.entries;
    stat->deletedOldSize = totalStat.size;
#endif
}

void WKBundleClearAllDiskCaches(WKBundleRef bundleRef)
{
    toImpl(bundleRef)->clearAllDiskCaches(String(), String());
}

void WKBundleClearDiskCachesByPattern(WKBundleRef bundleRef, WKStringRef pattern, WKStringRef excludePattern)
{
    toImpl(bundleRef)->clearAllDiskCaches(toWTFString(pattern), toWTFString(excludePattern));
}

void WKBundleShutdownProcess(WKBundleRef bundleRef)
{
    toImpl(bundleRef)->shutdownProcess();
}

void WKBundleSetGarbageCollectClient(WKBundleRef bundleRef, WKBundleGarbageCollectClientBase* wkClient)
{
    toImpl(bundleRef)->initializeGCClient(wkClient);
}

static inline WKBundleMemoryCacheTypeStatistic convert(const WebCore::MemoryCache::TypeStatistic& from)
{
    WKBundleMemoryCacheTypeStatistic to;
    to.count = from.count;
    to.size = from.size;
    to.liveSize = from.liveSize;
    to.decodedSize = from.decodedSize;

    // NOTE : Remove PurgeableBuffer since it is not very useful any more.
    // https://trac.webkit.org/changeset/172736
    to.purgeableSize = 0;
    to.purgedSize = 0;
    return to;
}

void WKBundleGetMemoryCacheStatistics(WKBundleRef, WKBundleMemoryCacheStatistics* out)
{
    if (!out)
        return;

    WebCore::MemoryCache::Statistics stat = WebCore::MemoryCache::singleton().getStatistics();
    out->images = convert(stat.images);
    out->cssStyleSheets = convert(stat.cssStyleSheets);
    out->scripts = convert(stat.scripts);
    out->xslStyleSheets = convert(stat.xslStyleSheets);
    out->fonts = convert(stat.fonts);
}
