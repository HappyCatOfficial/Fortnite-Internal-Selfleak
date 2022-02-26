#include <Windows.h>
#include <string>
#include <vector>
#include <Psapi.h>
#include <intrin.h>
#include <sysinfoapi.h>
 
#include "../other/custom_winapi.hpp"
#include "../spoofer/spoofer.hpp"
#include "../other/settings.hpp"
#include "../other/definitions.hpp"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_internal.h"
#include "../other/offsets.hpp"
#include "../spoofer/lazy_importer.hpp"


#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")

#define rva(addr, size) ((uintptr_t)((UINT_PTR)(addr) + *(PINT)((UINT_PTR)(addr) + ((size) - sizeof(INT))) + (size)))

ID3D11Device* device = nullptr;
ID3D11DeviceContext* context = nullptr;
ID3D11RenderTargetView* render_target_view = nullptr;

bool enabled = true;

static HWND window = 0;
static WNDPROC window_ex = 0;

typedef HRESULT(__fastcall* present_scene_t)(IDXGISwapChain* swap_chain, UINT sync_interval, UINT flags);


present_scene_t present_og;


auto width = spoof_call(safe_call(GetSystemMetrics), SM_CXSCREEN);
auto height = spoof_call(safe_call(GetSystemMetrics), SM_CYSCREEN);



namespace utils
{
	uintptr_t uworld;
	uintptr_t worldtoscreen;
	uintptr_t local_pawn;
	uintptr_t player_controller;
    uintptr_t local_root_component;
    uintptr_t player_camera_manager;

	ImGuiWindow& begin_scene();
	void end_scene(ImGuiWindow& window);
}


namespace color
{
	ImColor black = { 0, 0, 0 };
	ImColor blue = { 100, 100, 255 };
	ImColor white = { 255, 255, 255 };
	ImColor green = { 0, 255, 0 };
	ImColor red = { 255, 0, 0 };
}

namespace scanners
{
    uintptr_t pattern_scanw(uintptr_t pModuleBaseAddress, const char* sSignature, size_t nSelectResultIndex = 0);
    uintptr_t pattern_scan(const char* sSignature, size_t nSelectResultIndex = 0);
}

uintptr_t scanners::pattern_scanw(uintptr_t pModuleBaseAddress, const char* sSignature, size_t nSelectResultIndex)
{
    static auto patternToByte = [](const char* pattern)
    {
        auto       bytes = std::vector<int>{};
        const auto start = const_cast<char*>(pattern);
        const auto end = const_cast<char*>(pattern) + spoof_call(custom::strlen, pattern);

        for (auto current = start; current < end; ++current)
        {
            if (*current == '?')
            {
                ++current;
                if (*current == '?')
                    ++current;
                bytes.push_back(-1);
            }
            else {
                bytes.push_back(spoof_call(custom::strtoul, (const char*)current, &current, 16));
            }
        }
        return bytes;
    };

    const auto dosHeader = (PIMAGE_DOS_HEADER)pModuleBaseAddress;
    const auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)pModuleBaseAddress + dosHeader->e_lfanew);

    const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
    auto       patternBytes = patternToByte(sSignature);
    const auto scanBytes = reinterpret_cast<std::uint8_t*>(pModuleBaseAddress);

    const auto s = patternBytes.size();
    const auto d = patternBytes.data();

    size_t nFoundResults = 0;

    for (auto i = 0UL; i < sizeOfImage - s; ++i)
    {
        bool found = true;

        for (auto j = 0UL; j < s; ++j)
        {
            if (scanBytes[i + j] != d[j] && d[j] != -1)
            {
                found = false;
                break;
            }
        }

        if (found)
        {
            if (nSelectResultIndex != 0)
            {
                if (nFoundResults < nSelectResultIndex)
                {
                    nFoundResults++;
                    found = false;
                }
                else
                {
                    return reinterpret_cast<uintptr_t>(&scanBytes[i]);
                }
            }
            else
            {
                return reinterpret_cast<uintptr_t>(&scanBytes[i]);
            }
        }
    }

    return NULL;
}

PEB* get_peb()
{
    PEB* peb = (PEB*)__readgsqword(0x60);

    return peb;
}

uintptr_t get_base_address(const wchar_t* mod_name)
{
    LDR_DATA_TABLE_ENTRY* mod_entry = nullptr;

    PEB* peb = get_peb();

    LIST_ENTRY head = peb->Ldr->InMemoryOrderModuleList;

    LIST_ENTRY curr = head;

    for (auto curr = head; curr.Flink != &peb->Ldr->InMemoryOrderModuleList; curr = *curr.Flink)
    {
        LDR_DATA_TABLE_ENTRY* mod = (LDR_DATA_TABLE_ENTRY*)CONTAINING_RECORD(curr.Flink, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

        if (mod->BaseDllName.Buffer)
        {
            if (custom::_wcsicmp((const wchar_t*)mod_name, (const wchar_t*)mod->BaseDllName.Buffer) == 0)
            {
                mod_entry = mod;
                break;
            }
        }
    }

    return (uintptr_t)mod_entry->DllBase;
}

uintptr_t scanners::pattern_scan(const char* sSignature, size_t nSelectResultIndex)
{
    static bool is_setted = false;

    static MODULEINFO info = { 0 };

    if (!is_setted)
    {
        spoof_call(safe_call(GetModuleInformation), (HANDLE)spoof_call(safe_call(GetCurrentProcess)), (HMODULE)get_base_address(_(L"FortniteClient-Win64-Shipping.exe")), &info, (DWORD)sizeof(info));
        is_setted = true;
    }
    return spoof_call(pattern_scanw, (uintptr_t)info.lpBaseOfDll, sSignature, nSelectResultIndex);
}

#define _ZeroMemory(x, y) (custom::memset(x, 0, y));

void erase_pe(void* pbase)
{
    auto base = (uintptr_t)pbase;
    auto dos_headers = (IMAGE_DOS_HEADER*)(base);
    auto pINH = (IMAGE_NT_HEADERS*)(base + dos_headers->e_lfanew);

    auto p_section_header = (IMAGE_SECTION_HEADER*)(pINH + 1);

    auto sizeOfImage = pINH->OptionalHeader.SizeOfImage;

    DWORD op;

    auto dir = pINH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

    auto iat = (IMAGE_IMPORT_DESCRIPTOR*)(base + dir.VirtualAddress);

    for (; iat->Name; ++iat)
    {
        auto mod_name = (char*)(base + (uintptr_t)iat->Name);
        auto entry = (IMAGE_THUNK_DATA64*)(base + iat->OriginalFirstThunk);

        for (uintptr_t index = 0; entry->u1.AddressOfData; index += sizeof(uintptr_t), ++entry)
        {
            auto pimport = (IMAGE_IMPORT_BY_NAME*)(base + entry->u1.AddressOfData);
            auto import_name = pimport->Name;
            auto len = custom::strlen(import_name);
            _ZeroMemory(import_name, len);
        }

        auto len = custom::strlen(mod_name);
        _ZeroMemory(mod_name, len);
    }


    for (int i = 0; i < pINH->FileHeader.NumberOfSections; i++)
    {
        auto section = p_section_header[i];
        auto name = section.Name;
        auto rva = section.VirtualAddress;
        auto size = section.SizeOfRawData;

        auto secbase = (uintptr_t)base + rva;
        auto secend = secbase + size;

        if (custom::strstr((const char*)name, _(".rsrc"))
            || custom::strstr((const char*)name, _(".reloc"))
            || custom::strstr((const char*)name, _(".pdata")))
        {
            _ZeroMemory((void*)secbase, size);
        }
    }

    _ZeroMemory((void*)base, pINH->OptionalHeader.SizeOfHeaders);
}


void draw_outlined_text(float x, float y, const char* text, ImColor color)
{
    ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y + 1), color::black, text);
    ImGui::GetOverlayDrawList()->AddText(ImVec2(x - 1, y - 1), color::black, text);
    ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y - 1), color::black, text);
    ImGui::GetOverlayDrawList()->AddText(ImVec2(x - 1, y + 1), color::black, text);
    ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), color, text);
}

void draw_cornered_box(int x, int y, int w, int h, ImColor color, int thickness) {
    float line_w = (w / 3);
    float line_h = (h / 3);
    ImGui::GetOverlayDrawList()->AddLine(ImVec2(x, y), ImVec2(x, y + line_h), color, thickness);
    ImGui::GetOverlayDrawList()->AddLine(ImVec2(x, y), ImVec2(x + line_w, y), color, thickness);
    ImGui::GetOverlayDrawList()->AddLine(ImVec2(x + w - line_w, y), ImVec2(x + w, y), color, thickness);
    ImGui::GetOverlayDrawList()->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + line_h), color, thickness);
    ImGui::GetOverlayDrawList()->AddLine(ImVec2(x, y + h - line_h), ImVec2(x, y + h), color, thickness);
    ImGui::GetOverlayDrawList()->AddLine(ImVec2(x, y + h), ImVec2(x + line_w, y + h), color, thickness);
    ImGui::GetOverlayDrawList()->AddLine(ImVec2(x + w - line_w, y + h), ImVec2(x + w, y + h), color, thickness);
    ImGui::GetOverlayDrawList()->AddLine(ImVec2(x + w, y + h - line_h), ImVec2(x + w, y + h), color, thickness);
}
