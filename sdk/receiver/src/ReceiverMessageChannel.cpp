/*
 * ReceiverMessageChannel.cpp
 *
 *  Created on: 2015-1-3
 *      Author: manson
 */

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include "ReceiverMessageChannel.h"
#include "utils/Logging.h"

namespace flint {

ReceiverMessageChannel::ReceiverMessageChannel(const std::string &name) :
		MessageChannel(name) {
	// TODO Auto-generated constructor stub

}

ReceiverMessageChannel::~ReceiverMessageChannel() {
	// TODO Auto-generated destructor stub
}

void ReceiverMessageChannel::onMessage(const std::string &message) {

	rapidjson::Document d;
	d.Parse(message.c_str());
	if (d.IsNull()) {
		return;
	}

	// check message's item
	rapidjson::Value& type = d["type"];
	if (type.IsNull()) {
		return;
	}

	rapidjson::Value& senderId = d["senderId"];
	if (senderId.IsNull()) {
		return;
	}

	std::string senderIdStr = senderId.GetString();
	std::string typeStr = type.GetString();
	if (typeStr == "senderConnected") {
		ReceiverMessage(this, "senderConnected", senderIdStr, "");
	} else if (typeStr == "senderDisconnected") {
		ReceiverMessage(this, "senderDisconnected", senderIdStr, "");
	} else if (typeStr == "message") {
		rapidjson::Value& data = d["data"];
		if (!data.IsNull()) {
			ReceiverMessage(this, "message", senderIdStr, data.GetString());
		}
	} else if (typeStr == "error") {
		LOG_ERROR << "receiver message channel received error: " << message;
	} else {
		LOG_ERROR << "receiver message channel received unknow: " << message;
	}
}

} /* namespace flint */
