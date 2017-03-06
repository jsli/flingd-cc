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

#ifndef __HTTP_SERVER_H_
#define __HTTP_SERVER_H_

#include <boost/signals2.hpp>
#include <boost/asio.hpp>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include "core/Looper.h"
#include "utils/StringPiece.h"
#include "http/HTTPServer.h"
#include "comm/IServer.h"

namespace flint {

typedef websocketpp::server<websocketpp::config::asio> websocket_server;

class HTTPSession {
public:
	HTTPSession(websocket_server *server, websocketpp::connection_hdl hdl);
	virtual ~HTTPSession();

	std::string const & getSrcAddr();
	std::string const & getUrl();
	std::string const & getResource();
	std::string const & getMethod();
	std::string const & getBody();
	std::string const & getHost();
	std::string const & getQuery();
	std::string const & getHeader(const std::string &key);

	void setHeader(const std::string &headerKey,
			const std::string &headerValue);
	void setStatus(websocketpp::http::status_code::value code,
			const std::string &msg = "");
	void setBody(const std::string &body);

protected:
	websocket_server *httpServer_;
	websocket_server::connection_ptr conn_;
};

class WebsocketSession: public HTTPSession {
public:
	WebsocketSession(websocket_server *server, websocketpp::connection_hdl hdl);
	virtual ~WebsocketSession();

	void send(const StringPiece &message);
	void close();

public:
	boost::signals2::signal<void()> Close;
	boost::signals2::signal<void(const std::string &)> Message;

private:
	void onMessage(websocketpp::connection_hdl hdl,
			websocketpp::server<websocketpp::config::asio>::message_ptr msg);
	void onClose(websocketpp::connection_hdl hdl);
	void onFail(websocketpp::connection_hdl hdl);
	void onInterrupt(websocketpp::connection_hdl hdl);

	void closeInternal();
	void clear();
};

class HTTPServer: public IServer {
public:
	HTTPServer(boost::asio::io_service &ioService, int port);
	virtual ~HTTPServer();

public:
	boost::signals2::signal<void(HTTPSession *)> HttpRequest;
	boost::signals2::signal<void(WebsocketSession *)> WebsocketConnect;

protected:
	virtual void onStart();

	virtual void onStop();

protected:
	void onHttpRequest(websocketpp::connection_hdl hdl);
	virtual void onWebsocketConnect(websocketpp::connection_hdl hdl);

protected:
	boost::asio::io_service &ioService_;
	websocket_server server_;
	int port_;
};

}

#endif //__HTTP_SERVER_H_
