/*
 * ReceiverMessageChannel.h
 *
 *  Created on: 2015-1-3
 *      Author: manson
 */

#ifndef RECEIVERMESSAGECHANNEL_H_
#define RECEIVERMESSAGECHANNEL_H_

#include "message/MessageChannel.h"

namespace flint {

class ReceiverMessageChannel: public MessageChannel {
public:
	ReceiverMessageChannel(const std::string &name);
	virtual ~ReceiverMessageChannel();

public:
	boost::signals2::signal<
			void(ReceiverMessageChannel *, const std::string &/*type*/,
					const std::string & /*senderId*/,
					const std::string &/*data*/)> ReceiverMessage;

protected:
	virtual void onMessage(const std::string &message);
};

} /* namespace flint */
#endif /* RECEIVERMESSAGECHANNEL_H_ */
