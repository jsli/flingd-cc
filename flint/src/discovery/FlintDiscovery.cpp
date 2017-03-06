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

#include "FlintDiscovery.h"
#include "utils/Logging.h"

namespace flint {

#define ENABLE_SSDP true
#define ENABLE_MDNS true

FlintDiscovery::FlintDiscovery(boost::asio::io_service &ioService) :
		IServer(), ioService_(ioService), ssdpServer_(NULL), mdnsServer_(NULL) {
	// TODO Auto-generated constructor stub

}

FlintDiscovery::~FlintDiscovery() {
	// TODO Auto-generated destructor stub
	if (ENABLE_SSDP) {
		if (ssdpServer_ != NULL) {
			delete ssdpServer_;
		}
	}

	if (ENABLE_MDNS) {
		if (mdnsServer_ != NULL) {
			delete mdnsServer_;
		}
	}
}

void FlintDiscovery::onStart() {
	LOG_INFO << "FlintDiscovery onStart!!!";
	if (ENABLE_SSDP) {
		if (ssdpServer_ != NULL) {
			delete ssdpServer_;
		}
		ssdpServer_ = new SSDPServer(ioService_);
		ssdpServer_->start();
	}

	if (ENABLE_MDNS) {
		if (mdnsServer_ != NULL) {
			delete mdnsServer_;
		}
		mdnsServer_ = new MDNSServer(ioService_, 9431);
		mdnsServer_->start();
	}
}

void FlintDiscovery::onStop() {
	LOG_DEBUG << "FlintDiscovery onStop!!!";
	if (ENABLE_SSDP) {
		if (ssdpServer_ != NULL) {
			ssdpServer_->stop();
			delete ssdpServer_;
			ssdpServer_ = NULL;
		}
	}

	if (ENABLE_MDNS) {
		if (mdnsServer_ != NULL) {
			mdnsServer_->stop();
			delete mdnsServer_;
			mdnsServer_ = NULL;
		}
	}
}

} /* namespace flint */
