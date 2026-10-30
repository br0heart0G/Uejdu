// gtasan.cpp — Final Fixed Version
// Trick: d3d9.h exports رو با #define مخفی میکنیم قبل از include
// بعد undef میکنیم و با اسم واقعی export میکنیم — هیچ conflict ای نداره

#define WIN32_LEAN_AND_MEAN

// ── همه d3d9 export functions رو mask کن قبل از include ──────────────────
#define Direct3DCreate9                _H_Create9
#define Direct3DCreate9Ex              _H_Create9Ex
#define Direct3DShaderValidatorCreate9 _H_ShaderVal
#define D3DPERF_BeginEvent             _H_PerfBegin
#define D3DPERF_EndEvent               _H_PerfEnd
#define D3DPERF_GetStatus              _H_PerfStatus
#define D3DPERF_QueryRepeatFrame       _H_PerfQuery
#define D3DPERF_SetMarker              _H_PerfMarker
#define D3DPERF_SetOptions             _H_PerfOpts
#define D3DPERF_SetRegion              _H_PerfRegion
#define DebugSetLevel                  _H_DbgLevel
#define DebugSetMute                   _H_DbgMute

#include <windows.h>
#include <mmsystem.h>
#include <d3d9.h>

// ── حالا undef کن — ما export ها رو خودمون declare میکنیم ────────────────
#undef Direct3DCreate9
#undef Direct3DCreate9Ex
#undef Direct3DShaderValidatorCreate9
#undef D3DPERF_BeginEvent
#undef D3DPERF_EndEvent
#undef D3DPERF_GetStatus
#undef D3DPERF_QueryRepeatFrame
#undef D3DPERF_SetMarker
#undef D3DPERF_SetOptions
#undef D3DPERF_SetRegion
#undef DebugSetLevel
#undef DebugSetMute

#include <cstdint>
#include <cstring>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "kernel32.lib")


// ═════════════════════════════════════════════════════════════════════════════
//  PART 0 — REAL d3d9.dll LOADER
// ═════════════════════════════════════════════════════════════════════════════

typedef IDirect3D9* (WINAPI* PFN_Create9)(UINT);
typedef HRESULT     (WINAPI* PFN_Create9Ex)(UINT, IDirect3D9Ex**);
typedef void*       (WINAPI* PFN_ShaderVal)();
typedef int         (WINAPI* PFN_PerfBegin)(D3DCOLOR, LPCWSTR);
typedef int         (WINAPI* PFN_PerfEnd)();
typedef DWORD       (WINAPI* PFN_PerfStatus)();
typedef BOOL        (WINAPI* PFN_PerfQuery)();
typedef void        (WINAPI* PFN_PerfMarker)(D3DCOLOR, LPCWSTR);
typedef void        (WINAPI* PFN_PerfOpts)(DWORD);
typedef void        (WINAPI* PFN_PerfRegion)(D3DCOLOR, LPCWSTR);
typedef HRESULT     (WINAPI* PFN_DbgLevel)(DWORD);
typedef void        (WINAPI* PFN_DbgMute)();

static HMODULE      g_real       = nullptr;
static PFN_Create9  g_Create9    = nullptr;
static PFN_Create9Ex g_Create9Ex = nullptr;
static PFN_ShaderVal g_Shader    = nullptr;
static PFN_PerfBegin g_PBegin    = nullptr;
static PFN_PerfEnd   g_PEnd      = nullptr;
static PFN_PerfStatus g_PStatus  = nullptr;
static PFN_PerfQuery  g_PQuery   = nullptr;
static PFN_PerfMarker g_PMarker  = nullptr;
static PFN_PerfOpts   g_POpts    = nullptr;
static PFN_PerfRegion g_PRegion  = nullptr;
static PFN_DbgLevel   g_DbgLvl   = nullptr;
static PFN_DbgMute    g_DbgMute  = nullptr;

static bool LoadRealD3D9()
{
    char path[MAX_PATH];
    GetSystemDirectoryA(path, MAX_PATH);
    strcat_s(path, "\\d3d9.dll");
    g_real = LoadLibraryA(path);
    if (!g_real) return false;

#define GP(n,p) p = (decltype(p))GetProcAddress(g_real, n)
    GP("Direct3DCreate9",                g_Create9);
    GP("Direct3DCreate9Ex",              g_Create9Ex);
    GP("Direct3DShaderValidatorCreate9", g_Shader);
    GP("D3DPERF_BeginEvent",             g_PBegin);
    GP("D3DPERF_EndEvent",               g_PEnd);
    GP("D3DPERF_GetStatus",              g_PStatus);
    GP("D3DPERF_QueryRepeatFrame",       g_PQuery);
    GP("D3DPERF_SetMarker",              g_PMarker);
    GP("D3DPERF_SetOptions",             g_POpts);
    GP("D3DPERF_SetRegion",              g_PRegion);
    GP("DebugSetLevel",                  g_DbgLvl);
    GP("DebugSetMute",                   g_DbgMute);
#undef GP
    return true;
}


// ═════════════════════════════════════════════════════════════════════════════
//  PART 0B — PROXY EXPORTS
//  اسم واقعی d3d9 — conflict نداره چون d3d9.h اونا رو به _H_* تغییر داده
// ═════════════════════════════════════════════════════════════════════════════

extern "C" {

__declspec(dllexport) IDirect3D9* WINAPI
Direct3DCreate9(UINT sdk)
{ return g_Create9 ? g_Create9(sdk) : nullptr; }

__declspec(dllexport) HRESULT WINAPI
Direct3DCreate9Ex(UINT sdk, IDirect3D9Ex** pp)
{ return g_Create9Ex ? g_Create9Ex(sdk, pp) : E_FAIL; }

__declspec(dllexport) void* WINAPI
Direct3DShaderValidatorCreate9()
{ return g_Shader ? g_Shader() : nullptr; }

__declspec(dllexport) int WINAPI
D3DPERF_BeginEvent(D3DCOLOR c, LPCWSTR n)
{ return g_PBegin ? g_PBegin(c, n) : 0; }

__declspec(dllexport) int WINAPI
D3DPERF_EndEvent()
{ return g_PEnd ? g_PEnd() : 0; }

__declspec(dllexport) DWORD WINAPI
D3DPERF_GetStatus()
{ return g_PStatus ? g_PStatus() : 0; }

__declspec(dllexport) BOOL WINAPI
D3DPERF_QueryRepeatFrame()
{ return g_PQuery ? g_PQuery() : FALSE; }

__declspec(dllexport) void WINAPI
D3DPERF_SetMarker(D3DCOLOR c, LPCWSTR n)
{ if (g_PMarker) g_PMarker(c, n); }

__declspec(dllexport) void WINAPI
D3DPERF_SetOptions(DWORD o)
{ if (g_POpts) g_POpts(o); }

__declspec(dllexport) void WINAPI
D3DPERF_SetRegion(D3DCOLOR c, LPCWSTR n)
{ if (g_PRegion) g_PRegion(c, n); }

__declspec(dllexport) HRESULT WINAPI
DebugSetLevel(DWORD lvl)
{ return g_DbgLvl ? g_DbgLvl(lvl) : S_OK; }

__declspec(dllexport) void WINAPI
DebugSetMute()
{ if (g_DbgMute) g_DbgMute(); }

} // extern "C"


// ═════════════════════════════════════════════════════════════════════════════
//  PART 1 — MEMORY HELPERS
// ═════════════════════════════════════════════════════════════════════════════

static void PatchBytes(uintptr_t addr, const uint8_t* d, size_t n)
{
    DWORD old;
    VirtualProtect((void*)addr, n, PAGE_EXECUTE_READWRITE, &old);
    memcpy((void*)addr, d, n);
    VirtualProtect((void*)addr, n, old, &old);
    FlushInstructionCache(GetCurrentProcess(), (void*)addr, n);
}

template<typename T>
static void PatchVal(uintptr_t addr, T v)
{ PatchBytes(addr, (const uint8_t*)&v, sizeof(T)); }


// ═════════════════════════════════════════════════════════════════════════════
//  PART 2 — 200 FPS UNLOCK
// ═════════════════════════════════════════════════════════════════════════════

static constexpr int   FPS    = 200;
static constexpr float FPS_MS = 1000.0f / FPS;

static void PatchFPS()
{
    PatchVal<uint8_t>(0x00C1703C, 0x00);
    PatchVal<float>  (0x00C17040, FPS_MS);
    static const uint8_t nop5[] = {0x90,0x90,0x90,0x90,0x90};
    PatchBytes(0x0053E943, nop5, 5);
}

typedef HRESULT(APIENTRY* PFN_Present)(
    IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
static PFN_Present   g_oPresent = nullptr;
static LARGE_INTEGER g_freq     = {};
static LARGE_INTEGER g_last     = {};
static bool          g_timerOK  = false;

static void CapFPS()
{
    if (!g_timerOK) return;
    LONGLONG tpf = g_freq.QuadPart / FPS;
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    LONGLONG el = now.QuadPart - g_last.QuadPart;
    if (el < tpf)
    {
        LONGLONG rem = tpf - el - g_freq.QuadPart / 2000;
        if (rem > 0)
        {
            DWORD ms = (DWORD)((rem * 1000) / g_freq.QuadPart);
            if (ms > 0) Sleep(ms - 1);
        }
        do { QueryPerformanceCounter(&now); }
        while ((now.QuadPart - g_last.QuadPart) < tpf);
    }
    g_last = now;
}

static HRESULT APIENTRY hkPresent(
    IDirect3DDevice9* dev, const RECT* s, const RECT* d,
    HWND hw, const RGNDATA* rg)
{ CapFPS(); return g_oPresent(dev, s, d, hw, rg); }

static void HookPresent()
{
    // از g_Create9 مستقیم — نه Direct3DCreate9 که loop میکنه
    if (!g_Create9) return;
    IDirect3D9* d3d = g_Create9(D3D_SDK_VERSION);
    if (!d3d) return;

    D3DPRESENT_PARAMETERS pp = {};
    pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    pp.Windowed   = TRUE;
    pp.BackBufferFormat = D3DFMT_UNKNOWN;
    pp.BackBufferCount  = 1;
    pp.BackBufferWidth  = 2;
    pp.BackBufferHeight = 2;
    pp.hDeviceWindow    = GetDesktopWindow();

    IDirect3DDevice9* dev = nullptr;
    if (FAILED(d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_NULLREF,
        pp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pp, &dev)))
    { d3d->Release(); return; }

    void** vmt = *(void***)dev;
    DWORD old = 0;
    VirtualProtect(&vmt[17], sizeof(void*), PAGE_EXECUTE_READWRITE, &old);
    g_oPresent = (PFN_Present)vmt[17];
    vmt[17]    = (void*)hkPresent;
    VirtualProtect(&vmt[17], sizeof(void*), old, &old);
    dev->Release();
    d3d->Release();
}


// ═════════════════════════════════════════════════════════════════════════════
//  PART 3 — THREAD POOL 96 → 200
// ═════════════════════════════════════════════════════════════════════════════

static constexpr size_t SSIZE   = 0xE4;
static constexpr int    OLD_CNT = 96;
static constexpr int    NEW_CNT = 200;

static uint8_t* g_pool = nullptr;

static void ExpandPool()
{
    g_pool = (uint8_t*)VirtualAlloc(nullptr, NEW_CNT * SSIZE,
                                    MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
    if (!g_pool) return;
    memcpy(g_pool, (void*)0x00A8D040, OLD_CNT * SSIZE);
    memset(g_pool + OLD_CNT * SSIZE, 0, (NEW_CNT - OLD_CNT) * SSIZE);

    static const uintptr_t REFS[] =
        {0x0045D650, 0x0046A4B5, 0x00480090, 0x004800D0};
    for (auto a : REFS)
        PatchVal<uint32_t>(a + 3, (uint32_t)(uintptr_t)g_pool);

    static const uint8_t C200[] = {0xC8,0x00,0x00,0x00};
    static const uintptr_t CNTS[] =
        {0x0045D690, 0x0046A4E0, 0x004800A5};
    for (auto a : CNTS)
        PatchBytes(a, C200, 4);
}


// ═════════════════════════════════════════════════════════════════════════════
//  PART 4 — main2.scm LOADER
// ═════════════════════════════════════════════════════════════════════════════

#pragma pack(push,1)
struct CRunningScript {
    uint8_t  _p0[8];
    char     m_szName[8];
    uint8_t* m_pBaseIP;
    uint8_t* m_pIP;
    uint8_t* m_pStack[8];
    uint16_t m_nSP;
    uint8_t  _p1[2];
    int32_t  m_aLocals[40];
    uint8_t  m_bIsActive;
    uint8_t  m_bCondResult;
    uint8_t  _p2[6];
};
#pragma pack(pop)

typedef CRunningScript*(__cdecl* PFN_StartScript)(uint8_t*);
static uint8_t* g_scm2    = nullptr;
static DWORD    g_scm2Sz  = 0;

static void LoadMain2()
{
    char exe[MAX_PATH];
    GetModuleFileNameA(nullptr, exe, MAX_PATH);
    char* sl = strrchr(exe,'\\');
    if (sl) *(sl+1) = '\0';
    char path[MAX_PATH];
    sprintf_s(path, "%sdata\\script\\main2.scm", exe);

    HANDLE hf = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ,
                            nullptr, OPEN_EXISTING, 0, nullptr);
    if (hf == INVALID_HANDLE_VALUE) return;

    g_scm2Sz = GetFileSize(hf, nullptr);
    if (!g_scm2Sz || g_scm2Sz == INVALID_FILE_SIZE)
    { CloseHandle(hf); return; }

    g_scm2 = (uint8_t*)VirtualAlloc(nullptr, g_scm2Sz,
                                    MEM_COMMIT|MEM_RESERVE,
                                    PAGE_EXECUTE_READWRITE);
    if (!g_scm2) { CloseHandle(hf); return; }

    DWORD rd = 0;
    ReadFile(hf, g_scm2, g_scm2Sz, &rd, nullptr);
    CloseHandle(hf);
    if (rd != g_scm2Sz)
    { VirtualFree(g_scm2,0,MEM_RELEASE); g_scm2=nullptr; return; }

    auto pfn = (PFN_StartScript)0x0046A4B0;
    CRunningScript* t = pfn(g_scm2);
    if (t)
    {
        strncpy_s(t->m_szName, "MAIN2", 8);
        t->m_bIsActive   = 1;
        t->m_pBaseIP     = g_scm2;
        t->m_bCondResult = 0;
    }
}


// ═════════════════════════════════════════════════════════════════════════════
//  PART 5 — ScriptsInit HOOK
// ═════════════════════════════════════════════════════════════════════════════

typedef void(__cdecl* PFN_SI)();
static PFN_SI g_oSI = nullptr;

static void __cdecl hkSI()
{ if (g_oSI) g_oSI(); LoadMain2(); }

static void HookScriptsInit()
{
    uint8_t* t = (uint8_t*)0x0045D440;
    DWORD old;
    VirtualProtect(t, 5, PAGE_EXECUTE_READWRITE, &old);
    g_oSI = (PFN_SI)(uintptr_t)(0x0045D440 + 5 + *(int32_t*)(t+1));
    t[0] = 0xE9;
    *(int32_t*)(t+1) = (int32_t)((uintptr_t)hkSI - 0x0045D440 - 5);
    VirtualProtect(t, 5, old, &old);
    FlushInstructionCache(GetCurrentProcess(), t, 5);
}


// ═════════════════════════════════════════════════════════════════════════════
//  PART 6 — INIT + DllMain
// ═════════════════════════════════════════════════════════════════════════════

static DWORD WINAPI InitThread(LPVOID)
{
    Sleep(3000);
    if (QueryPerformanceFrequency(&g_freq))
    { QueryPerformanceCounter(&g_last); g_timerOK = true; }
    timeBeginPeriod(1);
    PatchFPS();
    HookPresent();
    ExpandPool();
    HookScriptsInit();
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hMod, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hMod);
        if (!LoadRealD3D9()) return FALSE;
        CloseHandle(CreateThread(nullptr,0,InitThread,nullptr,0,nullptr));
    }
    else if (reason == DLL_PROCESS_DETACH)
    {
        timeEndPeriod(1);
        if (g_pool)  VirtualFree(g_pool, 0, MEM_RELEASE);
        if (g_scm2)  VirtualFree(g_scm2, 0, MEM_RELEASE);
        if (g_real)  FreeLibrary(g_real);
    }
    return TRUE;
}