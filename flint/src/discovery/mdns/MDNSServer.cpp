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

#include <sstream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <iostream>

#include "boost/bind.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/algorithm/string_regex.hpp"
#include "boost/format.hpp"
#include "boost/tokenizer.hpp"
#include "boost/regex.hpp"
#include "boost/dynamic_bitset.hpp"

#include "udp/UDPServer.h"
#include "utils/Logging.h"
#include "net/NetworkManager.h"
#include "platform/Platform.h"

#include "MDNSServer.h"

#include "Buffer.hpp"
#include "MDNS.h"
#include "Composer.h"

namespace flint {

MDNSServer::MDNSServer(boost::asio::io_service &ioService, int servicePort) :
		UDPServer(ioService, "224.0.0.251", 5353), servicePort_(servicePort), protocol_(
				"_openflint._tcp.local") {
	// TODO Auto-generated constructor stub
}

MDNSServer::MDNSServer(boost::asio::io_service &ioService,
		const std::string protocol, int servicePort) :
		UDPServer(ioService, "224.0.0.251", 5353), servicePort_(servicePort), protocol_(
				"_" + protocol + "._tcp.local") {
	// TODO Auto-generated constructor stub
}

MDNSServer::~MDNSServer() {
	// TODO Auto-generated destructor stub
}

void MDNSServer::parseMessage(const std::string &srcIp, int srcPort,
		const char * message, std::size_t len) {
//	LOG_INFO << "received from -> " << srcIp << ":" << srcPort << "[ " << len
//			<< "]";
	try {
		MDNSIncoming incoming(message, len);
		if (!incoming.isQuery()) {
			return;
		}
		if (!incoming.compareProtocol(protocol_)) {
			return;
		}

		std::string device_name = Platform::getInstance()->getDeviceName();
		std::string model_name = Platform::getInstance()->getModelName();
		std::string ip = NetworkManager::getInstance()->getIpAddr();
		std::map<std::string, uint16> cache;
		vos::Buffer response(DEFAULT_BUF_SIZE);

		Composer::set_header(response, DEFAULT_ID,
				FLAG_RESPONSE | FLAG_AUTHORITATIVE, 1, 1, 0, 3);

		Composer::add_query_ptr(response, protocol_, TYPE_PTR,
				CLASS_UNICAST | CLASS_IN, cache);

		Composer::add_answer_ptr(response, protocol_, TYPE_PTR, CLASS_IN,
				DEFAULT_TTL, device_name + "." + protocol_, cache);

		Composer::add_additional_srv(response, device_name + "." + protocol_,
				TYPE_SRV, CLASS_IN, DEFAULT_TTL, device_name + ip + ".local",
				cache);

		std::vector<std::string> vector;
		vector.push_back("id=" + device_name);
		vector.push_back("ve=02");
		vector.push_back("md=" + model_name);
		vector.push_back("ic=/setup/icon.png");
		vector.push_back("ca=5");
		vector.push_back("fn=" + device_name);
		vector.push_back("st=0");
		Composer::add_additional_txt(response, device_name + "." + protocol_,
				TYPE_TXT, CLASS_IN, DEFAULT_TTL, vector, cache);

		Composer::add_additional_addr(response, device_name + ip + ".local",
				TYPE_ADDRESS, CLASS_IN, DEFAULT_TTL, ip, cache);

//		MDNSIncoming response_test(response._data_ptr, response._index);
//		response_test.dump();

		socket_.async_send_to(
				boost::asio::buffer(response._data_ptr, response._index),
				boost::asio::ip::udp::endpoint(
						boost::asio::ip::address::from_string(srcIp), srcPort),
				boost::bind(&MDNSServer::handle_send, this,
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));

//		std::size_t sendSize = socket_.send_to(
//				boost::asio::buffer(response._data_ptr, response._index),
//				boost::asio::ip::udp::endpoint(
//						boost::asio::ip::address::from_string(srcIp), srcPort));
//		LOG_ERROR << "sendSize ---------------------- > " << sendSize;
	} catch (std::exception &e) {
	}
}

void MDNSServer::handle_send(const boost::system::error_code& error,
		std::size_t bytes_transferred) {
//	LOG_INFO << "sendSize ---------- > " << bytes_transferred;
}

} /* namespace flint */
