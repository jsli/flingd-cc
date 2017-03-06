/*
 * MessageBusManager.h
 *
 *  Created on: 2014-12-21
 *      Author: manson
 */

#ifndef MESSAGEBUSMANAGER_H_
#define MESSAGEBUSMANAGER_H_

#include <string>
#include <map>

#include "ReceiverMessageChannel.h"
#include "ReceiverMessageBus.h"

namespace flint {

class MessageBusManager {
public:
	MessageBusManager(boost::asio::io_service *ioService,
			const std::string &appId, const std::string &url);
	virtual ~MessageBusManager();

	void open();

	void close();

	ReceiverMessageBus * createMessageBus(const std::string &ns);

private:
	void onOpen(MessageChannel *channel);
	void onClose(MessageChannel *channel);
	void onFailed(MessageChannel *channel);
	void onMessage(ReceiverMessageChannel *channel, const std::string &type,
			const std::string &senderId, const std::string &message);

	void clear();

private:
	bool isOpened_;
	boost::asio::io_service *ioService_;
	ReceiverMessageChannel *messageChannel_;
	std::string appId_;
	std::string url_;
	std::map<std::string, ReceiverMessageBus *> messageBusMap_; // <namespace, MessageBus>
};

} /* namespace flint */
#endif /* MESSAGEBUSMANAGER_H_ */
