/*
 * MessageBus.cpp
 *
 *  Created on: 2014-12-21
 *      Author: manson
 */

#include "MessageBus.h"
#include "message/MessageChannel.h"
#include "utils/Logging.h"

namespace flint {

MessageBus::MessageBus(const std::string &ns) :
		namespace_(ns) {
	// TODO Auto-generated constructor stub
}

MessageBus::~MessageBus() {
	// TODO Auto-generated destructor stub
}

const std::string & MessageBus::getNamesapce() {
	return namespace_;
}

} /* namespace flint */
