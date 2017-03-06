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

#ifndef PLATFORMPAL_H_
#define PLATFORMPAL_H_

#include <map>
#include <string>

#include <boost/asio.hpp>
#include <boost/signals2.hpp>

#include "Platform.h"
#include "tcp/TCPServer.h"

namespace flint {

class TcpSessionWrapper {
public:
	TcpSessionWrapper(TCPSession *session);
	virtual ~TcpSessionWrapper();

	const std::string &getAddr();

	void send(const std::string &message);

public:
	boost::signals2::signal<void(const std::string&)> Close;
	boost::signals2::signal<void(double)> VolumeChange;
	boost::signals2::signal<void(bool)> MuteChange;
	boost::signals2::signal<void(const std::string&)> NameChange;

private:
	int parseMessage(const std::string &message, std::string &outData);

	void onMessage(const std::string &message);

	void processMessage(const std::string &message);

	void onClose();

private:
	TCPSession *session_;
	std::string buffer_;
};

class PlatformPal: public Platform {
public:
	PlatformPal();
	virtual ~PlatformPal();

	virtual void init(boost::asio::io_service &ioService);

	virtual void reset();

	virtual void setVolume(double volume);

	virtual void setMuted(bool muted);

protected:
	virtual void onSetVolume(double volume);

	virtual void onSetMuted(bool muted);

	virtual void onLaunchWebApp(const std::string &url);

	virtual void onStopWebApp(const std::string &url);

	virtual void onLaunchNativeApp(const std::string &app);

	virtual void onStopNativeApp(const std::string &app);

	virtual void onSessionConnect(TCPSession * session);

	virtual void onSessionClose(const std::string &addr);

	virtual void sendMessageToAllSessions(const std::string &message);

	virtual void onVolumeChange(double volume);

	virtual void onMuteChange(bool mute);

	virtual void onNameChange(const std::string &name);

protected:
	TCPServer * tcpServer_;
	std::list<TcpSessionWrapper *> sessions_;

//	bool pendingMuted_;
//	double pendingVolume_;
};

} /* namespace flint */
#endif /* PLATFORMPAL_H_ */
