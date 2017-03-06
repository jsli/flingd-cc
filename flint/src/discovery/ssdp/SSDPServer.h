/*
 * Copyright (C) 2013-2014, The OpenFlint Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS-IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SSDPSERVER_H_
#define SSDPSERVER_H_

#include <string>

#include <boost/asio.hpp>
#include <boost/array.hpp>

#include "comm/IServer.h"

using boost::asio::ip::udp;
using boost::asio::ip::tcp;

namespace flint {

const std::string FLINT_SEARCH_TARGET =
		"urn:dial-multiscreen-org:service:dial:1";
const std::string FLINT_HTTP_DESCRIPTOR = "ssdp/device-desc.xml";
const unsigned int FLINT_HTTP_PORT = 9431;
const std::string FLINT_SSDP_ADDRESS = "239.255.255.250";
const unsigned int FLINT_SSDP_PORT = 1900;
const unsigned int FLINT_SSDP_TTL = 1800;

class SSDPServer: public IServer {

public:
	SSDPServer(boost::asio::io_service &ioService);
	virtual ~SSDPServer();

protected:
	virtual void onStart();

	virtual void onStop();

private:
	void init_socket();
	void start_internal();

	void wait_packet();
	void handle_packet(const boost::system::error_code& /*error*/,
			std::size_t /*bytes_transferred*/);
	void parseMessage(const std::string &message);
	void onSearch(const std::string &message);
	void handle_send(const boost::system::error_code& /*error*/,
			std::size_t /*bytes_transferred*/);

	void advertise();
	void handle_advertise(const boost::system::error_code& /*error*/);
	void handle_advertise_send(const boost::system::error_code& /*error*/,
			std::size_t /*bytes_transferred*/);

	std::string join_message(const std::string &ip, bool advertise = false);
	std::string join_location(const std::string &ip);
	std::string get_date();
	std::string get_ip();

private:
	boost::asio::deadline_timer timer_;
	std::string usn_;

	udp::socket socket_;
	udp::endpoint client_endpoint_;
	boost::array<char, 4096> recv_buffer_;

	udp::endpoint advertise_endpoint_;
};

} /* namespace flint */
#endif /* SSDPSERVER_H_ */
