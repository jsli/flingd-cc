/*
 * IpcMessageChannel.cpp
 *
 *  Created on: 2015-1-4
 *      Author: manson
 */

#include <string>

#include "IpcMessageChannel.h"

namespace flint {

IpcMessageChannel::IpcMessageChannel() :
		MessageChannel("ipc") {
	// TODO Auto-generated constructor stub
}

IpcMessageChannel::~IpcMessageChannel() {
	// TODO Auto-generated destructor stub
}

void IpcMessageChannel::onMessage(const std::string &message) {
	IpcMessage(this, message);
}

} /* namespace flint */
