/*
 * MessageBusManager.cpp
 *
 *  Created on: 2014-12-21
 *      Author: manson
 */

#include <cstdio>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include "MessageBusManager.h"
#include "utils/Logging.h"

namespace flint {

MessageBusManager::MessageBusManager(boost::asio::io_service *ioService,
		const std::string &appId, const std::string &url) :
		isOpened_(false), ioService_(ioService), appId_(appId), url_(url) {
	// TODO Auto-generated constructor stub
	messageChannel_ = new flint::ReceiverMessageChannel("channelBaseUrl");
	messageChannel_->Open.connect(
			boost::bind(&MessageBusManager::onOpen, this, ::_1));
	messageChannel_->Close.connect(
			boost::bind(&MessageBusManager::onClose, this, ::_1));
	messageChannel_->Failed.connect(
			boost::bind(&MessageBusManager::onFailed, this, ::_1));
	messageChannel_->ReceiverMessage.connect(
			boost::bind(&MessageBusManager::onMessage, this, ::_1, ::_2, ::_3,
					::_4));
}

MessageBusManager::~MessageBusManager() {
	// TODO Auto-generated destructor stub
}

void MessageBusManager::open() {
	if (appId_.size() == 0) {
		LOG_ERROR << "appId cannot be null!!!";
		return;
	}

	if (messageChannel_ != NULL) {
		messageChannel_->open(url_, ioService_);
	} else {
		LOG_ERROR << "MessageChannel is NULL!!! cannot open!!!";
	}
}

void MessageBusManager::close() {
	if (messageChannel_ != NULL) {
		messageChannel_->close();
	}
	clear();
}

void MessageBusManager::onOpen(MessageChannel *channel) {
	LOG_INFO << "MessageBusManager: opened!!!";
	isOpened_ = true;
}

void MessageBusManager::onClose(MessageChannel *channel) {
	LOG_WARN << "MessageBusManager: closed!!!";
	clear();
}

void MessageBusManager::onFailed(MessageChannel *channel) {
	LOG_ERROR << "MessageBusManager: failed!!!";
	clear();
}

void MessageBusManager::onMessage(ReceiverMessageChannel *channel,
		const std::string &type, const std::string &senderId,
		const std::string &message) {
	LOG_DEBUG << "MessageBusManager onMessage: " << message;
	if (type == "senderConnected") {
		std::map<std::string, ReceiverMessageBus *>::iterator it =
				messageBusMap_.begin();
		while (it != messageBusMap_.end()) {
			ReceiverMessageBus *bus = it->second;
			bus->onSenderConnect(senderId);
			++it;
		}
	} else if (type == "senderDisconnected") {
		std::map<std::string, ReceiverMessageBus *>::iterator it =
				messageBusMap_.begin();
		while (it != messageBusMap_.end()) {
			ReceiverMessageBus *bus = it->second;
			bus->onSenderDisconnect(senderId);
			++it;
		}
	} else if (type == "message") {
		rapidjson::Document d;
		d.Parse(message.c_str());
		if (d.IsNull()) {
			return;
		}

		// check message's item
		rapidjson::Value& ns = d["namespace"];
		if (ns.IsNull()) {
			return;
		}
		rapidjson::Value& payload = d["payload"];
		if (payload.IsNull()) {
			return;
		}

		std::string nsStr = ns.GetString();
		std::string payloadStr = payload.GetString();

		std::map<std::string, ReceiverMessageBus *>::iterator it =
				messageBusMap_.begin();
		while (it != messageBusMap_.end()) {
			ReceiverMessageBus *bus = it->second;
			if (bus->getNamesapce() == nsStr) {
				bus->onMessage(payloadStr, senderId);
			}
			++it;
		}
	} else {
		LOG_WARN << "unknow message: " << type;
	}
}

ReceiverMessageBus * MessageBusManager::createMessageBus(
		const std::string &ns) {
	if (ns.size() == 0) {
		return NULL;
	}

	std::map<std::string, ReceiverMessageBus *>::iterator it =
			messageBusMap_.find(ns);
	if (it == messageBusMap_.end()) {
		ReceiverMessageBus *bus = new ReceiverMessageBus(ns, messageChannel_);
		messageBusMap_.insert(
				std::pair<std::string, ReceiverMessageBus *>(ns, bus));
		return bus;
	} else {
		return it->second;
	}
}

void MessageBusManager::clear() {
	isOpened_ = false;
	if (messageChannel_ != NULL) {
		delete messageChannel_;
		messageChannel_ = NULL;
	}

	std::map<std::string, ReceiverMessageBus *>::iterator it =
			messageBusMap_.begin();
	while (it != messageBusMap_.end()) {
		ReceiverMessageBus *bus = it->second;
		delete bus;
		++it;
	}
	messageBusMap_.clear();
}

} /* namespace flint */
