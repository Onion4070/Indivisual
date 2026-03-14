#include "DrawPanel.h"
#include <cmath>

#ifdef __WXMSW__
#include <timeapi.h>
#pragma comment(lib, "winmm.lib")
#endif

wxBEGIN_EVENT_TABLE(DrawPanel, wxPanel)
EVT_PAINT(DrawPanel::OnPaint)
wxEND_EVENT_TABLE()

DrawPanel::DrawPanel(wxWindow* parent, ControllerStateManager& controller)
	: wxPanel(parent, wxID_ANY), m_running(false), m_controller(controller) {
    // 背景を黒に設定
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetBackgroundColour(*wxBLACK);

    // ペンの初期化
    m_blue_pen = wxPen(*wxBLUE, 2, wxPENSTYLE_SOLID);
    m_any_color_pen = wxPen(wxColor(255, 100, 100), 5);

#ifdef __WXMSW__
    timeBeginPeriod(1); // Windowsのタイマー精度を1msに引き上げ
#endif

    // タイマースレッドの開始
    m_running = true;
    m_timer_thread = std::thread([this]() {
        auto next_frame = std::chrono::steady_clock::now();

        while (m_running) {
            // 120fps = 8.333 ms = 8333 µs 
            next_frame += std::chrono::microseconds(1'000'000 / m_target_fps);

            // CPU負荷を抑えるためにスリープ
            std::this_thread::sleep_until(next_frame);

            if (m_running) {
                // CallAfterによりスレッドセーフにRefreshを呼び出す
                GetEventHandler()->CallAfter([this]() {
                    Refresh(false);
                });
            }
        }
    });
}

DrawPanel::~DrawPanel() {
    m_running = false;
    if (m_timer_thread.joinable()) {
        m_timer_thread.join();
    }
#ifdef __WXMSW__
    timeEndPeriod(1); // タイマー精度設定を解除
#endif
}

void DrawPanel::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    wxGCDC gdc(dc);
    RenderFrame(gdc);
}

void DrawPanel::ClearBackground(wxGCDC& gdc) {
    gdc.SetBrush(wxBrush(GetBackgroundColour()));
    gdc.SetPen(*wxTRANSPARENT_PEN);
    gdc.DrawRectangle(GetClientRect());
}

void DrawPanel::RenderFrame(wxGCDC& gdc) {
    wxLongLong now = wxGetLocalTimeMillis();
    ClearBackground(gdc);

    // FPS計測
    m_frame_count++;
    if (now - m_last_time >= 1000) {
        m_fps = m_frame_count;
        m_frame_count = 0;
        m_last_time = now;
    }

    // 描画処理

    // 回る円
    gdc.SetPen(m_blue_pen);
    gdc.SetBrush(wxBrush(*wxWHITE));
    gdc.DrawCircle(400 + 300*sin(M_PI/180*m_anim_x), 300, 50);
    m_anim_x += 1;
    m_anim_y += 1;

	// 正八角形を描画
    gdc.SetPen(m_any_color_pen);
    wxPoint points[8];
    for (int i = 0; i < 8; i++) {
        double angle = i * (2 * M_PI / 8);
        points[i] = wxPoint(500 + 50 * cos(angle), 100 + 50 * sin(angle));
    }
    gdc.DrawPolygon(8, points);

    gdc.SetTextForeground(*wxWHITE);
    gdc.DrawText(wxString::Format("FPS: %d (%d fps target)", m_fps, m_target_fps), 10, 10);

    if (!m_controller.IsConnected()) {
        gdc.SetTextForeground(*wxWHITE);
        gdc.DrawText("Not connected.", 10, 40);
        return;
    }

    auto state = m_controller.Get();

    wxFont mono_font(12, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    gdc.SetFont(mono_font);

    gdc.SetTextForeground(*wxWHITE);
    wxString index;
    wxString hex_str;
    for (int i = 0; i < state.length; i++) {
        index   += wxString::Format("%02d ", i);
        hex_str += wxString::Format("%02X ", state.raw[i]);
    }
    gdc.DrawText(index, 10, 30);
    gdc.DrawText(hex_str, 10, 50);
    //for (int i = 0; i < state.length; i++) {
    //    printf("%02X ", state.raw[i]);
    //}
    //std::cout << "\n";
}