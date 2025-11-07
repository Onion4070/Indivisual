#pragma once
#include <wx/wx.h>

// 描画用パネル
class DrawPanel : public wxPanel
{
public:
	DrawPanel(wxWindow* parent);
private:
	// 描画イベントハンドラ
	void OnPaint(wxPaintEvent& event);

	// イベントテーブル宣言
	wxDECLARE_EVENT_TABLE();
};

