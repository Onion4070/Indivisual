#include "ControllerState.h"

void ControllerStateManager::Update(const uint8_t* data, uint8_t len) {
	std::lock_guard<std::mutex> lock(m_mutex);
	memcpy(m_state.raw, data, len);
	m_state.length = len;
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