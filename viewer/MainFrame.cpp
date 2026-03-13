#include "MainFrame.h"
#include "SerialUtils.h"
#include <wx/wx.h>

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title) {
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	// 上部ツールバー
	wxPanel* toolbar = new wxPanel(this);
	wxBoxSizer* toolbar_sizer = new wxBoxSizer(wxHORIZONTAL);

	m_port_choice = new wxChoice(toolbar, wxID_ANY);
	wxButton* btn_connect = new wxButton(toolbar, wxID_ANY, "Connect");

	toolbar_sizer->Add(new wxStaticText(toolbar, wxID_ANY, "Port :"), 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	toolbar_sizer->Add(m_port_choice, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	toolbar_sizer->Add(btn_connect, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	toolbar->SetSizer(toolbar_sizer);


	// 描画用パネル
	m_draw_panel = new DrawPanel(this);

	sizer->Add(toolbar,     0, wxEXPAND);
	sizer->Add(m_draw_panel, 1, wxEXPAND);
	this->SetSizer(sizer);

	// Connectボタンをクリックしたときのイベント
	btn_connect->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
		if (m_port_choice->GetCount() == 0) return;
		wxString port = m_port_choice->GetString(m_port_choice->GetSelection());
		std::cout << "Connecting to " << port << std::endl;

		// 接続処理の実装...
	});

	// ポート選択コンボボックスをクリックしたときにポートリストを更新
	m_port_choice->Bind(wxEVT_LEFT_DOWN, [this](wxMouseEvent& event) {
		RefreshPorts(); 
		event.Skip();	// 元のクリックの実行
	});

	RefreshPorts();
}

void MainFrame::RefreshPorts() {
	m_port_choice->Clear();
	for (auto& p : SerialUtils::GetSerialPorts())
		m_port_choice->Append(p.port + " - " + p.description);
	if (m_port_choice->GetCount() > 0)
		m_port_choice->SetSelection(0);
}