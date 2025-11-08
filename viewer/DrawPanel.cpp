#include "DrawPanel.h"

wxBEGIN_EVENT_TABLE(DrawPanel, wxPanel)
	EVT_PAINT(DrawPanel::OnPaint)
wxEND_EVENT_TABLE()

DrawPanel::DrawPanel(wxWindow* parent) : wxPanel(parent, wxID_ANY) {
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	SetBackgroundColour(*wxWHITE);
}

void DrawPanel::ClearBackground(wxGCDC& gdc) {
	gdc.SetBrush(wxBrush(GetBackgroundColour()));
	gdc.SetPen(*wxTRANSPARENT_PEN);
	gdc.DrawRectangle(GetClientRect());
}

void DrawPanel::OnPaint(wxPaintEvent& event) {
	wxAutoBufferedPaintDC dc(this);
	wxGCDC gdc(dc);
	ClearBackground(gdc);

	wxPen bluePen(*wxBLUE, 2, wxPENSTYLE_SOLID); // 青色ペン，太さ2, 実線

	wxPen anyColorPen(wxColor(255, 100, 100), 5); // 任意の色(薄い赤)，太さ5

	gdc.SetPen(bluePen);
	gdc.DrawLine(50, 50, 200, 100); // (50, 50)から(200, 100)まで直線を描画

	gdc.DrawCircle(300, 100, 50); // 中心(300, 100), 半径50の円を描画

	// 正八角形を描画
	gdc.SetPen(anyColorPen);
	wxPoint points[8];
	for (int i = 0; i < 8; i++) {
		double angle = i * (2 * 3.14159 / 8); // 角度を計算
		points[i] = wxPoint(500 + 50 * cos(angle), 100 + 50 * sin(angle)); // 中心(500, 100), 半径50
	}
	gdc.DrawPolygon(8, points);
}
