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

#ifndef PLATFORM_H_
#define PLATFORM_H_

#include <string>

#include <boost/signals2.hpp>
#include <boost/asio.hpp>

namespace flint {

class Platform {
public:
	Platform();
	virtual ~Platform();

	static Platform *getInstance();

	static std::string const & getDeviceUUID();

	virtual bool launchApplication(const std::string &appInfo);

	virtual void stopApplication(const std::string &appInfo = "");

	virtual void init(boost::asio::io_service &ioService);

	virtual void reset();

public:
	double getVolume();
	virtual void setVolume(double volume);

	bool getMuted();
	virtual void setMuted(bool muted);

	std::string const & getDeviceName();
	void setDeviceName(const std::string deviceName);

	std::string const & getModelName();
	void setModelName(const std::string modelName);

protected:
	virtual void onSetVolume(double volume) = 0;
	virtual void onSetMuted(bool muted) = 0;

	virtual void onLaunchWebApp(const std::string &url) = 0;
	virtual void onLaunchNativeApp(const std::string &app) = 0;

	virtual void onStopWebApp(const std::string &url) = 0;
	virtual void onStopNativeApp(const std::string &app) = 0;

	bool isWebApp(const std::string &url);bool isNativeApp(
			const std::string &url);

	std::string parseAppInfo(const std::string & appInfo);

protected:
	bool muted_;
	double volume_;
	std::string deviceName_;
	std::string modelName_;
};

} /* namespace flint */
#endif /* PLATFORM_H_ */
