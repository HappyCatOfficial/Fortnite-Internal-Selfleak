#pragma once
#include <Windows.h>
#include <cstdint>
#include "spoofer/lazy_importer.hpp"

#define bit_check(a,b) (!!((a) & (1ULL<<(b))))
#define read_byte(base, offset) (*(((PBYTE)base + offset)))

static int shoot_downed = 0;
float aim_speed = 3.0f;
float aim_smooth = 3.0f;

void aimbot(float xscreen, float yscreen, float X, float Y, int smooth, int smooth2, bool is_visible, uint64_t current_pawn)
{
    std::uint64_t currentactormesh = structs::read<std::uint64_t>(current_pawn + offsets::mesh);

    if (!currentactormesh) return;

    std::uint8_t is_dying = structs::read<std::uint8_t>(current_pawn + offsets::is_dying);

    if (bit_check(is_dying, 3))
    {
        return;
    }

    if (read_byte(current_pawn, 0x57e) & 1 && !shoot_downed) {
        return;
    }


    float screen_center_x = (X / 2);
    float screen_center_y = (Y / 2);
    float target_x = 0;
    float target_y = 0;


    if (xscreen != 0)
    {
        if (xscreen > screen_center_x)
        {
            target_x = -(screen_center_x - xscreen);
            target_x /= aim_speed;
            if (target_x + screen_center_x > screen_center_x * 2) target_x = 0;
        }

        if (xscreen < screen_center_x)
        {
            target_x = xscreen - screen_center_x;
            target_x /= aim_speed;
            if (target_x + screen_center_x < 0) target_x = 0;
        }
    }

    if (yscreen != 0)
    {
        if (yscreen > screen_center_y)
        {
            target_y = -(screen_center_y - yscreen);
            target_y /= aim_speed;
            if (target_y + screen_center_y > screen_center_y * 2) target_y = 0;
        }

        if (yscreen < screen_center_y)
        {
            target_y = yscreen - screen_center_y;
            target_y /= aim_speed;
            if (target_y + screen_center_y < 0) target_y = 0;
        }
    }

    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.mouseData = 0;
    input.mi.time = 0;
    input.mi.dx = custom::floor(target_x / settings::Xsmooth);
    input.mi.dy = custom::floor(target_y / settings::Ysmooth);
    input.mi.dwFlags = MOUSEEVENTF_MOVE;
    input.mi.dwExtraInfo = 0;

    spoof_call(safe_call(SendInput), (UINT)1, (LPINPUT)&input, (int)sizeof(input));
}