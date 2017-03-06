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

#include <stdio.h>
#include <stdlib.h>

#include "PlatformUnix.h"
#include "utils/Logging.h"

namespace flint {

PlatformUnix::PlatformUnix() :
		Platform(), process_(0), browserPath_("") {
	// TODO Auto-generated constructor stub
}

PlatformUnix::~PlatformUnix() {
	// TODO Auto-generated destructor stub
}

void PlatformUnix::onLaunchWebApp(const std::string &url) {

	LOG_INFO << " launch web application: " << url;
	if (url.size() == 0) {
		LOG_ERROR << "illegal url: " << url;
		return;
	}

	std::vector<std::string> args;
	args.push_back(browserPath_);
	args.push_back("--no-default-browser-check");
	args.push_back("--enable-logging");
	args.push_back("--no-first-run");
	args.push_back("--disable-Application-cache");
	args.push_back("--disable-cache");
	args.push_back("--enable-kiosk-mode");
	args.push_back("--kiosk");
	args.push_back("--start-maximized");
	args.push_back("--window-size=1280,720");

//	[2497:35075:0302/182441:ERROR:proxy_service_factory.cc(104)] Cannot use V8 Proxy resolver in single process mode.
	//	args.push_back("--single-process");

	args.push_back("--allow-insecure-websocket-from-https-origin");
	args.push_back("--allow-running-insecure-content");
	args.push_back("--user-data-dir=/tmp/" + getDeviceUUID());
	args.push_back("--app=" + url);

	try {
		process_ = boost::process::execute(
				boost::process::initializers::set_args(args));
	} catch (std::exception &e) {
		LOG_ERROR << " launch web application faield: " << url;
		process_.pid = 0;
	}
	LOG_INFO << " launch web application end: " << url;
}

void PlatformUnix::onLaunchNativeApp(const std::string &app) {
	LOG_INFO << " launch native application: " << app;
	std::vector<std::string> args;

	if (boost::filesystem::exists(app)) {
		args.push_back(app);
	} else {
		LOG_ERROR << " illegal app: " << app;
		return;
	}

	try {
		process_ = boost::process::execute(
				boost::process::initializers::set_args(args));
	} catch (std::exception &e) {
		LOG_ERROR << " launch native application faield: " << app;
		process_.pid = 0;
	}

	LOG_INFO << " launch native application end: " << app;
}

void PlatformUnix::onStopWebApp(const std::string &url) {
	if (process_.pid != 0) {
		boost::process::terminate(process_);
		boost::system::error_code ec;
		boost::process::wait_for_exit(process_, ec);
		process_.pid = 0;
	} else {
		LOG_ERROR << " stop web application faield: " << url;
	}
}

void PlatformUnix::onStopNativeApp(const std::string &app) {
	if (process_.pid != 0) {
		boost::process::terminate(process_);
		boost::system::error_code ec;
		boost::process::wait_for_exit(process_, ec);
		process_.pid = 0;
	} else {
		LOG_ERROR << " stop native application faield: " << app;
	}
}

void PlatformUnix::onSetVolume(double volume) {
	LOG_INFO << " onSetVolume: " << volume;
}

void PlatformUnix::onSetMuted(bool muted) {
	LOG_INFO << " onSetMuted: " << muted;
}

} /* namespace flint */
