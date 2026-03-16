#include "App.h"
#include "MainFrame.h"
#include <wx/wx.h>
#include "SerialUtils.h"

#ifdef _DEBUG
	// プロジェクト->プロパティ->リンカー->システム->サブシステム をコンソールに設定する必要あり
	wxIMPLEMENT_APP_CONSOLE(App);
#else
	wxIMPLEMENT_APP(App);
#endif

bool App::OnInit() {
	// タイトル設定
	MainFrame* mainFrame = new MainFrame("Indivisual");

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
