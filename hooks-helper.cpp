#include "hooks-helper.h"

bool HooksHelper::PrimitiveHookFunction(const DWORD originalFn, DWORD hookFn, size_t copyBytes, bool callBased)
{
    DWORD OldProtection = { 0 };
    BOOL success = VirtualProtectEx(GetCurrentProcess(), (LPVOID)originalFn, copyBytes, PAGE_EXECUTE_READWRITE, &OldProtection);
    if (!success) {
        DWORD error = GetLastError();
        return 0;
    }

    *(BYTE*)((LPBYTE)originalFn) = callBased ? 0xE8 : 0xE9; // CALL / JMP FAR
    DWORD offset = (((DWORD)hookFn) - ((DWORD)originalFn + 5)); // Offset math
    *(DWORD*)((LPBYTE)originalFn + 1) = offset;

    for (size_t i = 5; i < copyBytes; i++) {
        *(BYTE*)((LPBYTE)originalFn + i) = 0x90;
    }

    return 1;
}

bool HooksHelper::HookFunction(const DWORD originalFn, DWORD hookFn, size_t copyBytes)
{
    DWORD OldProtection = { 0 };
    BOOL success = VirtualProtectEx(GetCurrentProcess(), (LPVOID)hookFn, copyBytes, PAGE_EXECUTE_READWRITE, &OldProtection);
    if (!success) {
        DWORD error = GetLastError();
        return 0;
    }

    for (size_t i = 0; i < copyBytes; i++) {
        *(BYTE*)((LPBYTE)hookFn + i + 1) = *(BYTE*)((LPBYTE)originalFn + i);
    }

    return PrimitiveHookFunction(originalFn, hookFn, copyBytes);
}

bool HooksHelper::HookMultipleCallBasedFunctions(const DWORD originalFns[], size_t fnCount, DWORD hookFn)
{
    bool allSuccess = true;

    for (size_t i = 0; i < fnCount; i++) {
        if (!PrimitiveHookFunction(originalFns[i], hookFn, 5, true)) {
            allSuccess = false;
        }
    }
    return allSuccess;
}

bool HooksHelper::ReplaceWithNoOp(const DWORD address, size_t bytes)
{
    DWORD OldProtection = { 0 };
    BOOL success = VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, bytes, PAGE_EXECUTE_READWRITE, &OldProtection);
    if (!success) {
        DWORD error = GetLastError();
        return 0;
    }

    for (size_t i = 0; i < bytes; i++) {
        *(BYTE*)((LPBYTE)address + i) = 0x90;
    }

    return 1;
}
