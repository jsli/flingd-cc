/*
 * IpcMessageChannel.h
 *
 *  Created on: 2015-1-4
 *      Author: manson
 */

#ifndef IPCMESSAGECHANNEL_H_
#define IPCMESSAGECHANNEL_H_

#include <boost/signals2.hpp>

#include "message/MessageChannel.h"
#include "utils/Logging.h"

namespace flint {

class IpcMessageChannel: public MessageChannel {
public:
	IpcMessageChannel();
	virtual ~IpcMessageChannel();

public:
	boost::signals2::signal<void(IpcMessageChannel *, const std::string &)> IpcMessage;

protected:
	virtual void onMessage(const std::string &message);
};

} /* namespace flint */
#endif /* IPCMESSAGECHANNEL_H_ */
