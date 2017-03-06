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

#include <boost/bind.hpp>

#include "HTTPServer.h"
#include "utils/StringPiece.h"
#include "utils/Logging.h"

namespace flint {

/**
 * HTTPServer
 */
HTTPServer::HTTPServer(boost::asio::io_service &ioService, int port) :
		IServer(), ioService_(ioService), port_(port) {
}

HTTPServer::~HTTPServer() {
}

void HTTPServer::onStart() {
	LOG_INFO << "HTTPServer start! " << port_;
	server_.init_asio(&ioService_);
	server_.set_reuse_addr(true);

	// handle HTTP request
	server_.set_http_handler(
			boost::bind(&HTTPServer::onHttpRequest, this, ::_1));
	// handle WebSocket connection
	server_.set_open_handler(
			boost::bind(&HTTPServer::onWebsocketConnect, this, ::_1));

	server_.listen(boost::asio::ip::tcp::v4(), port_);
	server_.start_accept();
}

void HTTPServer::onStop() {
	LOG_INFO << "HTTPServer stop! " << port_;
	server_.stop();
}

void HTTPServer::onHttpRequest(websocketpp::connection_hdl hdl) {
	HTTPSession *session = new HTTPSession(&server_, hdl);
	HttpRequest(session);
	delete session;
}

void HTTPServer::onWebsocketConnect(websocketpp::connection_hdl hdl) {
	WebsocketSession *session = new WebsocketSession(&server_, hdl);
	WebsocketConnect(session);
}

/**
 * HTTPSession
 */
HTTPSession::HTTPSession(websocket_server *server,
		websocketpp::connection_hdl hdl) :
		httpServer_(server) {
	conn_ = httpServer_->get_con_from_hdl(hdl);
}

HTTPSession::~HTTPSession() {
}

std::string const & HTTPSession::getSrcAddr() {
	std::string ip = conn_->get_remote_endpoint();
	return ip;
}

std::string const & HTTPSession::getUrl() {
	std::string url = conn_->get_uri()->str();
	return url;
}

/*
 * get request path, eg: http://host:port/path
 */
std::string const & HTTPSession::getResource() {
	return conn_->get_uri()->get_resource();
}

std::string const & HTTPSession::getMethod() {
	return conn_->get_request().get_method();
}

std::string const & HTTPSession::getBody() {
	return conn_->get_request().get_body();
}

std::string const & HTTPSession::getHost() {
	return conn_->get_host();
}

std::string const & HTTPSession::getQuery() {
	return conn_->get_uri()->get_query();
}

std::string const & HTTPSession::getHeader(const std::string &key) {
	return conn_->get_request_header(key);
}

void HTTPSession::setHeader(const std::string &headerKey,
		const std::string &headerValue) {
	conn_->replace_header(headerKey, headerValue);
}

void HTTPSession::setStatus(websocketpp::http::status_code::value code,
		const std::string &msg) {
	conn_->set_status(code, msg);
}

void HTTPSession::setBody(const std::string &body) {
	conn_->set_body(body);
}

/**
 * WebsocketSession
 */
WebsocketSession::WebsocketSession(websocket_server *server,
		websocketpp::connection_hdl hdl) :
		HTTPSession(server, hdl) {
	// set close hander
	conn_->set_close_handler(
			boost::bind(&WebsocketSession::onClose, this, ::_1));
	// set fail hander
	conn_->set_fail_handler(boost::bind(&WebsocketSession::onFail, this, ::_1));
	// set interrupt hander
	conn_->set_interrupt_handler(
			boost::bind(&WebsocketSession::onInterrupt, this, ::_1));
	// set message hander
	conn_->set_message_handler(
			boost::bind(&WebsocketSession::onMessage, this, ::_1, ::_2));
}

WebsocketSession::~WebsocketSession() {
	LOG_INFO << getResource() << " ~~~~ WebsocketSession";
}

void WebsocketSession::send(const StringPiece &message) {
	try {
		LOG_INFO << "WebsocketSession::send:" << message;
		conn_->send(message.data(), message.size(),
				websocketpp::frame::opcode::TEXT);
	} catch (std::exception &e) {
		LOG_ERROR << "WebsocketSession::send exception:" << e.what();
	}
}

void WebsocketSession::close() {
//	Looper::forThread()->runAfter(0,
//			boost::bind(&WebsocketSession::closeInternal, this));
	closeInternal();
}

void WebsocketSession::closeInternal() {
	LOG_INFO << "WebsocketSession closeInternal: " << getResource();
	conn_->close(websocketpp::close::status::normal, "");
}

void WebsocketSession::onMessage(websocketpp::connection_hdl hdl,
		websocketpp::server<websocketpp::config::asio>::message_ptr msg) {
	Message(msg->get_payload());
}

void WebsocketSession::onClose(websocketpp::connection_hdl hdl) {
	LOG_INFO << "WebsocketSession onClose: " << getResource();
	clear();
}

void WebsocketSession::onFail(websocketpp::connection_hdl hdl) {
	LOG_ERROR << "WebsocketSession onFail: " << getResource();
	clear();
}

void WebsocketSession::onInterrupt(websocketpp::connection_hdl hdl) {
	LOG_ERROR << "WebsocketSession onInterrupt: " << getResource();
	clear();
}

void WebsocketSession::clear() {
	Close();
	delete this;
}

} /* namespace flint */
