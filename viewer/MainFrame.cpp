#include "MainFrame.h"
#include "SerialUtils.h"
#include <wx/wx.h>

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title) {
	// ウィンドウサイズ固定化
	wxWindow::SetWindowStyle(wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER);

	// ========== コンポーネント ==========
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	// 上部ツールバー
	wxPanel* toolbar = new wxPanel(this);
	wxBoxSizer* toolbar_sizer = new wxBoxSizer(wxHORIZONTAL);

	// COMポート選択
	m_port_choice = new wxChoice(toolbar, wxID_ANY);

	// 接続ボタン
	wxButton* btn_connect = new wxButton(toolbar, wxID_ANY, "Connect");

	// 描画用パネル
	m_draw_panel = new DrawPanel(this, m_controller);

	// ========== コンポーネント配置 ==========
	toolbar_sizer->Add(new wxStaticText(toolbar, wxID_ANY, "Port :"), 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	toolbar_sizer->Add(m_port_choice, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	toolbar_sizer->Add(btn_connect, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	toolbar->SetSizer(toolbar_sizer);

	sizer->Add(toolbar,      0, wxEXPAND);
	sizer->Add(m_draw_panel, 1, wxEXPAND);
	this->SetSizer(sizer);

	// ========== イベント ==========

	// Connectボタン
	btn_connect->Bind(wxEVT_BUTTON, [this, btn_connect](wxCommandEvent&) {
		// 未選択
		if (m_port_choice->GetCount() == 0) {
			std::cerr << "Error: COM Port is not selected." << std::endl;
			return;
		}

		// 接続済みの場合は切断
		if (m_serial.IsOpen()) {
			m_serial.Close();
			m_controller.SetConnected(false);
			btn_connect->SetLabel("Connect");
			std::cout << "Disconnected." << std::endl;
			return;
		}

		// ポートリストから取得して接続
		wxString selected = m_port_choice->GetString(m_port_choice->GetSelection());
		wxString port = selected.BeforeFirst(':');		// "COM3: ..." -> "COM3"
		std::cout << "Connecting to " << port << std::endl;

		bool ok = m_serial.Open(port.ToStdString(), [this](const uint8_t* data, uint8_t len) {
			m_controller.Update(data, len);
		});

		if (ok) {
			m_controller.SetConnected(true);
			btn_connect->SetLabel("Disconnect");
			std::cout << "Connected." << std::endl;
		}
		else {
			std::cerr << "Failed to open port." << std::endl;
		}
	});

	// ポート選択 (ポートリストを更新)
	m_port_choice->Bind(wxEVT_LEFT_DOWN, [this](wxMouseEvent& event) {
		RefreshPorts(); 
		event.Skip();	// 元のクリックの実行
	});

	RefreshPorts();
}

void MainFrame::RefreshPorts() {
	m_port_choice->Clear();
	for (auto& p : SerialUtils::GetSerialPorts())
		m_port_choice->Append(p.port + ": " + p.description);
	if (m_port_choice->GetCount() > 0)
		m_port_choice->SetSelection(0);
}