#include "../include/types.h"
#include "../include/config.h"
#include "../include/window.h"
#include "../include/script.h"
#include "../include/constants/weather_numbers.h"

#ifdef IMPLEMENT_TRANSPARENT_TEXTBOXES

void Tr_TextBox(GF_BGL_BMPWIN* win)
{
    u8 type = 0;
    u32 weather = Fsys_GetWeather_HandleDiamondDust(gFieldSysPtr, gFieldSysPtr->location->mapId);

    if (weather != WEATHER_SYS_MIST1 && weather != WEATHER_SYS_MIST2)
    {
        reg_G2_BLDCNT = 0x1b4f;  // Enable blending for BG0/1/3 and OBJ
        reg_G2_BLDALPHA = 0x0510;  // Alpha blend: first 10/16, second 5/16
    }

    // Configure and render the text box
    TalkWinGraphicSet(win->ini, GF_BGL_BmpWinGet_Frame(win), 944, 10, 1, 4);

    // Determine type if frame > 4
    if (GF_BGL_BmpWinGet_Frame(win) > 4)
    {
        type = 4;
    }

    ArcUtil_PalSet(38, 51, type, 10 * 0x20, 0x20, 4);  // Load palette
    FieldTalkWinClear(win);  // Clear the textbox
    BmpTalkWinWrite(win, 0, 944, 10);  // Render the text box
}



void FieldMsgPrintInit(u32 type, u32 init)
{
    if (init == 1)
    {
        MSG_PrintInit();
    }
    SystemFontPaletteLoad(type, 0x1a0, 4);

    ArcUtil_PalSet(16, 10, type, 0x180, 0x20, 4);
}

#endif
