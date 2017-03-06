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

#include <boost/algorithm/string.hpp>
#include <boost/xpressive/xpressive_dynamic.hpp>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include "WsServer.h"
#include "utils/Logging.h"

namespace flint {

WsServer::WsServer(boost::asio::io_service &ioService, int httpPort) :
		IServer(), httpServer_(ioService, httpPort), allow_methods_(
				"GET, POST, DELETE, OPTIONS"), allow_origin_("*"), allow_headers_(
				"Content-Type, Authorization, Accept*, X-Requested-With") {
	// TODO Auto-generated constructor stub
}

WsServer::~WsServer() {
	// TODO Auto-generated destructor stub
}

void WsServer::onStart() {
	httpServer_.HttpRequest.connect(
			boost::bind(&WsServer::onHttpRequest, this, ::_1));
	httpServer_.WebsocketConnect.connect(
			boost::bind(&WsServer::onWebsocketConnection, this, ::_1));
	httpServer_.start();
}

void WsServer::onStop() {
	LOG_INFO << "WsServer stop!";
	httpServer_.stop();
}

void WsServer::onHttpRequest(HTTPSession *httpSession) {
}

void WsServer::onWebsocketConnection(WebsocketSession *websocketSession) {
}

void WsServer::response(HTTPSession *httpSession,
		websocketpp::http::status_code::value statusCode) {
	httpSession->setStatus(statusCode);

	httpSession->setHeader("Access-Control-Allow-Methods", allow_methods_);
	httpSession->setHeader("Access-Control-Allow-Origin", allow_origin_);
	httpSession->setHeader("Access-Control-Allow-Headers", allow_headers_);
}

void WsServer::response(HTTPSession *httpSession,
		websocketpp::http::status_code::value statusCode,
		std::map<std::string, std::string> headers, const std::string &body) {
	httpSession->setStatus(statusCode);

	httpSession->setHeader("Access-Control-Allow-Methods", allow_methods_);
	httpSession->setHeader("Access-Control-Allow-Origin", allow_origin_);
	httpSession->setHeader("Access-Control-Allow-Headers", allow_headers_);

	std::map<std::string, std::string>::iterator it;
	for (it = headers.begin(); it != headers.end(); ++it) {
		httpSession->setHeader(it->first, it->second);
	}

	httpSession->setBody(body);
}

} /* namespace flint */
