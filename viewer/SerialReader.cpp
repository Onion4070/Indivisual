#include "SerialReader.h"
#include <iostream>

bool SerialReader::Open(const std::string& port_name, PacketCallback cb) {
	try {
		m_port.open(port_name);
		m_port.set_option(asio::serial_port_base::baud_rate(921600));
		m_port.set_option(asio::serial_port_base::character_size(8));
		m_port.set_option(asio::serial_port_base::stop_bits(asio::serial_port_base::stop_bits::one));
		m_port.set_option(asio::serial_port_base::parity(asio::serial_port_base::parity::none));
		m_port.set_option(asio::serial_port_base::flow_control(asio::serial_port_base::flow_control::none));
	}
	catch (const std::exception& e) {
		std::cerr << "Failed to open port: " << e.what() << std::endl;
		return false;
	}
	m_callback = std::move(cb);
	m_running = true;
	m_thread = std::thread(&SerialReader::ReceiveLoop, this);
	return true;
}

void SerialReader::Close() {
	m_running = false;
	if (m_port.is_open()) m_port.close();
	if (m_thread.joinable()) m_thread.join();
}

void SerialReader::ReceiveLoop() {
	asio::write(m_port, asio::buffer("s\r\n", 3));
	const uint8_t START = 0xAA, END = 0xBB;
	uint8_t buf[256];
	uint16_t cnt = 0;
	auto freq_timer = std::chrono::steady_clock::now();

	while (m_running) {
		try {
			uint8_t b;
			asio::read(m_port, asio::buffer(&b, 1));
			if (b != START) continue;

			uint8_t len;
			asio::read(m_port, asio::buffer(&len, 1));
			asio::read(m_port, asio::buffer(buf, len));
			asio::read(m_port, asio::buffer(&b, 1));

			if (b != END) continue;

			++cnt;
			auto now = std::chrono::steady_clock::now();
			if (std::chrono::duration_cast<std::chrono::milliseconds>(now - freq_timer).count() >= 1000) {
				m_freq = cnt;
				cnt = 0;
				freq_timer = now;
			}

			if (m_callback) m_callback(buf, len);
		}
		catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
}