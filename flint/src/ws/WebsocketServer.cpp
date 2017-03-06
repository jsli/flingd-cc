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

#include "WebsocketServer.h"
#include "utils/Logging.h"

namespace flint {

// SenderChannel //
SenderChannel::SenderChannel(WebsocketSession *session,
		const std::string &senderId, ReceiverChannel *channel) :
		WebsocketHandler(session), senderId_(senderId), receiverChannel_(
				channel) {
}

SenderChannel::~SenderChannel() {
	LOG_INFO << senderId_ << " ~~~~ SenderChannel";
}

const std::string & SenderChannel::getSenderId() {
	return senderId_;
}

void SenderChannel::onMessage(const std::string &msg) {
	//TODO: sender message to ReceiverChannel
	LOG_INFO << senderId_ << " -> SenderChannel onMessage: " << msg;
	rapidjson::StringBuffer s;
	rapidjson::Writer<rapidjson::StringBuffer> writer(s);
	writer.StartObject();
	writer.String("type");
	writer.String("message");
	writer.String("senderId");
	writer.String(senderId_.c_str());
	writer.String("data");
	writer.String(msg.c_str());
	writer.EndObject();
	std::string _msg = s.GetString();
	receiverChannel_->send(_msg);
}

void SenderChannel::close() {
	receiverChannel_ = NULL;
	WebsocketHandler::close();
}

void SenderChannel::onClose() {
	LOG_INFO << "SenderChannel onClose(): senderId : " << senderId_;

	// romove self in ReceiverChannel
	if (receiverChannel_ != NULL) {
		receiverChannel_->removeSenderChannel(senderId_);
	}

	// super method: delete self
	WebsocketHandler::onClose();
}

// ReceiverChannel //

ReceiverChannel::ReceiverChannel(WebsocketSession *session,
		const std::string &channelId) :
		WebsocketHandler(session), channelId_(channelId) {
}

ReceiverChannel::~ReceiverChannel() {
	LOG_INFO << channelId_ << " ~~~~ ReceiverChannel";
}

const std::string & ReceiverChannel::getChannelId() {
	return channelId_;
}

void ReceiverChannel::onMessage(const std::string &msg) {
	LOG_INFO << channelId_ << " -> ReceiverChannel onMessage: " << msg;
	rapidjson::Document d;
	d.Parse(msg.c_str());
	rapidjson::Value& senderId = d["senderId"];
	std::string senderIdStr = senderId.GetString();
	rapidjson::Value& data = d["data"];
	if ("*:*" == senderIdStr) {
		std::map<std::string, SenderChannel*>::iterator it = senders_.begin();
		while (it != senders_.end()) {
			it->second->send(data.GetString());
			++it;
		}
	} else {
		SenderChannel *senderChannel = getSenderChannel(senderIdStr);
		if (senderChannel != NULL) {
			senderChannel->send(data.GetString());
		}
	}
}

void ReceiverChannel::onClose() {
	LOG_WARN << "ReceiverChannel onClose(): channelId : " << channelId_;
	std::map<std::string, SenderChannel*>::iterator it = senders_.begin();

	// delete all senders
	while (it != senders_.end()) {
		SenderChannel* senderChannel = it->second;
		senderChannel->close();
		++it;
	}
	senders_.clear();

	// notify WebsocketServer that a ReceiverChannel closed
	Close(channelId_);

	// super method: delete self
	WebsocketHandler::onClose();
}

bool ReceiverChannel::addSenderChannel(const std::string &senderId,
		WebsocketSession *senderSession) {
	if (getSenderChannel(senderId) == NULL) {
		// insert a new SenderChannel
		SenderChannel *senderChannel = new SenderChannel(senderSession,
				senderId, this);
		senders_.insert(
				std::pair<std::string, SenderChannel*>(senderId,
						senderChannel));
		LOG_INFO << "add sender channel: " << senderId << " to receiver:"
				<< channelId_ << ", senders size = " << senders_.size();

		// notify receiver application
		rapidjson::StringBuffer s;
		rapidjson::Writer<rapidjson::StringBuffer> writer(s);
		writer.StartObject();
		writer.String("type");
		writer.String("senderConnected");
		writer.String("senderId");
		writer.String(senderId.c_str());
		writer.EndObject();
		send(s.GetString());
		return true;
	} else {
		LOG_WARN << "in receiver " << channelId_
				<< ", duplicate sender channel: " << senderId << ", close!";
		senderSession->close();
		return false;
	}
}

bool ReceiverChannel::removeSenderChannel(const std::string &senderId) {
	std::map<std::string, SenderChannel*>::iterator it = senders_.begin();
	while (it != senders_.end()) {
		if (senderId == it->first) {
			rapidjson::StringBuffer s;
			rapidjson::Writer<rapidjson::StringBuffer> writer(s);
			writer.StartObject();
			writer.String("type");
			writer.String("senderDisconnected");
			writer.String("senderId");
			writer.String(senderId.c_str());
			writer.EndObject();
			send(s.GetString());
			senders_.erase(it);
			LOG_INFO << "remove sender channel: " << senderId
					<< " frome receiver:" << channelId_ << ", senders size = "
					<< senders_.size();
			return true;
		}
		++it;
	}
	return false;
}

SenderChannel * ReceiverChannel::getSenderChannel(const std::string &senderId) {
	std::map<std::string, SenderChannel*>::iterator it = senders_.begin();
	while (it != senders_.end()) {
		if (senderId == it->first) {
			return it->second;
		}
		++it;
	}
	return NULL;
}

// WebsocketServer //

WebsocketServer::WebsocketServer(boost::asio::io_service &ioService,
		int httpPort) :
		WsServer(ioService, httpPort), httpServer_(ioService, httpPort) {
	// TODO Auto-generated constructor stub
}

WebsocketServer::~WebsocketServer() {
	// TODO Auto-generated destructor stub
}

void WebsocketServer::onStart() {
	LOG_INFO << "WebsocketServer start!";
	WsServer::onStart();
}

void WebsocketServer::onStop() {
	LOG_INFO << "WebsocketServer stop!";
	WsServer::onStop();
}

void WebsocketServer::onHttpRequest(HTTPSession *httpSession) {
	// ignore
}

void WebsocketServer::onWebsocketConnection(
		WebsocketSession *websocketSession) {
	std::string path = websocketSession->getResource();
	LOG_INFO << "WebsocketServer HANDLE WEBSOCKET [" << path << "]";

	if (boost::starts_with(path, "/channels/")) {
		// receiver connection regex
		boost::xpressive::cregex receiverRegex =
				boost::xpressive::cregex::compile(
						"\\/(channels)\\/([a-zA-Z_0-9\\-]+)",
						boost::xpressive::icase);
		// sender connection regex
		boost::xpressive::cregex senderRegex =
				boost::xpressive::cregex::compile(
						"\\/(channels)\\/([a-zA-Z_0-9\\-]+)\\/(senders)\\/([a-zA-Z_0-9\\-]+)",
						boost::xpressive::icase);
		boost::xpressive::cmatch segs;

		if (boost::xpressive::regex_search(path.c_str(), segs, senderRegex)) { // check sender connection
			if (segs.size() == 5) {
				std::string channelId = segs[2];
				std::string senderId = segs[4];
				LOG_DEBUG << "new sender connection: " << "channelId="
						<< channelId << ", senderId=" << senderId;
				onSenderConnected(channelId, senderId, websocketSession);
			} else {
				LOG_ERROR << "parse " << path << " error";
			}
		} else if (boost::xpressive::regex_search(path.c_str(), segs,
				receiverRegex)) { // check receiver connection
			if (segs.size() == 3) {
				std::string channelId = segs[2];
				LOG_DEBUG << "new receiver connection: " << "channelId="
						<< channelId;
				onReceiverConnected(channelId, websocketSession);
			} else {
				LOG_ERROR << "parse " << path << " error";
			}
		} else {
			LOG_ERROR << "WebsocketServer HADNLE WEBSOCKET error";
		}
	} else {
		LOG_ERROR << "WebsocketServer CANNOT HANDLE WEBSOCKET [" << path << "]";
	}
}

void WebsocketServer::onSenderConnected(const std::string &channelId,
		const std::string &senderId, WebsocketSession *senderSession) {
	ReceiverChannel *channel = getReceiverChannel(channelId);
	if (channel != NULL) {
		channel->addSenderChannel(senderId, senderSession);
	} else {
		LOG_WARN << "no receiver " << channelId << ", drop sender channel: "
				<< senderId << ", close!";
		senderSession->close();
	}
}

void WebsocketServer::onReceiverConnected(const std::string &channelId,
		WebsocketSession *session) {
	addReceiverChannel(channelId, session);
}

bool WebsocketServer::addReceiverChannel(const std::string &channelId,
		WebsocketSession *receiverSession) {
	if (getReceiverChannel(channelId) == NULL) {
		ReceiverChannel *channel = new ReceiverChannel(receiverSession,
				channelId);
		channel->Close.connect(
				boost::bind(&WebsocketServer::removeReceiverChannel, this,
						::_1));
		receivers_.insert(
				std::pair<std::string, ReceiverChannel*>(channelId, channel));
		LOG_INFO << "add receiver channel: " << channelId
				<< ", receiver size = " << receivers_.size();
		return true;
	} else {
		LOG_WARN << "duplicate receiver channel: " << channelId << ", close!";
		receiverSession->close();
		return false;
	}
}

bool WebsocketServer::removeReceiverChannel(const std::string &channelId) {
	std::map<std::string, ReceiverChannel*>::iterator it = receivers_.find(
			channelId);
	if (it != receivers_.end()) {
		if (channelId == it->first) {
			receivers_.erase(it);
			LOG_INFO << "remove receiver channel: " << channelId
					<< ", receiver size = " << receivers_.size();
			return true;
		}
		++it;
	}
	return false;
}

ReceiverChannel * WebsocketServer::getReceiverChannel(
		const std::string &channelId) {
	std::map<std::string, ReceiverChannel*>::iterator it = receivers_.find(
			channelId);
	if (it != receivers_.end()) {
		if (channelId == it->first) {
			return it->second;
		}
		++it;
	}
	return NULL;
}

} /* namespace flint */
