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

#include <cstdlib>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <boost/algorithm/string.hpp>

#include "PlatformPal.h"
#include "utils/Logging.h"
#include "utils/StringUtils.h"

namespace flint {

TcpSessionWrapper::TcpSessionWrapper(TCPSession *session) :
		session_(session) {
	session_->Close.connect(boost::bind(&TcpSessionWrapper::onClose, this));
	session_->Message.connect(
			boost::bind(&TcpSessionWrapper::onMessage, this, ::_1));
}

TcpSessionWrapper::~TcpSessionWrapper() {
	LOG_DEBUG << "~TcpSessionWrapper()";
}

void TcpSessionWrapper::onMessage(const std::string &message) {
	buffer_ += message;

	std::string data = "";
	int erase = parseMessage(buffer_, data);
	while (erase > 0) {
//		LOG_DEBUG << "need to erase [" << erase << "]";
//		LOG_DEBUG << "buffer is [" << buffer_ << "]";
//		LOG_DEBUG << "data is [" << data << "]";
		StringUtils::ShiftStringLeft(buffer_, erase);
//		LOG_DEBUG << "erased buffer is [" << buffer_ << "]";
		if (data != "") {
			processMessage(data);
			erase = parseMessage(buffer_, data);
		} else {
			break;
		}
	}
}

void TcpSessionWrapper::processMessage(const std::string &message) {
	LOG_INFO << session_->getAddr() << "  received\n" << message;

	rapidjson::StringBuffer sb;
	rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

	rapidjson::Document doc;
	doc.Parse(message.c_str());
	if (doc.IsNull()) {
		return;
	}

	rapidjson::Value &device_name = doc["name"];
	if (!device_name.IsNull()) {
		std::string name = device_name.GetString();
		NameChange(name);
	}

	rapidjson::Value &volumeMuted = doc["volumeMuted"];
	if (!volumeMuted.IsNull()) {
		bool muted = volumeMuted.GetBool();
		MuteChange(muted);
	}

	rapidjson::Value &volumeLevel = doc["volumeLevel"];
	if (!volumeLevel.IsNull()) {
		double volume = volumeLevel.GetDouble();
		VolumeChange(volume);
	}
}

void TcpSessionWrapper::onClose() {
	LOG_WARN << " session colsed!!!";
	Close(getAddr());
	session_ = NULL;
	delete this;
}

const std::string &TcpSessionWrapper::getAddr() {
	if (session_ != NULL) {
		return session_->getAddr();
	}
	return "";
}

void TcpSessionWrapper::send(const std::string &message) {
	if (session_ != NULL) {
		session_->send(message);
	}
}

/**
 * parse a single message from message buffer.
 * parameters:
 * 	message: need to parse
 * 	outData: a single available message
 * return value:
 * 	int: need to erase from message buffer
 */
int TcpSessionWrapper::parseMessage(const std::string &message,
		std::string &outData) {
	int index = StringUtils::FindFirstSymbol(":", message);
	if (index >= 0) {
		//parse message length
		std::string lenStr = StringUtils::HeadSubString(message, index).c_str();
		int length = atoi(lenStr.c_str());
		if (length <= 0) { //illegal message header
			LOG_WARN << "Illegal message header : " << lenStr.c_str();
			outData = "";
			// clean all buffer
			return message.length();
		} else {
			if ((unsigned int) length <= (message.size() - lenStr.size()/* message length */
			- 1/* ":" */)) {
				//get message body
				outData = StringUtils::SubString(message, index + 1, length);
				return (index + 1 + length);
			} else {
				// message is not complete, continue read and do not erase string
				outData = "";
				return 0;
			}
		}
	} else {
		return 0;
	}
}

PlatformPal::PlatformPal() :
		Platform(), tcpServer_(NULL) {
	// TODO Auto-generated constructor stub
}

PlatformPal::~PlatformPal() {
	// TODO Auto-generated destructor stub
	delete tcpServer_;
}

void PlatformPal::init(boost::asio::io_service &ioService) {
	Platform::init(ioService);

	if (tcpServer_ != NULL) {
		delete tcpServer_;
	}
	tcpServer_ = new TCPServer(ioService, 9440);

	// TODO: listen tcpServer
	tcpServer_->TcpConnection.connect(
			boost::bind(&PlatformPal::onSessionConnect, this, ::_1));

	tcpServer_->start();
}

void PlatformPal::reset() {
	if (tcpServer_ != NULL) {
		tcpServer_->stop();
		delete tcpServer_;
		tcpServer_ = NULL;
	}
}

void PlatformPal::onSessionConnect(TCPSession * session) {
	TcpSessionWrapper *wrapper = new TcpSessionWrapper(session);
	wrapper->Close.connect(
			boost::bind(&PlatformPal::onSessionClose, this, ::_1));
	wrapper->VolumeChange.connect(
			boost::bind(&PlatformPal::onVolumeChange, this, ::_1));
	wrapper->MuteChange.connect(
			boost::bind(&PlatformPal::onMuteChange, this, ::_1));
	wrapper->NameChange.connect(
			boost::bind(&PlatformPal::onNameChange, this, ::_1));
	sessions_.push_back(wrapper);
	LOG_INFO << "add: Pal sessions size = " << sessions_.size();
}

void PlatformPal::onVolumeChange(double volume) {
	LOG_INFO << "update volume : " << volume;
	volume_ = volume;
}

void PlatformPal::onMuteChange(bool mute) {
	LOG_INFO << "update mute : " << (mute ? "true" : "false");
	muted_ = mute;
}

void PlatformPal::onNameChange(const std::string &name) {
	LOG_INFO << "update device name : " << name;
	deviceName_ = name;
}

void PlatformPal::onSessionClose(const std::string &addr) {
	LOG_WARN << addr << " closed!!!";
	std::list<TcpSessionWrapper*>::iterator it = sessions_.begin();
	while (it != sessions_.end()) {
		if (boost::iequals((*it)->getAddr(), addr)) {
			sessions_.erase(it);
			break;
		} else {
			it++;
		}
	}
	LOG_INFO << "remove: Pal sessions size = " << sessions_.size();
}

void PlatformPal::sendMessageToAllSessions(const std::string &message) {
	std::stringstream ss;
	ss << message.size() << ":" << message;
	std::string _message = ss.str();
//	LOG_ERROR << "send:\n" << _message;
	std::list<TcpSessionWrapper*>::iterator it = sessions_.begin();
	while (it != sessions_.end()) {
		(*it)->send(_message);
		it++;
	}
}

void PlatformPal::setVolume(double volume) {
	onSetVolume(volume);
}

void PlatformPal::setMuted(bool muted) {
	onSetMuted(muted);
}

void PlatformPal::onSetVolume(double volume) {
	LOG_INFO << "onSetVolume";
	if (sessions_.size() > 0) {
		rapidjson::StringBuffer sb;
		rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
		writer.StartObject();
		writer.String("type");
		writer.String("SET_VOLUME");
		writer.String("level");
		writer.Double(volume);
		writer.EndObject();
		std::string msg = sb.GetString();
		sendMessageToAllSessions(msg);
	}
}

void PlatformPal::onSetMuted(bool muted) {
	LOG_INFO << "onSetMuted";
	if (sessions_.size() > 0) {
		rapidjson::StringBuffer sb;
		rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
		writer.StartObject();
		writer.String("type");
		writer.String("SET_MUTED");
		writer.String("muted");
		writer.Bool(muted);
		writer.EndObject();
		std::string msg = sb.GetString();
		sendMessageToAllSessions(msg);
	}
}

void PlatformPal::onLaunchWebApp(const std::string &url) {
	LOG_INFO << "PlatformPal  onLaunchWebApp";
	if (sessions_.size() > 0) {
		rapidjson::StringBuffer sb;
		rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
		writer.StartObject();
		writer.String("type");
		writer.String("LAUNCH_RECEIVER");
		writer.String("app_info");
		writer.StartObject();
		writer.String("url");
		writer.String(url.c_str());
		writer.EndObject();
		writer.EndObject();
		std::string msg = sb.GetString();
		sendMessageToAllSessions(msg);
	}
}

void PlatformPal::onLaunchNativeApp(const std::string &app) {
	LOG_INFO << "onLaunchNativeApp: " << app;
	if (sessions_.size() > 0) {
		rapidjson::StringBuffer sb;
		rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
		writer.StartObject();
		writer.String("type");
		writer.String("LAUNCH_RECEIVER");
		writer.String("app_info");
		writer.StartObject();
		writer.String("url");
		writer.String(app.c_str());
		writer.EndObject();
		writer.EndObject();
		std::string msg = sb.GetString();
		sendMessageToAllSessions(msg);
	}
}

void PlatformPal::onStopWebApp(const std::string &url) {
	LOG_INFO << "onStopWebApp";
	if (sessions_.size() > 0) {
		rapidjson::StringBuffer sb;
		rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
		writer.StartObject();
		writer.String("type");
		writer.String("STOP_RECEIVER");
		writer.String("app_info");
		writer.StartObject();
		writer.String("url");
		writer.String(url.c_str());
		writer.EndObject();
		writer.EndObject();
		std::string msg = sb.GetString();
		sendMessageToAllSessions(msg);
	}
}

void PlatformPal::onStopNativeApp(const std::string &app) {
	LOG_INFO << "onStopNativeApp";
	if (sessions_.size() > 0) {
		rapidjson::StringBuffer sb;
		rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
		writer.StartObject();
		writer.String("type");
		writer.String("STOP_RECEIVER");
		writer.String("app_info");
		writer.StartObject();
		writer.String("url");
		writer.String(app.c_str());
		writer.EndObject();
		writer.EndObject();
		std::string msg = sb.GetString();
		sendMessageToAllSessions(msg);
	}
}

} /* namespace flint */
