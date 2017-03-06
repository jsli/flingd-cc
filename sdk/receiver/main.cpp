#include <cstdio>

#include "FlintReceiverManager.h"
#include "ReceiverMessageBus.h"
#include "utils/Logging.h"
#include "media/MediaManager.h"

class Listener: public flint::ReceiverMessageBus::IListener {
	virtual void onMessage(flint::ReceiverMessageBus *bus,
			const std::string &message, const std::string &senderId) {
		LOG_ERROR << "from [" << senderId << "] received: [" << message << "]";
		bus->send("from receiver", senderId);
	}

	virtual void onSenderConnect(const std::string &senderId) {
		LOG_ERROR << "sender connected: " << senderId;
	}

	virtual void onSenderDisconnect(const std::string &senderId) {
		LOG_ERROR << "sender disconnected: " << senderId;
	}
};

int main(int argc, char **argv) {
	printf("FlintDaemon receiver SDK.\n");

//	flint::FlintReceiverManager *manager = new flint::FlintReceiverManager(
//			"~receiver_sdk_test");
//
//	flint::ReceiverMessageBus * bus = manager->createMessageBus();
//	bus->setListener(new Listener());
//
//	manager->open();

	flint::MediaManager *manager = new flint::MediaManager("~receiver_sdk_test");
	manager->open();

	printf("FlintDaemon receiver SDK -- end.\n");
	return 0;
}

