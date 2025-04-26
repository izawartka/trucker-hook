#pragma once
#include "common.h"

class HooksHelper {
public:
    static bool PrimitiveHookFunction(const DWORD originalFn, DWORD hookFn, size_t copyBytes, bool callBased = false);
    static bool HookFunction(const DWORD originalFn, DWORD hookFn, size_t copyBytes);
    static bool HookMultipleCallBasedFunctions(const DWORD originalFns[], size_t fnCount, DWORD hookFn);
    static bool ReplaceWithNoOp(const DWORD address, size_t bytes);
};
