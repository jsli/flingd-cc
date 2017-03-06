/*
 * MessageBus.h
 *
 *  Created on: 2014-12-21
 *      Author: manson
 */

#ifndef MESSAGEBUS_H_
#define MESSAGEBUS_H_

#include <string>

namespace flint {

class MessageBus {
public:
	MessageBus(const std::string &ns);
	virtual ~MessageBus();

public:
	const std::string & getNamesapce();

protected:
	std::string namespace_;
};

} /* namespace flint */
#endif /* MESSAGEBUS_H_ */
