#pragma once
#include <asio.hpp>
#include <thread>
#include <atomic>
#include <functional>
#include <cstdint>

class SerialReader
{
public:
	using PacketCallback = std::function<void(const uint8_t*, uint8_t)>;

	SerialReader() : m_io(), m_port(m_io) {}
	~SerialReader() { Close(); }

	bool Open(const std::string& port_name, PacketCallback cb);
	void Close();
	bool IsOpen()  const { return m_port.is_open(); }
	int  GetFreq() const { return m_freq.load(); }

private:
	void ReceiveLoop();

	asio::io_context m_io;
	asio::serial_port m_port;
	std::thread m_thread;
	std::atomic<bool> m_running{ false };
	std::atomic<int> m_freq;
	PacketCallback m_callback;
};

