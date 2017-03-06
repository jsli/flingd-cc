/*
 * Copyright (C) 2013-2014, The OpenFlint Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS-IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PLATFORMUNIX_H_
#define PLATFORMUNIX_H_

#include <string>
#include <boost/process.hpp>

#include "platform/Platform.h"

namespace flint {

class PlatformUnix: public Platform {
public:
	PlatformUnix();
	virtual ~PlatformUnix();

protected:
	virtual void onSetVolume(double volume);
	virtual void onSetMuted(bool muted);

	virtual void onLaunchWebApp(const std::string &url);
	virtual void onLaunchNativeApp(const std::string &app);

	virtual void onStopWebApp(const std::string &url);
	virtual void onStopNativeApp(const std::string &app);

protected:
	boost::process::child process_;
	std::string browserPath_;
};

} /* namespace flint */
#endif /* PLATFORMUNIX_H_ */
