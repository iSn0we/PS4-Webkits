#include "config.h"
#include "JSMemoryStatsSCE.h"

#include "APICast.h"

void JSMemoryStatsQuerySCE(
    JSContextRef context,
    JSMemoryStatsSCE& stats)
{
    JSC::ExecState* execState = ::toJS(context);
    JSC::Heap* heap = execState->heap();

    stats.size        = heap->size();
    stats.capacity    = heap->capacity();
    stats.objectCount = heap->objectCount();
}

void JSMemoryActivitySettingsConfigSCE(
    JSMemoryActivitySettingsSCE settings)
{
}
