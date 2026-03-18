#pragma once
#include "ControllerState.h"

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include <thread>
#include <atomic>
#include <chrono>

enum class DpadDir { UP, DOWN, LEFT, RIGHT };

class DrawPanel : public wxPanel
{
public:
    DrawPanel(wxWindow* parent, ControllerStateManager& controller);
    virtual ~DrawPanel();

private:
    // 描画イベントハンドラ
    void OnPaint(wxPaintEvent& event);

    // 描画コアロジック
    void ClearBackground(wxGCDC& gdc);
    void DebugRender(wxGCDC& gdc, ControllerState& controller);
    void RenderFrame(wxGCDC& gdc);
    void DrawStick(wxGCDC& gdc, int cx, int cy, int r, float sx, float sy, bool pressed, wxColour color);
    void DrawDpadFrame(wxGCDC& gdc, int cx, int cy, int size);
    void DrawDpad(wxGCDC& gdc, int cx, int cy, int size, DpadDir dir, bool pressed);
    void DrawBtn(wxGCDC& gdc, int bx, int by, int btn_r, bool pressed, const wxString& label, wxColour color);
    void DrawPlusMinus(wxGCDC& gdc, int cx, int cy, int r, bool pressed, bool is_plus);
    void DrawTrigger(wxGCDC& gdc, int tx, int ty, int w, int h, bool pressed, const wxString& label);

    ControllerStateManager& m_controller;

    // スレッド関連
    std::thread m_timer_thread;
    std::atomic<bool> m_running;

    // 描画リソース・状態管理
    wxPen m_blue_pen;
    wxPen m_any_color_pen;

    int m_fps = 0;
	int m_target_fps = 120;     // 目標FPS
    int m_frame_count = 0;
    wxLongLong m_last_time = 0;

    // アニメーション用変数
    int m_anim_x = 0;
    int m_anim_y = 0;

    wxDECLARE_EVENT_TABLE();
};
