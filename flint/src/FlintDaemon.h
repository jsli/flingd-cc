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

#ifndef FLINTDAEMON_H_
#define FLINTDAEMON_H_

#include <boost/asio.hpp>

#include "comm/IServer.h"
#include "flint/FlintServer.h"
#include "ws/WebsocketServer.h"
#include "discovery/FlintDiscovery.h"
#include "peer/PeerServer.h"

namespace flint {

class FlintDaemon: public IServer {
public:
	enum ERROR_CODE {
		NONE_ERROR = 0,
		FLINTSERVER_ERROR = 1,
		WEBSOCKETSERVER_ERROR = 2,
		FLINTDISCOVERY_ERROR = 3,
		PEERSERVER_ERROR = 4
	};

public:
	FlintDaemon();
	FlintDaemon(boost::asio::io_service *ioService);

	virtual ~FlintDaemon();

	ERROR_CODE getErrorCode();

protected:
	virtual void onStart();

	virtual void onStop();

	void clear();

private:
	boost::asio::io_service *ioService_;
	FlintServer *flintServer_;
	WebsocketServer *wsServer_;
	PeerServer *peerServer_;
	FlintDiscovery *flintDiscovery_;
	ERROR_CODE errorCode_;
};

} /* namespace flint */
#endif /* FLINTDAEMON_H_ */
