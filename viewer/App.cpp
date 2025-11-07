#include "App.h"
#include "MainFrame.h"
#include <wx/wx.h>

// インスタンス生成
wxIMPLEMENT_APP(App);

bool App::OnInit() {
	// タイトル設定
	MainFrame* mainFrame = new MainFrame("title: C++ GUI");

	// フレームサイズ設定
	mainFrame->SetSize(800, 600);

	// フレーム初期位置を中央に
	mainFrame->Center();

	// フレームを表示
	mainFrame->Show(true);
	return true;
}
