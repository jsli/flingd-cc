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

#include <sstream>
#include <cstdio>
#include <exception>

#include <boost/algorithm/string.hpp>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include "Platform.h"
#include "PlatformDarwin.h"
#include "PlatformLinux.h"
#include "PlatformAndroid.h"
#include "PlatformFfos.h"
#include "utils/Logging.h"
#include "utils/UUID.h"

namespace flint {

static Platform *sPlatform = NULL;
static std::string sUUID = "";

Platform::Platform() {
	modelName_ = "Matchstick";
	deviceName_ = modelName_ + getDeviceUUID().substr(0, 4);
	LOG_INFO << "Platform modelName: " << modelName_ << ", deviceName: "
			<< deviceName_;
}

Platform::~Platform() {
	// TODO Auto-generated destructor stub
}

Platform *Platform::getInstance() {
	if (sPlatform == NULL) {
#ifdef APPLE
//		LOG_INFO << "new PlatformDarwin";
		sPlatform = new PlatformDarwin();
#elif LINUX
//		LOG_INFO << "new PlatformLinux";
		sPlatform = new PlatformLinux();
#elif ANDROID
//		LOG_INFO << "new PlatformAndroid";
		sPlatform = new PlatformAndroid();
#elif FFOS
//		LOG_INFO << "new PlatformFfos";
		sPlatform = new PlatformFfos();
#else
		throw new std::exception();
#endif
	}
	return sPlatform;
}

std::string const & Platform::getDeviceUUID() {
	if (sUUID == "") {
		sUUID = UUID::generateUUID();
	}
	return sUUID;
}

bool Platform::launchApplication(const std::string &appInfo) {
	std::string url = parseAppInfo(appInfo);
	if (isWebApp(url)) {
		onLaunchWebApp(url);
		return true;
	} else if (isNativeApp(url)) {
		std::string app = url;
		boost::ierase_first(app, "app:?");
		onLaunchNativeApp(app);
		return true;
	} else {
		LOG_WARN << "Platform cannot handle launch request: " << appInfo;
		return false;
	}
}

void Platform::stopApplication(const std::string &appInfo) {
	std::string url = parseAppInfo(appInfo);
	if (isWebApp(url)) {
		onStopWebApp(url);
	} else if (isNativeApp(url)) {
		std::string app = url;
		boost::ierase_first(app, "app:?");
		onStopNativeApp(app);
	} else {
		LOG_WARN << "Platform cannot handle stop request: " << appInfo;
	}
}

std::string Platform::parseAppInfo(std::string const & appInfo) {
	rapidjson::Document appInfoDoc;
	appInfoDoc.Parse(appInfo.c_str());
	std::string url = appInfoDoc["url"].GetString();
	return url;
}

bool Platform::isWebApp(const std::string &url) {
	if (boost::starts_with(url, "http://")
			|| boost::starts_with(url, "https://")) {
		return true;
	}
	return false;
}

bool Platform::isNativeApp(const std::string &url) {
	if (boost::starts_with(url, "app:?")) {
		return true;
	}
	return false;
}

void Platform::setVolume(double volume) {
	if (volume_ != volume) {
		volume_ = volume;
		onSetVolume(volume);
	}
}

double Platform::getVolume() {
	return volume_;
}

void Platform::setMuted(bool muted) {
	if (muted_ != muted) {
		muted_ = muted;
		onSetMuted(muted);
	}
}

bool Platform::getMuted() {
	return muted_;
}

std::string const & Platform::getDeviceName() {
	return deviceName_;
}

void Platform::setDeviceName(const std::string deviceName) {
	LOG_INFO << "set device name " << deviceName;
	deviceName_ = deviceName;
}

std::string const & Platform::getModelName() {
	return modelName_;
}

void Platform::setModelName(const std::string modelName) {
	LOG_INFO << "set model name " << modelName;
	modelName_ = modelName;
}

void Platform::init(boost::asio::io_service &ioService) {
	LOG_DEBUG << "Platform init: " << deviceName_;
	LOG_DEBUG << "Platform init: " << modelName_;
}

void Platform::reset() {
	LOG_DEBUG << "Platform reset: " << deviceName_;
	LOG_DEBUG << "Platform reset: " << modelName_;
}

} /* namespace flint */
