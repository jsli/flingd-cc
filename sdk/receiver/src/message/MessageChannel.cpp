/*
 * MessageChannel.cpp
 *
 *  Created on: 2014-12-21
 *      Author: manson
 */

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include "MessageChannel.h"
#include "utils/Logging.h"

namespace flint {

int MessageChannel::CONNECTING = 0;
int MessageChannel::OPEN = 1;
int MessageChannel::CLOSING = 2;
int MessageChannel::CLOSED = 3;

MessageChannel::MessageChannel(const std::string &name) :
		opened_(false), channelName_(name), url_("") {
	// TODO Auto-generated constructor stub
}

MessageChannel::~MessageChannel() {
	// TODO Auto-generated destructor stub
}

void MessageChannel::open(const std::string & url) {
	open(url, NULL);
}

void MessageChannel::open(const std::string & url,
		boost::asio::io_service *ioService) {
	if (url != "") {
		url_ = url;
	}
	if (url_.size() == 0) {
		LOG_ERROR << "MessageChannel url is null, cannot open!!!";
		return;
	}

	// set up access channels to only log interesting things
	client_.clear_access_channels(websocketpp::log::alevel::all);
	client_.set_access_channels(websocketpp::log::alevel::connect);
	client_.set_access_channels(websocketpp::log::alevel::disconnect);
	client_.set_access_channels(websocketpp::log::alevel::app);

	// Initialize the Asio transport policy
	if (ioService == NULL) {
		client_.init_asio();
	} else {
		client_.init_asio(ioService);
	}

	client_.set_open_handler(bind(&MessageChannel::onOpen, this, ::_1));
	client_.set_close_handler(bind(&MessageChannel::onClose, this, ::_1));
	client_.set_fail_handler(bind(&MessageChannel::onFail, this, ::_1));
	client_.set_message_handler(
			bind(&MessageChannel::onWsMessage, this, ::_1, ::_2));

	websocketpp::lib::error_code ec;
	client::connection_ptr conn = client_.get_connection(url_, ec);

	// Grab a handle for this connection so we can talk to it in a thread
	// safe manor after the event loop starts.
	hdl_ = conn->get_handle();

	client_.connect(conn);

	// Create a detach thread to run the ASIO io_service event loop
//	websocketpp::lib::thread asio_thread = websocketpp::lib::thread(
//			&client::run, &client_);
//	asio_thread.detach();
}

void MessageChannel::close() {
	client_.get_io_service().post(
			boost::bind(&MessageChannel::closeImpl, this));
}

void MessageChannel::closeImpl() {
	client_.close(hdl_, websocketpp::close::status::normal, "NORMAL CLOSE");
}

void MessageChannel::send(const std::string & message) {
	if (message.size() == 0) {
		return;
	}
	client_.get_io_service().post(
			boost::bind(&MessageChannel::sendImpl, this, std::string(message)));
}

void MessageChannel::send(const std::string &message,
		const std::string &senderId) {
	if (message.size() == 0) {
		return;
	}

	rapidjson::StringBuffer s;
	rapidjson::Writer<rapidjson::StringBuffer> writer(s);
	writer.StartObject();
	writer.String("senderId");
	writer.String(senderId.c_str());
	writer.String("data");
	writer.String(message.c_str());
	writer.EndObject();

	send(s.GetString());
}

void MessageChannel::sendImpl(const std::string & message) {
	LOG_INFO << channelName_ << " send: [" << message << "]";
	if (opened_) {
		websocketpp::lib::error_code error_code;
		client_.send(hdl_, message, websocketpp::frame::opcode::text,
				error_code);
	} else {
		LOG_WARN << "MessageChannle is not opened, push message: [" << message
				<< "] to pendding list";
		penddingMessages_.push_back(message);
	}
}

bool MessageChannel::isOpened() {
	return opened_;
}

std::string const & MessageChannel::getChannelName() {
	return channelName_;
}

std::string const & MessageChannel::getUrl() {
	return url_;
}

void MessageChannel::onOpen(websocketpp::connection_hdl) {
	LOG_INFO << channelName_ << " -> " << url_ << "  opened!!!";
	std::vector<std::string>::iterator it = penddingMessages_.begin();
	while (it != penddingMessages_.end()) {
		std::string message = *it;
		LOG_DEBUG << "flush pending message: " << message;
		send(message);
		++it;
	}
	penddingMessages_.clear();
	opened_ = true;
	Open(this);
}

void MessageChannel::onClose(websocketpp::connection_hdl) {
	LOG_INFO << channelName_ << " -> " << url_ << "  closed!!!";
	opened_ = false;
	Close(this);
}

void MessageChannel::onFail(websocketpp::connection_hdl) {
	LOG_INFO << channelName_ << " -> " << url_ << "  failed!!!";
	opened_ = false;
	Failed(this);
}

void MessageChannel::onWsMessage(websocketpp::connection_hdl hdl,
		websocketpp::config::asio_client::message_type::ptr message) {
	std::string payload = message->get_payload();
	LOG_DEBUG << channelName_ << " onmessage: " << payload;
	onMessage(payload);
}

} /* namespace flint */
