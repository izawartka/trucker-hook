#include "common.h"
#include "trucker.h"
#include "config.h"
#include "config-values.h"
#include "hooks-helper.h"

// stretched - (0 = left side of the screen, 1024 = right side of the screen)
// left aligned (original) - (0 = left side of the screen, 1024 != right side of the screen)
// right aligned - (0 != left side of the screen, 1024 = right side of the screen)
#define SCALED_X(x) (screenUIWidthScale * (x)) // stretched scaling
#define SCALED_Y(y) (screenUIScale * (y)) // left aligned (original) scaling
#define SCALED_V(x) (screenUIScale * x + rightSideShift) // right aligned scaling
#define SCALED_C(x) (screenUIScale * x + halfRightSideShift) // centered scaling
#define SCALED_RECTS(x1, y1, x2, y2) SCALED_X(x1), SCALED_Y(y1), SCALED_X(x2), SCALED_Y(y2) // stretched rect scaling
#define SCALED_RECTO(x1, y1, x2, y2) SCALED_Y(x1), SCALED_Y(y1), SCALED_Y(x2), SCALED_Y(y2) // left aligned (original) rect scaling
#define SCALED_RECTV(x1, y1, x2, y2) SCALED_V(x1), SCALED_Y(y1), SCALED_V(x2), SCALED_Y(y2) // right aligned rect scaling
#define SCALED_RECTC(x1, y1, x2, y2) SCALED_C(x1), SCALED_Y(y1), SCALED_C(x2), SCALED_Y(y2) // centered rect scaling

static DWORD pScreenWidth = 0x006d196c;
static DWORD pScreenHeight = 0x006d1968;
static DWORD pScreenBitdepth = 0x006d1970;
static DWORD pScreenUIScale = 0x006d1974;

static DWORD pScreenOptionsListJump = 0x00401e9C;
static DWORD pScreenOptionsListReturn = 0x00401ee6;

static DWORD pFixUIPlacementJump = 0x0040c888;
static DWORD pFixUIPlacementReturn = 0x0040cd74;

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

//void __thiscall drawTexture(UIElement *this,char *path,float x1,float y1,float x2,float y2,int flags)
typedef void(__thiscall* DrawTexture)(UIElement* param_1_00, char* path, float x1, float y1, float x2, float y2, int flags);
static DrawTexture fnDrawTexture = (DrawTexture)0x0044c3d0;

//int __thiscall drawPanel(UIPanel *this,char *path,int width,int height)
typedef int(__thiscall* DrawPanel)(UIPanel* param_1_00, char* path, int width, int height);
static DrawPanel fnDrawPanel = (DrawPanel)0x004b0ff0;

const char* customScreenOptionsText = "Custom [Hook]";

int screenWidth;
int screenHeight;
int screenBitdepth;
float screenUIScale;
float screenUIWidthScale;
float rightSideShift;
float halfRightSideShift;

UIStruct* pUIStruct = (UIStruct*)0x006d1af8;

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

static void FixDamageVisualizerPlacement(void) {
	char buf[256];

	for (int i = 1; i <= 6; i++) {
		sprintf(buf, "hud_strefa_%i.tga", i);
		fnDrawTexture(&pUIStruct->hudStrefa[i - 1], buf, SCALED_RECTV(868, 368, 1060, 568), -0x7f7f3f80);
		pUIStruct->strefaInt[i - 1] = 0;
	}

	for (int i = 1; i <= 4; i++) {
		sprintf(buf, "hud_strefa_kolo_%i.tga", i);
		fnDrawTexture(&pUIStruct->hudStrefaKolo[i - 1], buf, SCALED_RECTV(868, 368, 1060, 568), -0x7f7f3f80);
		pUIStruct->strefaKoloInts[i - 1] = 0;
	}
}

static __declspec(naked) void FixUIPlacementHook(void) {
	fnDrawTexture(&pUIStruct->hudIcoCzas, "hud_ico_czas.tga", SCALED_RECTV(820, 2, 1000, 92), -1);
	fnDrawTexture(&pUIStruct->hudIcoMiejsce, "hud_ico_miejsce.tga", SCALED_RECTV(820, 56, 1000, 146), -1);
	fnDrawTexture(&pUIStruct->hudIcoLap, "hud_ico_lap.tga", SCALED_RECTV(820, 130, 1000, 220), -1);
	fnDrawTexture(&pUIStruct->hudCarJa, "hud_car_ja.tga", SCALED_RECTO(98, 654, 114, 670), -1);
	fnDrawTexture(&pUIStruct->hudCarWrog, "hud_car_wrog.tga", SCALED_RECTO(98, 654, 114, 670), -1);
	fnDrawTexture(&pUIStruct->hudCarPolicja, "hud_car_policja.tga", SCALED_RECTO(98, 654, 114, 670), -1);
	fnDrawTexture(&pUIStruct->hudIcoN2Oniegotowy, "hud_ico_N2O_niegotowy.tga", SCALED_RECTV(720, 2, 784, 66), -1);
	fnDrawTexture(&pUIStruct->hudIcoN2Ogotowy, "hud_ico_N2O_gotowy.tga", SCALED_RECTV(720, 2, 784, 66), -1);
	fnDrawTexture(&pUIStruct->hudIcoRadarCzerwony, "hud_ico_radar_czerwony.tga", SCALED_RECTV(640, 2, 704, 66), -1);
	fnDrawTexture(&pUIStruct->hudIcoRadarZielony, "hud_ico_radar_zielony.tga", SCALED_RECTV(640, 2, 704, 66), -1);
	fnDrawTexture(&pUIStruct->hudZakazR, "hud_zakaz_r.tga", SCALED_RECTV(560, 2, 624, 66), -1);
	fnDrawTexture(&pUIStruct->hudPaliwo, "hud_paliwo.tga", SCALED_RECTV(896, 596, 1024, 724), -1);
	fnDrawTexture(&pUIStruct->hudPaliwoWskazowka, "hud_paliwo_wskazowka.tga", 0.0f, 0.0f, 256.0f, 256.0f, -1);
	fnDrawTexture(&pUIStruct->hudPredkosciomierz, "hud_predkosciomierz.tga", SCALED_RECTV(738, 532, 994, 788), -1);
	fnDrawTexture(&pUIStruct->hudWskazowkaPredkosc, "hud_wskazowka_predkosc.tga", 0.0f, 0.0f, 256.0f, 256.0f, -1);
	fnDrawTexture(&pUIStruct->hudWskazowkaObroty, "hud_wskazowka_obroty.tga", 1000.0f, 0.0f, 256.0f, 256.0f, -1);

	pUIStruct->unkInt = -2;

	fnDrawPanel(&pUIStruct->hudMenuPolicja, "misc\\HUD\\hud_menu_policja.bmp", screenWidth, screenHeight);

	FixDamageVisualizerPlacement();

	fnDrawTexture(&pUIStruct->hudIcoKierunek, "hud_ico_kierunek.tga", SCALED_RECTC(384, 300, 640, 428), -1);

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

void createHooks() {
	// read values from config file
	Config::ReadOrDefaultFromConfig(CONF_SCREEN_SECTION, CONF_WIDTH_KEY, screenWidth, DEFAULT_SCREEN_WIDTH);
	Config::ReadOrDefaultFromConfig(CONF_SCREEN_SECTION, CONF_HEIGHT_KEY, screenHeight, DEFAULT_SCREEN_HEIGHT);
	Config::ReadOrDefaultFromConfig(CONF_SCREEN_SECTION, CONF_BITDEPTH_KEY, screenBitdepth, DEFAULT_SCREEN_BITDEPTH);
	screenUIScale = screenHeight / 3.0f * 4.0f / 1024.0f;
	screenUIWidthScale = screenWidth / 1024.0f;
	rightSideShift = (screenWidth - screenHeight / 3.0f * 4.0f);
	halfRightSideShift = rightSideShift / 2.0f;

	// disable setting screen options (width, height, bitdepth) & ui scale by the game
	HooksHelper::ReplaceWithNoOp(0x004021f3, 6);  // sets screen height from the start menu
	HooksHelper::ReplaceWithNoOp(0x00402207, 12); // sets screen width & bitdepth from the start menu
	HooksHelper::ReplaceWithNoOp(0x00402227, 6);  // calculates ui scale from the start menu

	HooksHelper::ReplaceWithNoOp(0x0043cc05, 5);  // loads screen width from config
	HooksHelper::ReplaceWithNoOp(0x0043cc25, 5);  // loads screen height from config
	HooksHelper::ReplaceWithNoOp(0x0043cc52, 5);  // loads screen bitdepth from config
	HooksHelper::ReplaceWithNoOp(0x0043da62, 6);  // calculates ui scale from config

	HooksHelper::PrimitiveHookFunction(pScreenOptionsListJump, (DWORD)SkipScreenOptionsListHook, 5);

	// fix ui elements placement
	HooksHelper::PrimitiveHookFunction(pFixUIPlacementJump, (DWORD)FixUIPlacementHook, 5);
	HooksHelper::HookMultipleCallBasedFunctions(pFixGaugesPlacementCalls, sizeof(pFixGaugesPlacementCalls) / sizeof(DWORD), (DWORD)FixGaugesPlacementHook);
	HooksHelper::HookMultipleCallBasedFunctions(pFixRaceTextPlacementCalls, sizeof(pFixRaceTextPlacementCalls) / sizeof(DWORD), (DWORD)FixRaceTextPlacementHook);

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