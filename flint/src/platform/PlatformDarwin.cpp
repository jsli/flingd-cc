/*
 * PlatformDarwin.cpp
 *
 *  Created on: 2014-12-8
 *      Author: manson
 */

#include "PlatformDarwin.h"
#include "utils/Logging.h"

namespace flint {

PlatformDarwin::PlatformDarwin() :
		PlatformUnix() {
	// TODO Auto-generated constructor stub
	deviceName_ += "-Darwin";
	std::string _chromePath =
			"/Applications/Google Chrome.app/Contents/MacOS/Google Chrome";
	if (boost::filesystem::exists(_chromePath)) {
		browserPath_ = _chromePath;
	} else {
		LOG_ERROR << "Please install chrome browser";
	}
}

PlatformDarwin::~PlatformDarwin() {
	// TODO Auto-generated destructor stub
}

} /* namespace flint */
