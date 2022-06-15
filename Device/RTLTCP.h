/*
    Copyright(c) 2021-2022 jvde.github@gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "Device.h"

#include <fstream>
#include <iostream>

#ifdef _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>

#else

#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>

#define SOCKET int
#define SOCKADDR struct sockaddr
#define SOCKET_ERROR -1

#define closesocket close

#endif

#ifdef __ANDROID__
#include <netinet/in.h>
#endif

namespace Device {

	class RTLTCP : public Device
	{

		enum class PROTOCOL { NONE, RTLTCP } Protocol = PROTOCOL::RTLTCP;

		// Protocol: RTL_TCP
		int freq_offset = 0;
		bool tuner_AGC = true;
		bool RTL_AGC = false;
		FLOAT32 tuner_Gain = 33.0;
		bool bias_tee = false;

		// Protocol: NONE
		Format format = Format::CF32;

		SOCKET sock = -1;
		int timeout = 2;

		std::string host = "localhost";
		std::string port = "1234";

		struct addrinfo* address;
		// output vector

		static const int TRANSFER_SIZE = 1024;
		static const int BUFFER_SIZE = 16 * 16384;

		bool lost = false;

		int Read(void *data,int length,int timeout);
		std::thread async_thread;
		std::thread run_thread;

		void RunAsync();
		void Run();

		FIFO<char> fifo;

		void setParameterRTLTCP(uint8_t cmd, uint32_t param);
		void applySettings();

	public:

		RTLTCP();
		~RTLTCP();

		void OpenWithFileDescriptor(int) { Open(0); } 
		// Control

		void Close();
		void Play();
		void Stop();

		bool isStreaming() { return Device::isStreaming() && !lost; }
		bool isCallback() { return true; }

		void getDeviceList(std::vector<Description>& DeviceList);

		// Settings
		void Print();
		void Set(std::string option, std::string arg);
	};
}
