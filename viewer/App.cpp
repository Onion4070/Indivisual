#include "App.h"
#include "MainFrame.h"
#include <wx/wx.h>
#include "SerialUtils.h"

#define DEBUG
// インスタンス生成
wxIMPLEMENT_APP(App);

bool App::OnInit() {

	// ターミナルを強制表示
#ifdef DEBUG
	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);
	freopen_s(&fp, "CONIN$", "r", stdin);

	std::cout << "Debug mode: Terminal ready." << std::endl;
#endif

	// タイトル設定
	MainFrame* mainFrame = new MainFrame("Indivisual Viewer");

	// フレームサイズ設定
	mainFrame->SetSize(800, 600);

	// フレーム初期位置を中央に
	mainFrame->Center();

	// フレームを表示
	mainFrame->Show(true);

	auto ports = SerialUtils::GetSerialPorts();
	for (const auto& port : ports) {
		std::cout << "Port: " << port.port << ", Description: " << port.description << std::endl;
	}
	return true;
}
