#pragma once
#include <wx/wx.h>
#include "DrawPanel.h"
#include "SerialReader.h"
#include "ControllerState.h"

class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title);

private:
	void RefreshPorts();
	wxChoice* m_port_choice;
	wxChoice* m_gamepad_choice;
	DrawPanel* m_draw_panel;
	SerialReader m_serial;
	ControllerStateManager m_controller;
};

