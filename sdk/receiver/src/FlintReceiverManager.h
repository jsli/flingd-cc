/*
 * FlintReceiverManager.h
 *
 *  Created on: 2014-12-21
 *      Author: manson
 */

#ifndef FLINTRECEIVERMANAGER_H_
#define FLINTRECEIVERMANAGER_H_

#include <string>
#include <map>

#include <boost/asio.hpp>

#include "IpcMessageChannel.h"
#include "ReceiverMessageChannel.h"
#include "ReceiverMessageBus.h"
#include "MessageBusManager.h"

namespace flint {

class FlintReceiverManager {
public:
	FlintReceiverManager(const std::string &appId);
	virtual ~FlintReceiverManager();

	void open();

	void close();

	void setAdditionalData(const std::string &data);

	ReceiverMessageBus * createMessageBus();

	ReceiverMessageBus * createMessageBus(const std::string &ns);

private:
	void clear();

	bool isOpened();

	void ipcSend(const std::string &message);

	std::string joinAdditionalData();

	void sendAdditionalData();

	void sendHeartbeat(const std::string &heartbeat);

	void registerSelf();

	void unRegisterSelf();

	void onIpcOpen(MessageChannel *channel);
	void onIpcClose(MessageChannel *channel);
	void onIpcFailed(MessageChannel *channel);
	void onIpcMessage(IpcMessageChannel *channel, const std::string &message);

private:
	bool isOpened_;
	std::string appId_;
	MessageBusManager *messageBusManager_;
	IpcMessageChannel * ipcChannel_;
	std::string flintServiceUrl_;
	std::string customAdditionalData_;

	boost::asio::io_service ioService_;
};

} /* namespace flint */
#endif /* FLINTRECEIVERMANAGER_H_ */
