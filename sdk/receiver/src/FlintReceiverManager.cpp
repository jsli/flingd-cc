/*
 * FlintReceiverManager.cpp
 *
 *  Created on: 2014-12-21
 *      Author: manson
 */

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include "FlintReceiverManager.h"
#include "utils/Logging.h"

namespace flint {

FlintReceiverManager::FlintReceiverManager(const std::string &appId) :
		isOpened_(false), appId_(appId), messageBusManager_(NULL), ipcChannel_(
				NULL), flintServiceUrl_("127.0.0.1"), customAdditionalData_("") {
	// TODO Auto-generated constructor stub
	ipcChannel_ = new IpcMessageChannel();
	ipcChannel_->Open.connect(
			boost::bind(&FlintReceiverManager::onIpcOpen, this, ::_1));
	ipcChannel_->Close.connect(
			boost::bind(&FlintReceiverManager::onIpcClose, this, ::_1));
	ipcChannel_->Failed.connect(
			boost::bind(&FlintReceiverManager::onIpcFailed, this, ::_1));
	ipcChannel_->IpcMessage.connect(
			boost::bind(&FlintReceiverManager::onIpcMessage, this, ::_1, ::_2));

	messageBusManager_ = new MessageBusManager(&ioService_, appId_,
			"ws://127.0.0.1:9439/channels/channelBaseUrl");
}

FlintReceiverManager::~FlintReceiverManager() {
	// TODO Auto-generated destructor stub
}

void FlintReceiverManager::open() {
	if (isOpened_) {
		LOG_ERROR << "FlintReceiverManager cannot be reopen!!!";
		return;
	}

	if (messageBusManager_ != NULL) {
		messageBusManager_->open();
	}

	if (ipcChannel_ != NULL) {
		ipcChannel_->open("ws://127.0.0.1:9431/receiver/" + appId_,
				&ioService_);
	}

	isOpened_ = true;
	ioService_.run();
}

void FlintReceiverManager::close() {
	if (!isOpened_) {
		LOG_ERROR << "FlintReceiverManager is not opened!!!";
		return;
	}

	unRegisterSelf();

	if (messageBusManager_ != NULL) {
		messageBusManager_->close();
	}

	if (ipcChannel_ != NULL) {
		ipcChannel_->close();
	}
	clear();
}

void FlintReceiverManager::unRegisterSelf() {
	rapidjson::StringBuffer s;
	rapidjson::Writer<rapidjson::StringBuffer> writer(s);
	writer.StartObject();
	writer.String("type");
	writer.String("unregister");
	writer.String("appid");
	writer.String(appId_.c_str());
	writer.EndObject();

	ipcSend(s.GetString());
}

ReceiverMessageBus * FlintReceiverManager::createMessageBus() {
	return createMessageBus("urn:flint:org.openflint.default");
}

ReceiverMessageBus * FlintReceiverManager::createMessageBus(
		const std::string &ns) {
	if (isOpened()) {
		LOG_ERROR
				<< "cannot create message bus! flint receiver manager is opened!!!";
		return NULL;
	}

	if (messageBusManager_ != NULL) {
		return messageBusManager_->createMessageBus(ns);
	} else {
		return NULL;
	}
}

void FlintReceiverManager::setAdditionalData(const std::string &data) {
	customAdditionalData_ = data;
	sendAdditionalData();
}

void FlintReceiverManager::sendAdditionalData() {
	std::string additionalData = joinAdditionalData();
	if (additionalData != "") {
		ipcSend(additionalData);
	}
}

std::string FlintReceiverManager::joinAdditionalData() {
	if (messageBusManager_ == NULL && customAdditionalData_ == "") {
		return "";
	}

	rapidjson::StringBuffer s;
	rapidjson::Writer<rapidjson::StringBuffer> writer(s);
	writer.StartObject();
	writer.String("type");
	writer.String("additionaldata");
	writer.String("appid");
	writer.String(appId_.c_str());

	writer.String("additionaldata");
	writer.StartObject();
	if (messageBusManager_ != NULL) {
		writer.String("channelBaseUrl");
		writer.String(
				std::string(
						"ws://" + flintServiceUrl_ + ":9439/channels/"
								+ "channelBaseUrl").c_str());
	}
	if (customAdditionalData_ != "") {
		writer.String("customData");
		writer.String(customAdditionalData_.c_str());
	}
	writer.EndObject();

	writer.EndObject();

	return std::string(s.GetString());
}

void FlintReceiverManager::sendHeartbeat(const std::string &heartbeat) {
	rapidjson::StringBuffer s;
	rapidjson::Writer<rapidjson::StringBuffer> writer(s);
	writer.StartObject();
	writer.String("type");
	writer.String("heartbeat");
	writer.String("appid");
	writer.String(appId_.c_str());
	writer.String("heartbeat");
	writer.String(heartbeat.c_str());
	writer.EndObject();
	ipcSend(s.GetString());
}

void FlintReceiverManager::onIpcOpen(MessageChannel *channel) {
	LOG_INFO << "IPC channel: opened!!!";
	registerSelf();
}

void FlintReceiverManager::registerSelf() {
	rapidjson::StringBuffer s;
	rapidjson::Writer<rapidjson::StringBuffer> writer(s);
	writer.StartObject();
	writer.String("type");
	writer.String("register");
	writer.String("appid");
	writer.String(appId_.c_str());
	writer.EndObject();

	ipcSend(s.GetString());
}

void FlintReceiverManager::onIpcMessage(IpcMessageChannel *channel,
		const std::string &message) {
	LOG_DEBUG << "IPC channel onMessage: " << message;
	if (message.size() == 0) {
		return;
	}

	rapidjson::Document d;
	d.Parse(message.c_str());
	if (d.IsNull()) {
		return;
	}

	// check message's item
	rapidjson::Value& appId = d["appid"];
	if (appId.IsNull()) {
		return;
	} else if (appId_ != std::string(appId.GetString())) {
		return;
	}
	rapidjson::Value& type = d["type"];
	if (type.IsNull()) {
		return;
	}

	std::string typeStr = type.GetString();
	LOG_INFO << "IPC channel received type: " << typeStr;
	if (typeStr == "registerok") {
		rapidjson::Value& service_info = d["service_info"];
		if (service_info.IsNull()) {
			return;
		}

		rapidjson::Value& ipArr = service_info["ip"];
		if (ipArr.IsNull() || !ipArr.IsArray()) {
			return;
		}
		if (ipArr.Size() == 0) {
			return;
		}
		std::string ip = ipArr.Begin()->GetString();
		flintServiceUrl_ = ip;
		LOG_INFO << "get flint service url: " << flintServiceUrl_;

		sendAdditionalData();
	} else if (typeStr == "startHeartbeat") {
		LOG_INFO << appId << ": receiver ready to start heartbeat!!!";
	} else if (typeStr == "heartbeat") {
		rapidjson::Value& heartbeat = d["heartbeat"];
		if (!heartbeat.IsNull()) {
			if (std::string(heartbeat.GetString()) == "ping") {
				sendHeartbeat("pong");
			} else if (std::string(heartbeat.GetString()) == "pong") {
				sendHeartbeat("ping");
			}
		}
	} else if (typeStr == "senderconnected") {
		// TODO: ignore
		rapidjson::Value& token = d["token"];
		LOG_INFO << "sender connected ---> " << token.GetString();
	} else if (typeStr == "senderdisconnected") {
		// TODO: ignore
		rapidjson::Value& token = d["token"];
		LOG_INFO << "sender disconnected ---> " << token.GetString();
	} else {
		LOG_INFO << "IPC channel received unknow type: " << typeStr;
	}
}

bool FlintReceiverManager::isOpened() {
	if (isOpened_ && ipcChannel_ != NULL && ipcChannel_->isOpened()) {
		return true;
	}
	return false;
}

void FlintReceiverManager::clear() {
	if (ipcChannel_ != NULL) {
		delete ipcChannel_;
		ipcChannel_ = NULL;
	}
	if (messageBusManager_ != NULL) {
		delete messageBusManager_;
		messageBusManager_ = NULL;
	}

	isOpened_ = false;
}

void FlintReceiverManager::onIpcClose(MessageChannel *channel) {
	LOG_WARN << "IPC channel: closed!!!";
	clear();
}

void FlintReceiverManager::onIpcFailed(MessageChannel *channel) {
	LOG_ERROR << "IPC channel: failed!!!";
	clear();
}

void FlintReceiverManager::ipcSend(const std::string &message) {
	if (ipcChannel_ != NULL) {
		ipcChannel_->send(message);
	} else {
		LOG_ERROR << "ipc channel is null, cannot send: " << message;
	}
}

} /* namespace flint */
