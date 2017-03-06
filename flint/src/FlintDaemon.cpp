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

#include "FlintDaemon.h"
#include "platform/Platform.h"
#include "utils/Logging.h"

namespace flint {

FlintDaemon::FlintDaemon() :
		IServer(), ioService_(NULL), flintServer_(NULL), wsServer_(NULL), peerServer_(
				NULL), flintDiscovery_(NULL), errorCode_(NONE_ERROR) {
	// TODO Auto-generated constructor stub
}

FlintDaemon::FlintDaemon(boost::asio::io_service *ioService) :
		IServer(), ioService_(ioService), flintServer_(NULL), wsServer_(NULL), peerServer_(
				NULL), flintDiscovery_(NULL), errorCode_(NONE_ERROR) {
	// TODO Auto-generated constructor stub
}

FlintDaemon::~FlintDaemon() {
	// TODO Auto-generated destructor stub
	clear();
}

void FlintDaemon::onStart() {
	LOG_DEBUG << "FlintDaemon onStart!!!";
	errorCode_ = NONE_ERROR;

	if (ioService_ != NULL) {
		delete ioService_;
	}
	ioService_ = new boost::asio::io_service();

	if (flintServer_ != NULL) {
		delete flintServer_;
	}
	flintServer_ = new FlintServer(*ioService_, 9431);

	if (wsServer_ != NULL) {
		delete wsServer_;
	}
	wsServer_ = new WebsocketServer(*ioService_, 9439);

	if (peerServer_ != NULL) {
		delete peerServer_;
	}
	peerServer_ = new PeerServer(*ioService_, 9433);

	if (flintDiscovery_ != NULL) {
		delete flintDiscovery_;
	}
	flintDiscovery_ = new FlintDiscovery(*ioService_);

	try {
		if (errorCode_ == NONE_ERROR) {
			flintServer_->start();
		}
	} catch (websocketpp::exception &e) {
		LOG_ERROR << "FlintServer start failed:" << e.what() << "|" << e.code();
		errorCode_ = FLINTSERVER_ERROR;
	}

	try {
		if (errorCode_ == NONE_ERROR) {
			wsServer_->start();
		}
	} catch (websocketpp::exception &e) {
		LOG_ERROR << "WebsocketServer start failed:" << e.what() << "|"
				<< e.code();
		errorCode_ = WEBSOCKETSERVER_ERROR;
	}

	try {
		if (errorCode_ == NONE_ERROR) {
			flintDiscovery_->start();
		}
	} catch (std::exception &e) {
		LOG_ERROR << "FlintDiscovery start failed:" << e.what();
		errorCode_ = FLINTDISCOVERY_ERROR;
	}

	try {
		if (errorCode_ == NONE_ERROR) {
			peerServer_->start();
		}
	} catch (websocketpp::exception &e) {
		LOG_ERROR << "PeerServer start failed:" << e.what() << "|" << e.code();
		errorCode_ = PEERSERVER_ERROR;
	}

	Platform::getInstance()->init(*ioService_);

	if (errorCode_ == NONE_ERROR) {
		ioService_->run();
	} else {
		LOG_ERROR << "FlintDaemon start error: " << errorCode_;
		stop();
	}
}

void FlintDaemon::onStop() {
	LOG_DEBUG << "FlintDaemon onStop!!!";

	clear();
}

FlintDaemon::ERROR_CODE FlintDaemon::getErrorCode() {
	return errorCode_;
}

void FlintDaemon::clear() {

	if (flintDiscovery_ != NULL) {
		flintDiscovery_->stop();
		delete flintDiscovery_;
		flintDiscovery_ = NULL;
	}

	if (flintServer_ != NULL) {
		flintServer_->stop();
		delete flintServer_;
		flintServer_ = NULL;
	}

	if (wsServer_ != NULL) {
		wsServer_->stop();
		delete wsServer_;
		wsServer_ = NULL;
	}

	if (peerServer_ != NULL) {
		peerServer_->stop();
		delete peerServer_;
		peerServer_ = NULL;
	}

	// reset Platform last
	Platform::getInstance()->reset();

	if (ioService_ != NULL) {
		ioService_->stop();
		delete ioService_;
		ioService_ = NULL;
	}
}

} /* namespace flint */
