/*
 * ReceiverMessageBus.h
 *
 *  Created on: 2015-1-4
 *      Author: manson
 */

#ifndef RECEIVERMESSAGEBUS_H_
#define RECEIVERMESSAGEBUS_H_

#include "message/MessageBus.h"
#include "ReceiverMessageChannel.h"

namespace flint {

class ReceiverMessageBus: public MessageBus {
public:
	class IListener {
	public:
		IListener() {
		}

		virtual ~IListener() {
		}

	public:
		virtual void onMessage(ReceiverMessageBus *bus,
				const std::string &message, const std::string &senderId) = 0;

		virtual void onSenderConnect(const std::string &senderId) = 0;

		virtual void onSenderDisconnect(const std::string &senderId) = 0;
	};

public:
	ReceiverMessageBus(const std::string &ns,
			ReceiverMessageChannel *messageChannel);
	virtual ~ReceiverMessageBus();

	void send(const std::string &message);

	void send(const std::string &message, const std::string &senderId);

	void setListener(ReceiverMessageBus::IListener *listener);

	void onSenderConnect(const std::string &senderId);

	void onSenderDisconnect(const std::string &senderId);

	void onMessage(const std::string &message, const std::string &senderId);

private:
	ReceiverMessageChannel * messageChannel_;
	ReceiverMessageBus::IListener *listener_;
};

} /* namespace flint */
#endif /* RECEIVERMESSAGEBUS_H_ */
