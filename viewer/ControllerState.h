#pragma once
#include <cstdint>
#include <mutex>
#include <cstring>

struct ControllerState {
	// スティック
	float lx = 0.0f, ly = 0.0f;
	float rx = 0.0f, ry = 0.0f;

	// ボタン
	bool btn_a = false;
	bool btn_b = false;
	bool btn_x = false;
	bool btn_y = false;
	bool btn_l = false;
	bool btn_r = false;
	bool btn_zl = false;
	bool btn_zr = false;
	bool btn_plus = false;
	bool btn_minus = false;
	bool btn_home = false;
	bool btn_capture = false;
	bool btn_lstick = false;
	bool btn_rstick = false;
	bool btn_chat = false;

	// 背面ボタン
	bool btn_gl = false;
	bool btn_gr = false;

	// 十字キー
	bool dpad_up = false;
	bool dpad_down = false;
	bool dpad_left = false;
	bool dpad_right = false;

	// 生データ(デバッグ用)
	uint8_t raw[256] = {};
	uint8_t length = 0;
};

enum CONTROLLER_TYPE {
	UNDEFINED,
	SWITCH_PRO,
	SWITCH2_PRO,
	JOYCON2,
};

class ControllerStateManager
{
public:
	void Update(const uint8_t* data, uint8_t len);
	ControllerState Get();
	void SetConnected(bool connected);
	bool IsConnected();
	void SetGamepad(CONTROLLER_TYPE);

private:
	ControllerState m_state;
	std::mutex m_mutex;
	bool m_connected = false;
	std::atomic<CONTROLLER_TYPE> m_gamepad_type{ UNDEFINED };
};

