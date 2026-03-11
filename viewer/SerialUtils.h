#pragma once
#include <string>
#include <vector>

struct SerialPortInfo {
	std::string port;			// "COM3"
	std::string description;	// "USB Serial Device (COM3)"
};

class SerialUtils
{
public:
	static std::vector<SerialPortInfo> GetSerialPorts();
};