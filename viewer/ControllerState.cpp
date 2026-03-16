#include "ControllerState.h"

void ControllerStateManager::Update(const uint8_t* data, uint8_t len) {
	std::lock_guard<std::mutex> lock(m_mutex);

	uint16_t lx, ly, rx, ry;
	uint8_t b0, b1, b2;

	switch (m_gamepad_type.load()) {
	case ControllerType::UNDEFINED:
		break;

	case ControllerType::SWITCH_PRO:
		// 不正パケットを除外
		if (len < 50) return;
		if (data[0] != (uint8_t)ControllerID::SWITCH_PRO) return;

		// 生データ
		memcpy(m_state.raw, data, len);
		m_state.length = len;

		// スティック
		lx = data[6] | ((data[7] & 0x0F) << 8);
		ly = 0x0FFF - ((data[7] >> 4) | (data[8] << 4));
		rx = data[9] | ((data[10] & 0x0F) << 8);
		ry = 0x0FFF - ((data[10] >> 4) | (data[11] << 4));

		m_state.lx = (lx - 2048) / 2048.0f;
		m_state.ly = (ly - 2048) / 2048.0f;
		m_state.rx = (rx - 2048) / 2048.0f;
		m_state.ry = (ry - 2048) / 2048.0f;

		// ボタン
		b0 = data[3];
		b1 = data[4];
		b2 = data[5];

		m_state.btn_y = b0 & 0x01;
		m_state.btn_x = b0 & 0x02;
		m_state.btn_b = b0 & 0x04;
		m_state.btn_a = b0 & 0x08;

		m_state.btn_r  = b0 & 0x40;
		m_state.btn_zr = b0 & 0x80;

		m_state.btn_home    = b1 & 0x10;
		m_state.btn_capture = b1 & 0x20;

		m_state.btn_minus  = b1 & 0x01;
		m_state.btn_plus   = b1 & 0x02;
		m_state.btn_rstick = b1 & 0x04;
		m_state.btn_lstick = b1 & 0x08;

		m_state.dpad_down  = b2 & 0x01;
		m_state.dpad_up    = b2 & 0x02;
		m_state.dpad_right = b2 & 0x04;
		m_state.dpad_left  = b2 & 0x08;

		m_state.btn_l  = b2 & 0x40;
		m_state.btn_zl = b2 & 0x80;

		break;

	case ControllerType::SWITCH2_PRO:
		// 不正パケットを除外
		if (len < 50) return;
		if (data[0] != (uint8_t)ControllerID::SWITCH2_PRO) return;

		// 生データ
		memcpy(m_state.raw, data, len);
		m_state.length = len;

		// スティック
		lx = data[6] | ((data[7] & 0x0F) << 8);
		ly = 0x0FFF - ((data[7] >> 4) | (data[8] << 4));
		rx = data[9] | ((data[10] & 0x0F) << 8);
		ry = 0x0FFF - ((data[10] >> 4) | (data[11] << 4));

		m_state.lx = (lx - 2048) / 2048.0f;
		m_state.ly = (ly - 2048) / 2048.0f;
		m_state.rx = (rx - 2048) / 2048.0f;
		m_state.ry = (ry - 2048) / 2048.0f;

		// ボタン
		b0 = data[3];
		b1 = data[4];
		b2 = data[5];

		m_state.btn_b = b0 & 0x01;
		m_state.btn_a = b0 & 0x02;
		m_state.btn_y = b0 & 0x04;
		m_state.btn_x = b0 & 0x08;

		m_state.btn_r      = b0 & 0x10;
		m_state.btn_zr     = b0 & 0x20;
		m_state.btn_plus   = b0 & 0x40;
		m_state.btn_rstick = b0 & 0x80;

		m_state.dpad_down  = b1 & 0x01;
		m_state.dpad_right = b1 & 0x02;
		m_state.dpad_left  = b1 & 0x04;
		m_state.dpad_up    = b1 & 0x08;

		m_state.btn_l      = b1 & 0x10;
		m_state.btn_zl     = b1 & 0x20;
		m_state.btn_minus  = b1 & 0x40;
		m_state.btn_lstick = b1 & 0x80;

		m_state.btn_home    = b2 & 0x01;
		m_state.btn_capture = b2 & 0x02;
		m_state.btn_gr      = b2 & 0x04;
		m_state.btn_gl      = b2 & 0x08;

		m_state.btn_chat = b2 & 0x10;

		break;

	default:
		break;
	}

}

ControllerState ControllerStateManager::Get() {
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_state;
}

void ControllerStateManager::SetConnected(bool connected) {
	std::lock_guard<std::mutex> lock(m_mutex);
	m_connected = connected;

	// 切断時にstateをクリア
	if (!connected) m_state = {};
}

bool ControllerStateManager::IsConnected() {
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_connected;
}

void ControllerStateManager::SetGamepad(ControllerType type) {
	m_gamepad_type = type;
}