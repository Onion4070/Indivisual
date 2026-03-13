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
	void Update(const uint8_t* data, uint8_t len);
	ControllerState Get();
	void SetConnected(bool connected);
	bool IsConnected();

private:
	ControllerState m_state;
	std::mutex m_mutex;
	bool m_connected = false;
};

