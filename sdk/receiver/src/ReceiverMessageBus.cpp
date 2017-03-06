/*
 * ReceiverMessageBus.cpp
 *
 *  Created on: 2015-1-4
 *      Author: manson
 */

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include "ReceiverMessageBus.h"
#include "utils/Logging.h"

namespace flint {

ReceiverMessageBus::ReceiverMessageBus(const std::string &ns,
		ReceiverMessageChannel *messageChannel) :
		MessageBus(ns), messageChannel_(messageChannel), listener_(NULL) {
	// TODO Auto-generated constructor stub

}

ReceiverMessageBus::~ReceiverMessageBus() {
	// TODO Auto-generated destructor stub
}

void ReceiverMessageBus::send(const std::string &message,
		const std::string &senderId) {
	if (messageChannel_ != NULL) {
		rapidjson::StringBuffer s;
		rapidjson::Writer<rapidjson::StringBuffer> writer(s);
		writer.StartObject();
		writer.String("namespace");
		writer.String(namespace_.c_str());
		writer.String("payload");
		writer.String(message.c_str());
		writer.EndObject();
		messageChannel_->send(s.GetString(), senderId);
	} else {
		LOG_ERROR << "null messageChannel, " << namespace_
				<< " cannot send message: " << message;
	}
}

void ReceiverMessageBus::send(const std::string &message) {
	send(message, "*:*");
}

void ReceiverMessageBus::setListener(ReceiverMessageBus::IListener *listener) {
	listener_ = listener;
}

void ReceiverMessageBus::onMessage(const std::string &message,
		const std::string &senderId) {
	if (listener_ != NULL) {
		listener_->onMessage(this, message, senderId);
	}
}

void ReceiverMessageBus::onSenderConnect(const std::string &senderId) {
	if (listener_ != NULL) {
		listener_->onSenderConnect(senderId);
	}
}

void ReceiverMessageBus::onSenderDisconnect(const std::string &senderId) {
	if (listener_ != NULL) {
		listener_->onSenderDisconnect(senderId);
	}
}

} /* namespace flint */
