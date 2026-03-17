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

void DrawPanel::DebugRender(wxGCDC& gdc, ControllerState& controller) {
    // ===== 生パケット =====
    wxFont mono_font(12, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    gdc.SetFont(mono_font);

    gdc.SetTextForeground(*wxWHITE);
    wxString index;

    const int bytes_per_line = 24;
    for (int i = 0; i < bytes_per_line; i++) {
        index += wxString::Format("%02d ", i);
    }
    gdc.DrawText(index, 10, 60);

    for (int line = 0; line * bytes_per_line < controller.length; line++) {
        wxString hex_str;
        for (int i = 0; i < bytes_per_line && line * bytes_per_line + i < controller.length; i++) {
            hex_str += wxString::Format("%02X ", controller.raw[line * bytes_per_line + i]);
        }
        gdc.DrawText(hex_str, 10, 80 + line * 20);
    }

    // ===== 各種ボタン =====
    wxFont mono_font_big(36, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    gdc.SetFont(mono_font_big);

    if (controller.btn_a) gdc.DrawText("A", 300, 200);
    if (controller.btn_b) gdc.DrawText("B", 300, 200);
    if (controller.btn_x) gdc.DrawText("X", 300, 200);
    if (controller.btn_y) gdc.DrawText("Y", 300, 200);

    if (controller.btn_l)  gdc.DrawText("L", 300, 200);
    if (controller.btn_r)  gdc.DrawText("R", 300, 200);
    if (controller.btn_zl) gdc.DrawText("ZL", 300, 200);
    if (controller.btn_zr) gdc.DrawText("ZR", 300, 200);

    if (controller.btn_plus)    gdc.DrawText("+", 300, 200);
    if (controller.btn_minus)   gdc.DrawText("-", 300, 200);
    if (controller.btn_home)    gdc.DrawText("Home", 300, 200);
    if (controller.btn_capture) gdc.DrawText("Capture", 300, 200);

    if (controller.btn_lstick) gdc.DrawText("Ls", 300, 200);
    if (controller.btn_rstick) gdc.DrawText("Rs", 300, 200);
    if (controller.btn_chat)   gdc.DrawText("C", 300, 200);

    if (controller.btn_gl) gdc.DrawText("GL", 300, 200);
    if (controller.btn_gr) gdc.DrawText("GR", 300, 200);

    if (controller.dpad_up)    gdc.DrawText("↑", 300, 200);
    if (controller.dpad_down)  gdc.DrawText("↓", 300, 200);
    if (controller.dpad_left)  gdc.DrawText("←", 300, 200);
    if (controller.dpad_right) gdc.DrawText("→", 300, 200);
}

void DrawPanel::RenderFrame(wxGCDC& gdc) {
    wxLongLong now = wxGetLocalTimeMillis();
    ClearBackground(gdc);

    const int W = GetSize().GetWidth();
    const int H = GetSize().GetHeight();

    // FPS計測
    m_frame_count++;
    if (now - m_last_time >= 1000) {
        m_fps = m_frame_count;
        m_frame_count = 0;
        m_last_time = now;
    }

    // ステータスバー
    gdc.SetTextForeground(*wxWHITE);
    wxFont status_font(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    gdc.SetFont(status_font);
    gdc.DrawText(wxString::Format("FPS: %d", m_fps), 10, 10);

    if (!m_controller.IsConnected()) {
        gdc.SetTextForeground(*wxWHITE);
        gdc.DrawText("Not connected.", W / 2 - 50, H / 2);
        return;
    }

    ControllerState controller = m_controller.Get();

    // ===== スティック =====
    const int stick_r = std::min(W, H) * 0.12;
    const int lsx = W * 0.22, lsy = H * 0.55;
    const int rsx = W * 0.60, rsy = H * 0.80;

    DrawStick(gdc, lsx, lsy, stick_r, controller.lx, controller.ly, controller.btn_lstick, *wxWHITE);
    DrawStick(gdc, rsx, rsy, stick_r, controller.rx, controller.ry, controller.btn_rstick, *wxYELLOW);

    // ===== 十字キー =====
    const int size = std::min(H, W) * 0.04;
    const int dpad_cx = W * 0.35;
    const int dpad_cy = H * 0.80;

    DrawDpadFrame(gdc, dpad_cx, dpad_cy, size);

    DrawDpad(gdc, dpad_cx, dpad_cy, size, DpadDir::DOWN,  controller.dpad_down);
    DrawDpad(gdc, dpad_cx, dpad_cy, size, DpadDir::LEFT,  controller.dpad_left);
    DrawDpad(gdc, dpad_cx, dpad_cy, size, DpadDir::UP,    controller.dpad_up);
    DrawDpad(gdc, dpad_cx, dpad_cy, size, DpadDir::RIGHT, controller.dpad_right);
    
    // ===== ABXYボタン =====
    const int btn_r = std::min(W, H) * 0.05;
    const int abxy_cx = W * 0.78;
    const int abxy_cy = H * 0.55;
    const int abxy_d = btn_r * 1.8;
    DrawBtn(gdc, abxy_cx, abxy_cy - abxy_d, btn_r, controller.btn_x, "X", wxColour(100, 100, 255));
    DrawBtn(gdc, abxy_cx, abxy_cy + abxy_d, btn_r, controller.btn_b, "B", wxColour(255, 100, 100));
    DrawBtn(gdc, abxy_cx - abxy_d, abxy_cy, btn_r, controller.btn_y, "Y", wxColour(255, 200, 0));
    DrawBtn(gdc, abxy_cx + abxy_d, abxy_cy, btn_r, controller.btn_a, "A", wxColour(100, 255, 100));

    // ===== +-ボタン =====
    const int pm_cx = W * 0.50;
    const int pm_cy = H * 0.40;
    const int pm_d = btn_r * 4.2;
    DrawBtn(gdc, pm_cx - pm_d, pm_cy, btn_r, controller.btn_minus, "-", wxColour(200, 200, 200));
    DrawBtn(gdc, pm_cx + pm_d, pm_cy, btn_r, controller.btn_plus, "+", wxColour(200, 200, 200));

    // ===== CaptureHomeボタン =====
    const int ch_cx = W * 0.50;
    const int ch_cy = H * 0.50;
    const int ch_d = btn_r * 2.2;
    DrawBtn(gdc, ch_cx - ch_d, ch_cy, btn_r, controller.btn_capture, "C", wxColour(60, 200, 200));
    DrawBtn(gdc, ch_cx + ch_d, ch_cy, btn_r, controller.btn_home   , "H", wxColour(60, 200, 200));


    // ===== LR/ZLZRボタン =====
    const int lr_y = H * 0.22;
    const int zlzr_y = H * 0.15;
    const int lr_w = W * 0.10;
    const int lr_h = H * 0.06;
    DrawTrigger(gdc, W * 0.22, lr_y, lr_w, lr_h, controller.btn_l, "L");
    DrawTrigger(gdc, W * 0.78, lr_y, lr_w, lr_h, controller.btn_r, "R");
    DrawTrigger(gdc, W * 0.22, zlzr_y, lr_w, lr_h, controller.btn_zl, "ZL");
    DrawTrigger(gdc, W * 0.78, zlzr_y, lr_w, lr_h, controller.btn_zr, "ZR");

#ifdef _DEBUG
    DebugRender(gdc, controller);
#endif
}

void DrawPanel::DrawStick(wxGCDC& gdc, int cx, int cy, int r, float sx, float sy, bool pressed, wxColour color) {
    // 外枠（八角形）
    wxPoint points[8];
    for (int i = 0; i < 8; i++) {
        double angle = i * (2 * M_PI / 8);
        points[i] = wxPoint(cx + r * cos(angle), cy + r * sin(angle));
    }
    gdc.SetPen(wxPen(color, 3));
    gdc.SetBrush(*wxTRANSPARENT_BRUSH);
    gdc.DrawPolygon(8, points);

    // スティック位置
    int dx = cx + (int)(sx * r * 0.6);
    int dy = cy + (int)(sy * r * 0.6);
    gdc.SetPen(wxPen(pressed ? *wxRED : color, 3));
    gdc.SetBrush(GetBackgroundColour());
    gdc.DrawCircle(dx, dy, r * 0.6);
}

void DrawPanel::DrawDpadFrame(wxGCDC& gdc, int cx, int cy, int size) {
    wxPoint cross[12] = {
    {cx - size, cy - size},
    {cx - size, cy - 3 * size},
    {cx + size, cy - 3 * size},
    {cx + size, cy - size},
    {cx + 3 * size, cy - size},
    {cx + 3 * size, cy + size},
    {cx + size, cy + size},
    {cx + size, cy + 3 * size},
    {cx - size, cy + 3 * size},
    {cx - size, cy + size},
    {cx - 3 * size, cy + size},
    {cx - 3 * size, cy - size},
    };
    gdc.SetPen(wxPen(*wxWHITE, 2));
    gdc.SetBrush(*wxTRANSPARENT_BRUSH); // 塗りつぶしなし
    gdc.DrawPolygon(12, cross);
}

void DrawPanel::DrawDpad(wxGCDC& gdc, int cx, int cy, int size, DpadDir dir, bool pressed) {
    if (!pressed) return;
    switch (dir) {
    case DpadDir::UP:    cy++; break;
    case DpadDir::DOWN:  cy--; break;
    case DpadDir::LEFT:  cx++; break;
    case DpadDir::RIGHT: cx--; break;
    }
    wxPoint base[5] = {
        {cx,        cy},
        {cx - size, cy - size},
        {cx - size, cy - 3 * size},
        {cx + size, cy - 3 * size},
        {cx + size, cy - size},
    };
    wxPoint pts[5];
    for (int i = 0; i < 5; i++) {
        int dx = base[i].x - cx;
        int dy = base[i].y - cy;
        switch (dir) {
        case DpadDir::UP:    pts[i] = { cx + dx, cy + dy }; break;
        case DpadDir::DOWN:  pts[i] = { cx - dx, cy - dy }; break;
        case DpadDir::LEFT:  pts[i] = { cx + dy, cy - dx }; break;
        case DpadDir::RIGHT: pts[i] = { cx - dy, cy + dx }; break;
        }
    }
    gdc.SetPen(*wxTRANSPARENT_PEN);
    gdc.SetBrush(wxBrush(*wxWHITE));
    gdc.DrawPolygon(5, pts);
}

void DrawPanel::DrawBtn(wxGCDC& gdc, int bx, int by, int btn_r, bool pressed, const wxString& label, wxColour color) {
    gdc.SetBrush(pressed ? wxBrush(color) : wxBrush(wxColour(80, 80, 80)));
    gdc.SetPen(*wxTRANSPARENT_PEN);
    gdc.DrawCircle(bx, by, btn_r);
    gdc.SetTextForeground(pressed ? *wxBLACK : *wxWHITE);
    wxFont btn_font(btn_r * 0.8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    gdc.SetFont(btn_font);
    int tw, th;
    gdc.GetTextExtent(label, &tw, &th);
    gdc.DrawText(label, bx - tw / 2, by - th / 2);
}

void DrawPanel::DrawTrigger(wxGCDC& gdc, int tx, int ty, int w, int h, bool pressed, const wxString& label) {
    gdc.SetBrush(pressed ? wxBrush(*wxWHITE) : wxBrush(wxColour(80, 80, 80)));
    gdc.SetPen(*wxTRANSPARENT_PEN);
    gdc.DrawRoundedRectangle(tx - w / 2, ty - h / 2, w, h, 5);
    gdc.SetTextForeground(pressed ? *wxBLACK : *wxWHITE);
    wxFont btn_font(h * 0.4, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    gdc.SetFont(btn_font);
    int tw, th;
    gdc.GetTextExtent(label, &tw, &th);
    gdc.DrawText(label, tx - tw / 2, ty - th / 2);
}