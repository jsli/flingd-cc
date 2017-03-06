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

#ifndef WEBSOCKETSERVER_H_
#define WEBSOCKETSERVER_H_

#include <map>

#include "boost/signals2.hpp"
#include "boost/asio.hpp"

#include "http/HTTPServer.h"
#include "handler/WebsocketHandler.h"
#include "ws/WsServer.h"

namespace flint {

class ReceiverChannel;

class SenderChannel: public WebsocketHandler {
public:
	SenderChannel(WebsocketSession *session, const std::string &senderId,
			ReceiverChannel *channel);

	virtual ~SenderChannel();

	virtual void close();

public:
	const std::string & getSenderId();

protected:
	void onClose();
	void onMessage(const std::string &msg);

private:
	std::string senderId_;
	ReceiverChannel * receiverChannel_;
};

class ReceiverChannel: public WebsocketHandler {
public:
	ReceiverChannel(WebsocketSession *session, const std::string &channelId);

	virtual ~ReceiverChannel();

public:
	const std::string & getChannelId();

	bool removeSenderChannel(const std::string &senderId);

	bool addSenderChannel(const std::string &senderId,
			WebsocketSession *senderSession);

	SenderChannel *getSenderChannel(const std::string &senderId);

public:
	boost::signals2::signal<void(const std::string &)> Close;

protected:
	void onClose();
	void onMessage(const std::string &msg);

private:
	std::map<std::string, SenderChannel*> senders_;
	std::string channelId_;
};

class WebsocketServer: public WsServer {
public:
	WebsocketServer(boost::asio::io_service &ioService, int httpPort);
	virtual ~WebsocketServer();

protected:
	void onHttpRequest(HTTPSession *httpSession);

	void onWebsocketConnection(WebsocketSession *websocketSession);

	virtual void onStart();

	virtual void onStop();

private:
	bool addReceiverChannel(const std::string &channelId,
			WebsocketSession *session);

	bool removeReceiverChannel(const std::string &channelId);

	ReceiverChannel *getReceiverChannel(const std::string &channelId);

	void onReceiverConnected(const std::string &channelId,
			WebsocketSession *session);

	void onSenderConnected(const std::string &channelId,
			const std::string &senderId, WebsocketSession *session);

private:
	HTTPServer httpServer_;
	std::map<std::string, ReceiverChannel*> receivers_;
};

} /* namespace flint */
#endif /* WEBSOCKETSERVER_H_ */
