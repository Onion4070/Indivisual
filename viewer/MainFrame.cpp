#include "MainFrame.h"
#include <wx/wx.h>

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title) {
	// 描画用パネル
	m_drawPanel = new DrawPanel(this);
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(m_drawPanel, 1, wxEXPAND);
	this->SetSizer(sizer);
}