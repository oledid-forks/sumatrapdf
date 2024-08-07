/* Copyright 2022 the SumatraPDF project authors (see AUTHORS file).
   License: Simplified BSD (see COPYING.BSD) */

#include "utils/BaseUtil.h"
#include "utils/ScopedWin.h"
#include "utils/BitManip.h"
#include "utils/WinUtil.h"
#include "utils/Dpi.h"
#include "utils/WinDynCalls.h"

#include "wingui/UIModels.h"

#include "wingui/Layout.h"
#include "wingui/WinGui.h"
#include "wingui/VirtWnd.h"

#include "utils/Log.h"

Kind kindVirtWnd = "kindVirtWnd";

VirtWnd::VirtWnd() {
    kind = kindVirtWnd;
}

int VirtWnd::MinIntrinsicHeight(int) {
    return 0;
}

int VirtWnd::MinIntrinsicWidth(int) {
    return 0;
}

Size VirtWnd::Layout(Constraints) {
    return {};
}

Size VirtWnd::GetIdealSize() {
    return {};
}

void VirtWnd::Paint(HDC) {
}

Kind kindVirWndText = "virtWndText";

VirtWndText::VirtWndText(HWND hwnd, const char* s, HFONT font) {
    kind = kindVirWndText;
    this->s = str::Dup(s);
    this->hwnd = hwnd;
    this->font = font;
}

VirtWndText::~VirtWndText() {
    str::Free(s);
}

Size VirtWndText::Layout(const Constraints bc) {
    GetIdealSize();
    return bc.Constrain({sz.dx, sz.dy});
}

int VirtWndText::MinIntrinsicHeight(int width) {
    GetIdealSize(true);
    return sz.dy;
}

int VirtWndText::MinIntrinsicWidth(int height) {
    GetIdealSize(true);
    return sz.dx;
}

Size VirtWndText::MinIntrinsicSize(int width, int height) {
    int dx = MinIntrinsicWidth(height);
    int dy = MinIntrinsicHeight(width);
    return {dx, dy};
}

Size VirtWndText::GetIdealSize(bool onlyIfEmpty) {
    if (onlyIfEmpty && !sz.IsEmpty()) {
        return sz;
    }
    sz = HwndMeasureText(hwnd, s, font);
    return sz;
}

void VirtWndText::Draw(HDC hdc) {
    ReportIf(lastBounds.IsEmpty());
    UINT fmt = DT_NOCLIP | DT_NOPREFIX | (isRtl ? DT_RTLREADING : DT_LEFT);
    RECT dr = ToRECT(lastBounds);
    HdcDrawText(hdc, s, &dr, fmt, font);
    if (withUnderline) {
        auto& r = lastBounds;
        Rect lineRect = {r.x, r.y + sz.dy, sz.dx, 0};
        DrawLine(hdc, lineRect);
    }
}
