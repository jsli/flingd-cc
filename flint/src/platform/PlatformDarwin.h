/*
 * PlatformDarwin.h
 *
 *  Created on: 2014-12-8
 *      Author: manson
 */

#ifndef PLATFORMDARWIN_H_
#define PLATFORMDARWIN_H_

#include <string>

#include "platform/PlatformUnix.h"

namespace flint {

class PlatformDarwin: public PlatformUnix {
public:
	PlatformDarwin();
	virtual ~PlatformDarwin();
};

} /* namespace flint */
#endif /* PLATFORMDARWIN_H_ */
