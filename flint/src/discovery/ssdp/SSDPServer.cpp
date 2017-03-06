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

#include <iostream>
#include <string>
#include <sstream>
#include <ctime>

#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string_regex.hpp>
#include <boost/format.hpp>
#include <boost/tokenizer.hpp>
#include <boost/regex.hpp>

#include "SSDPServer.h"
#include "utils/Logging.h"
#include "platform/Platform.h"
#include "net/NetworkManager.h"

namespace flint {

SSDPServer::SSDPServer(boost::asio::io_service &ioService) :
		IServer(), timer_(ioService), socket_(ioService) {
	std::string uuid = Platform::getDeviceUUID();
	usn_ = "uuid:" + uuid + "::" + FLINT_SEARCH_TARGET;
}

SSDPServer::~SSDPServer() {
	// TODO Auto-generated destructor stub
}

void SSDPServer::init_socket() {
//	LOG_DEBUG << "SSDPServer init!!!";
	boost::asio::ip::address address = boost::asio::ip::address::from_string(
			"0.0.0.0");
	boost::asio::ip::udp::endpoint endpoint(address, FLINT_SSDP_PORT);

	// open
	socket_.open(endpoint.protocol());

	// reuse address, must set before binding!!!
	boost::asio::ip::udp::socket::reuse_address reuse_option(true);
	socket_.set_option(reuse_option);

	// close quickly
	boost::asio::socket_base::linger linger_option(true, 0);
	socket_.set_option(linger_option);

	try {
		// bind
		socket_.bind(endpoint);
	} catch (boost::system::system_error &e) {
		LOG_ERROR << "SSDPServer bind: " << e.what();
	}

	// join the multicast group.
	boost::asio::ip::address multicastAddress =
			boost::asio::ip::address::from_string(FLINT_SSDP_ADDRESS);
	try {
		std::string ip = NetworkManager::getInstance()->getIpAddr();
		if (boost::iequals(ip, "")) {
//			LOG_INFO << "join group: " << multicastAddress.to_string();
			socket_.set_option(
					boost::asio::ip::multicast::join_group(multicastAddress));
		} else {
//			LOG_INFO << "join group: " << multicastAddress.to_string() << " | "
//					<< ip;
			socket_.set_option(
					boost::asio::ip::multicast::join_group(
							multicastAddress.to_v4(),
							boost::asio::ip::address::from_string(ip).to_v4()));
		}
	} catch (boost::system::system_error &e) {
		LOG_ERROR << "SSDPServer join group: " << e.what();
	}

	advertise_endpoint_ = boost::asio::ip::udp::endpoint(multicastAddress,
			FLINT_SSDP_PORT);
//	LOG_DEBUG << "SSDPServer init!!!- end";
}

void SSDPServer::onStart() {
	LOG_INFO << "SSDPServer start!!!";
	init_socket();

	advertise();
	wait_packet();
}

void SSDPServer::onStop() {
	socket_.close();
	timer_.cancel();
}

/**
 * receive methods
 */
void SSDPServer::wait_packet() {
//	LOG_DEBUG << "SSDPServer wait packet!!!";
	socket_.async_receive_from(boost::asio::buffer(recv_buffer_),
			client_endpoint_,
			boost::bind(&SSDPServer::handle_packet, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
//	LOG_DEBUG << "SSDPServer wait packet!!! end";
}

void SSDPServer::handle_packet(const boost::system::error_code& error,
		std::size_t len) {
//	LOG_DEBUG << "SSDPServer handle packet!!!";
	if (!error || error == boost::asio::error::message_size) {
		if (len > 0) {
			std::string message;
			message.assign(recv_buffer_.data(), len);
			parseMessage(message);
		}
		wait_packet();
	}
//	LOG_DEBUG << "SSDPServer handle packet!!! end";
}

void SSDPServer::parseMessage(const std::string &message) {
	if (boost::starts_with(message, "M-SEARCH")) {
		onSearch(message);
	} else if (boost::starts_with(message, "NOTIFY")) {
		// ignore
	} else {
		LOG_INFO << "unknow message: " << message;
	}
}

void SSDPServer::onSearch(const std::string &message) {
//	LOG_INFO << "onSearch:\n" << message;
	std::vector<std::string> messageVector;
	std::map<std::string, std::string> vars;

	boost::algorithm::split_regex(messageVector, message, boost::regex("\r\n"));
	size_t v_size = messageVector.size();
	if (v_size > 0) {
		//i = 1, ignore command line, such as M-SEARCH
		for (size_t i = 1; i < v_size; i++) {
			if (messageVector[i].size() > 0) {
				std::string _tmp = boost::trim_copy_if(messageVector[i],
						boost::is_space()); // trim space
				std::vector<std::string> _v_tmp;
				boost::algorithm::split_regex(_v_tmp, _tmp, boost::regex(":")); // string maybe contains >= 2 ":"
				if (_v_tmp.size() >= 2) {
					std::string key = _v_tmp[0];
					_v_tmp.erase(_v_tmp.begin());
					std::string value = boost::trim_copy_if(
							boost::join(_v_tmp, ":"), boost::is_any_of(" \"")); //trim " and space
					vars.insert(
							std::pair<std::string, std::string>(key, value));
				}
			}
		}
	}

	if (vars.size() > 0) {
		if (vars.find("MAN") != vars.end() && vars.find("MX") != vars.end()
				&& vars.find("ST") != vars.end()) {
			std::map<std::string, std::string>::iterator _itMan = vars.find(
					"MAN");
			std::map<std::string, std::string>::iterator _itSt = vars.find(
					"ST");
//			LOG_DEBUG << "man = " << _itMan->second << ", st = " << _itSt->second;
			if (("ssdp:all" == _itMan->second)
					|| ("ssdp:discover" == _itMan->second
							&& FLINT_SEARCH_TARGET == _itSt->second)) {
//				LOG_INFO << "handle M-SEARCH";
//				LOG_INFO << "send to: "
//						<< client_endpoint_.address().to_string() << ":"
//						<< client_endpoint_.port();
				std::string ip = NetworkManager::getInstance()->getIpAddr();
				if (!boost::iequals(ip, "")) {
					std::string msg = join_message(ip);
//					socket_.send_to(boost::asio::buffer(msg), client_endpoint_);
					socket_.async_send_to(boost::asio::buffer(msg),
							client_endpoint_,
							boost::bind(&SSDPServer::handle_send, this,
									boost::asio::placeholders::error,
									boost::asio::placeholders::bytes_transferred));
				}
			}
		}
	}
}

void SSDPServer::handle_send(const boost::system::error_code& error,
		std::size_t bytes_transferred) {
//	LOG_INFO << "send ---------- > " << bytes_transferred;
}

/**
 * advertise
 */
void SSDPServer::advertise() {
//	LOG_DEBUG << "SSDPServer advertise!!";
	timer_.expires_from_now(boost::posix_time::seconds(3));
	timer_.async_wait(
			boost::bind(&SSDPServer::handle_advertise, this,
					boost::asio::placeholders::error));
//	LOG_DEBUG << "SSDPServer advertise!! - end";
}

void SSDPServer::handle_advertise(const boost::system::error_code& error) {
//	LOG_DEBUG << "SSDPServer handle advertise!!";
	if (!error) {
		std::string ip = NetworkManager::getInstance()->getIpAddr();
		if (!boost::iequals(ip, "")) {
			std::string msg = join_message(ip, true);
			//		LOG_DEBUG << "advertise:\n" << msg;
			socket_.async_send_to(boost::asio::buffer(msg), advertise_endpoint_,
					boost::bind(&SSDPServer::handle_advertise_send, this,
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred));
		}

		advertise();
	}
}

void SSDPServer::handle_advertise_send(const boost::system::error_code& error,
		std::size_t bytes_transferred) {
//	LOG_INFO << "advertise ---------- > " << bytes_transferred;
}

std::string SSDPServer::join_message(const std::string &ip, bool advertise) {
	std::stringstream ss;
	if (advertise) {
		ss << "NOTIFY * HTTP/1.1\r\n";
		ss << "HOST: " << FLINT_SSDP_ADDRESS << ":" << FLINT_SSDP_PORT
				<< "\r\n";
		ss << "NT: " << FLINT_SEARCH_TARGET << "\r\n";
		ss << "NTS: ssdp:alive\r\n";

	} else {
		ss << "HTTP/1.1 200 OK\r\n";
		ss << "ST: " << FLINT_SEARCH_TARGET << "\r\n";

	}
	ss << "USN: " << usn_ << "\r\n";
	ss << "LOCATION: " << join_location(ip) << "\r\n";
	ss << "CACHE-CONTROL: max-age=" << FLINT_SSDP_TTL << "\r\n";
	ss << "SERVER: "
			<< "Linux/3.8.13+, UPnP/1.0, Portable SDK for UPnP devices/1.6.18"
			<< "\r\n";
	ss << "DATE: " + get_date() << "\r\n";
	ss << "BOOTID.UPNP.ORG: 9\r\n";
	ss << "CONFIGID.UPNP.ORG: 1\r\n";
	ss << "OPT: \"http://schemas.upnp.org/upnp/1/0/\"; ns=01\r\n";
	ss << "X-USER-AGENT: redsonic\r\n";
	ss << "EXT:\r\n";
	ss << "\r\n";
	return ss.str();
}

std::string SSDPServer::get_date() {
	time_t t = time(NULL);
	char buf[128] = { 0 };
	strftime(buf, 127, "%a, %d %b %Y %H:%M:%S GMT", gmtime(&t));
	return std::string(buf);
}

std::string SSDPServer::join_location(const std::string &ip) {
	std::stringstream ss;
	ss << "http://" << ip << ":" << FLINT_HTTP_PORT << "/"
			<< FLINT_HTTP_DESCRIPTOR;
	return ss.str();
}

} /* namespace flint */
