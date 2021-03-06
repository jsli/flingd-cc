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

#ifndef FLINTDISCOVERY_H_
#define FLINTDISCOVERY_H_

#include <boost/asio.hpp>

#include "comm/IServer.h"
#include "discovery/ssdp/SSDPServer.h"
#include "discovery/mdns/MDNSServer.h"

namespace flint {

class FlintDiscovery: public IServer {
public:
	FlintDiscovery(boost::asio::io_service &ioService);
	virtual ~FlintDiscovery();

protected:
	virtual void onStart();

	virtual void onStop();

private:
	boost::asio::io_service &ioService_;
	SSDPServer *ssdpServer_;
	MDNSServer *mdnsServer_;
};

} /* namespace flint */
#endif /* FLINTDISCOVERY_H_ */
