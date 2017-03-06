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

#ifndef MDNSSERVER_H_
#define MDNSSERVER_H_

#include <string>

#include "boost/array.hpp"

#include "udp/UDPServer.h"
#include "MDNSDef.h"

namespace flint {

class MDNSServer: public UDPServer {
public:
	MDNSServer(boost::asio::io_service &ioService, int servicePort);
	MDNSServer(boost::asio::io_service &ioService, const std::string protocol,
			int servicePort);
	virtual ~MDNSServer();

protected:
	void parseMessage(const std::string &srcIp, int srcPort,
			const char * message, std::size_t len);

	void handle_send(const boost::system::error_code& error,
			std::size_t bytes_transferred);

protected:
	int servicePort_;
	std::string protocol_;
	boost::array<uint8, 512> array_;
};

} /* namespace flint */
#endif /* MDNSSERVER_H_ */
