#pragma once
#include "ControllerState.h"

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include <thread>
#include <atomic>
#include <chrono>

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
    void RenderFrame(wxGCDC& gdc);

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
