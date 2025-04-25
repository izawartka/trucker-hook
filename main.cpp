#include "common.h"
#include "config.h"

#define DEFAULT_SCREEN_WIDTH 1280
#define DEFAULT_SCREEN_HEIGHT 720
#define DEFAULT_SCREEN_BITDEPTH 32

// stretched - (0 = left side of the screen, 1000 = right side of the screen)
// left aligned (original) - (0 = left side of the screen, 1000 != right side of the screen)
// right aligned - (0 != left side of the screen, 1000 = right side of the screen)
#define SCALED_X(x) (screenUIWidthScale * (x)) // stretched scaling
#define SCALED_Y(y) (screenUIScale * (y)) // left aligned (original) scaling
#define SCALED_V(x) (SCALED_X(1000) - SCALED_Y(1000 - x)) // right aligned scaling
#define SCALED_RECTS(x1, y1, x2, y2) SCALED_X(x1), SCALED_Y(y1), SCALED_X(x2), SCALED_Y(y2) // stretched rect scaling
#define SCALED_RECTO(x1, y1, x2, y2) SCALED_Y(x1), SCALED_Y(y1), SCALED_Y(x2), SCALED_Y(y2) // left aligned (original) rect scaling
#define SCALED_RECTV(x1, y1, x2, y2) SCALED_V(x1), SCALED_Y(y1), SCALED_V(x2), SCALED_Y(y2) // right aligned rect scaling

static DWORD pScreenWidth = 0x006d196c;
static DWORD pScreenHeight = 0x006d1968;
static DWORD pScreenBitdepth = 0x006d1970;
static DWORD pScreenUIScale = 0x006d1974;

static DWORD pScreenOptionsListJump = 0x00401e9C;
static DWORD pScreenOptionsListReturn = 0x00401ee6;

static DWORD pFixUIPlacementJump = 0x0040c888;
static DWORD pFixUISomeStruct = 0x006d1af8;
static DWORD pFixUIPlacementReturn = 0x0040cc0f;

static DWORD pFixGaugesPlacementSomeFn = 0x0044b690;
static DWORD pFixGaugesPlacementCalls[] = {
	0x0040aa65, // RPM
	0x0040a823, // Speed
	0x0040ba89, // Fuel
};

static DWORD pFixRaceTextPlacementSomeFn = 0x0044e070;
static DWORD pFixRaceTextPlacementCalls[] = {
	0x0040aba7,
	0x0040abe0,
	0x004a6eaf,
	0x004a6eea,
	0x004a6f9c,
	0x004a6fd7,
	0x004a70cb,
	0x004a7106,
	0x004a71b6,
	0x004a71f1,
	0x004a7266,
	0x004a72a1
};

//void __thiscall drawTexture(int param_1_00,char *path,float x1,float y1,float x2,float y2,int flags)
typedef void(__thiscall* DrawTexture)(int param_1_00, char* path, float x1, float y1, float x2, float y2, int flags);
static DrawTexture fnDrawTexture = (DrawTexture)0x0044c3d0;

const char* customScreenOptionsText = "Custom [Hook]";

const char* SECTION = "Screen";
const char* WIDTH_KEY = "Width";
const char* HEIGHT_KEY = "Height";
const char* BITDEPTH_KEY = "BitDepth";

int screenWidth = DEFAULT_SCREEN_WIDTH;
int screenHeight = DEFAULT_SCREEN_HEIGHT;
int screenBitdepth = DEFAULT_SCREEN_BITDEPTH;
float screenUIScale;
float screenUIWidthScale;
float rightSideShift;

static __declspec(naked) void SkipScreenOptionsListHook(void) {
	// Screen options' hWnd is stored in ECX

	__asm {
		push ECX

		// Add "Custom" to the screen options list
		push dword ptr ds : [customScreenOptionsText]
		push 0
		push 0x14A
		push ECX
		call SendMessageA

		pop ECX

		// Select the "Custom" option
		push 0
		push 0
		push 0x14E
		push ECX
		call SendMessageA

		// Go back to the program
		JMP pScreenOptionsListReturn
	}
}

static __declspec(naked) void FixUIPlacementHook(void) {
	fnDrawTexture(pFixUISomeStruct + 0x640, "hud_ico_czas.tga", SCALED_RECTV(820, 2, 1000, 92), -1);
	fnDrawTexture(pFixUISomeStruct + 0x6ec, "hud_ico_miejsce.tga", SCALED_RECTV(820, 56, 1000, 146), -1);
	fnDrawTexture(pFixUISomeStruct + 0x798, "hud_ico_lap.tga", SCALED_RECTV(820, 130, 1000, 220), -1);
	fnDrawTexture(pFixUISomeStruct + 0x000, "hud_car_ja.tga", SCALED_RECTO(98, 654, 114, 670), -1);
	fnDrawTexture(pFixUISomeStruct + 0x0ac, "hud_car_wrog.tga", SCALED_RECTO(98, 654, 114, 670), -1);
	fnDrawTexture(pFixUISomeStruct + 0x158, "hud_car_policja.tga", SCALED_RECTO(98, 654, 114, 670), -1);
	fnDrawTexture(pFixUISomeStruct + 0x43c, "hud_ico_N2O_niegotowy.tga", SCALED_RECTV(720, 2, 784, 66), -1);
	fnDrawTexture(pFixUISomeStruct + 0x4e8, "hud_ico_N2O_gotowy.tga", SCALED_RECTV(720, 2, 784, 66), -1);
	fnDrawTexture(pFixUISomeStruct + 0x2b8, "hud_ico_radar_czerwony.tga", SCALED_RECTV(640, 2, 704, 66), -1);
	fnDrawTexture(pFixUISomeStruct + 0x364, "hud_ico_radar_zielony.tga", SCALED_RECTV(640, 2, 704, 66), -1);
	fnDrawTexture(pFixUISomeStruct + 0x204, "hud_zakaz_r.tga", SCALED_RECTV(560, 2, 624, 66), -1);
	fnDrawTexture(pFixUISomeStruct + 0xbb0, "hud_paliwo.tga", SCALED_RECTV(896, 596, 1024, 724), -1);
	fnDrawTexture(pFixUISomeStruct + 0xc5c, "hud_paliwo_wskazowka.tga", 0.0f, 0.0f, 256.0f, 256.0f, -1);
	fnDrawTexture(pFixUISomeStruct + 0x9ac, "hud_predkosciomierz.tga", SCALED_RECTV(738, 532, 994, 788), -1);
	fnDrawTexture(pFixUISomeStruct + 0xb04, "hud_wskazowka_predkosc.tga", 0.0f, 0.0f, 256.0f, 256.0f, -1);
	fnDrawTexture(pFixUISomeStruct + 0xa58, "hud_wskazowka_obroty.tga", 1000.0f, 0.0f, 256.0f, 256.0f, -1);
	/// TODO: rest of the function

	__asm {
		JMP pFixUIPlacementReturn
	}
}

static __declspec(naked) void FixGaugesPlacementHook(void) {
	__asm {
		MOVSS xmm0, rightSideShift
		ADDSS xmm0, [esp + 4]
		MOVSS[esp + 4], xmm0

		MOVSS xmm0, rightSideShift
		ADDSS xmm0, [esp + 12]
		MOVSS [esp + 12], xmm0

		MOVSS xmm0, rightSideShift
		ADDSS xmm0, [esp + 20]
		MOVSS [esp + 20], xmm0

		MOVSS xmm0, rightSideShift
		ADDSS xmm0, [esp + 28]
		MOVSS[esp + 28], xmm0

		JMP pFixGaugesPlacementSomeFn
	}
}

static __declspec(naked) void FixRaceTextPlacementHook(void) {
	__asm {
		MOVSS xmm0, rightSideShift
		ADDSS xmm0, [esp + 8]
		MOVSS[esp + 8], xmm0

		JMP pFixRaceTextPlacementSomeFn
	}
}

BOOL PrimitiveHookFunction(const DWORD originalFn, DWORD hookFn, size_t copyBytes, bool callBased = false) {
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

BOOL HookMultipleCallBasedFunctions(const DWORD originalFns[], size_t fnCount, DWORD hookFn) {
	bool allSuccess = true;

	for (size_t i = 0; i < fnCount; i++) {
		if (!PrimitiveHookFunction(originalFns[i], hookFn, 5, true)) {
			allSuccess = false;
		}
	}
	return allSuccess;
}

BOOL HookFunction(const DWORD originalFn, DWORD hookFn, size_t copyBytes) {
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

// Replaces given number of bytes with NOPs (0x90) at the given address
BOOL ReplaceWithNoOp(const DWORD address, size_t bytes) {
	DWORD OldProtection = { 0 };
	BOOL success = VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, bytes, PAGE_EXECUTE_READWRITE, &OldProtection);
	if (!success) {
		DWORD error = GetLastError();
		return 0;
	}

	for (size_t i = 0; i < bytes; i++) {
		*(BYTE*)((LPBYTE)address + i) = 0x90;
	}
}

void createHooks() {
	// read values from config file
	Config::ReadOrDefaultFromConfig(SECTION, WIDTH_KEY, screenWidth, DEFAULT_SCREEN_WIDTH);
	Config::ReadOrDefaultFromConfig(SECTION, HEIGHT_KEY, screenHeight, DEFAULT_SCREEN_HEIGHT);
	Config::ReadOrDefaultFromConfig(SECTION, BITDEPTH_KEY, screenBitdepth, DEFAULT_SCREEN_BITDEPTH);
	screenUIScale = screenHeight / 3.0f * 4.0f * 0.0009765625f;
	screenUIWidthScale = screenWidth * 0.0009765625f;
	rightSideShift = (SCALED_X(994) - SCALED_Y(994));

	// disable setting screen options (width, height, bitdepth) & ui scale by the game
	ReplaceWithNoOp(0x004021f3, 6);	 // sets screen height from the start menu
	ReplaceWithNoOp(0x00402207, 12); // sets screen width & bitdepth from the start menu
	ReplaceWithNoOp(0x00402227, 6);  // calculates ui scale from the start menu

	ReplaceWithNoOp(0x0043cc05, 5);  // loads screen width from config
	ReplaceWithNoOp(0x0043cc25, 5);  // loads screen height from config
	ReplaceWithNoOp(0x0043cc52, 5);  // loads screen bitdepth from config
	ReplaceWithNoOp(0x0043da62, 6);  // calculates ui scale from config

	PrimitiveHookFunction(pScreenOptionsListJump, (DWORD)SkipScreenOptionsListHook, 5);

	// fix ui elements placement
	PrimitiveHookFunction(pFixUIPlacementJump, (DWORD)FixUIPlacementHook, 5);
	HookMultipleCallBasedFunctions(pFixGaugesPlacementCalls, sizeof(pFixGaugesPlacementCalls) / sizeof(DWORD), (DWORD)FixGaugesPlacementHook);
	HookMultipleCallBasedFunctions(pFixRaceTextPlacementCalls, sizeof(pFixRaceTextPlacementCalls) / sizeof(DWORD), (DWORD)FixRaceTextPlacementHook);

	// write new screen options to the game
	*(int*)pScreenWidth = screenWidth;
	*(int*)pScreenHeight = screenHeight;
	*(int*)pScreenBitdepth = screenBitdepth;
	*(float*)pScreenUIScale = screenUIScale;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)createHooks, NULL, 0, NULL);
    }
    return TRUE;
}

typedef void* (__stdcall* tDirect3DCreate8)(UINT);
tDirect3DCreate8 original_Direct3DCreate8 = nullptr;

extern "C" __declspec(dllexport) void* __stdcall Direct3DCreate8(UINT SDKVersion) {
	if (!original_Direct3DCreate8) {
		char sysPath[MAX_PATH];
		GetSystemDirectoryA(sysPath, MAX_PATH);
		strcat_s(sysPath, "\\d3d8.dll");

		HMODULE realD3D8 = LoadLibraryA(sysPath);
		original_Direct3DCreate8 = (tDirect3DCreate8)GetProcAddress(realD3D8, "Direct3DCreate8");
	}

	return original_Direct3DCreate8(SDKVersion);
}