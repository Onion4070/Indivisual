#pragma once
#include <cstdint>
#include <mutex>
#include <cstring>

struct ControllerState {
	uint8_t raw[256] = {};
	uint8_t length = 0;
};

class ControllerStateManager
{
public:
	void Update(const uint8_t* data, uint8_t len) {
		std::lock_guard<std::mutex> lock(m_mutex);
		memcpy(m_state.raw, data, len);
		m_state.length = len;
	}

	ControllerState Get() {
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_state;
	}

private:
	ControllerState m_state;
	std::mutex m_mutex;
};

