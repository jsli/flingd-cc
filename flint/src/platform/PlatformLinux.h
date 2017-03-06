/*
 * PlatformLinux.h
 *
 *  Created on: 2014-12-8
 *      Author: manson
 */

#ifndef PLATFORMLINUX_H_
#define PLATFORMLINUX_H_

#include <string>
#include <boost/process.hpp>

#include "platform/PlatformUnix.h"

namespace flint {

class PlatformLinux: public PlatformUnix {
public:
	PlatformLinux();
	virtual ~PlatformLinux();
};

} /* namespace flint */
#endif /* PLATFORMLINUX_H_ */
