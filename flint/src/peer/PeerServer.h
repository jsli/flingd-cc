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

#ifndef PEERSERVER_H_
#define PEERSERVER_H_

#include <vector>
#include <map>

#include <boost/signals2.hpp>

#include "ws/WsServer.h"
#include "utils/Logging.h"

namespace flint {

class PeerMessage {
public:
	PeerMessage(const std::string &type, const std::string &src,
			const std::string &dst, const std::string &data = "") :
			type_(type), src_(src), dst_(dst), data_(data) {
	}

	virtual ~PeerMessage() {
	}

	inline const std::string & get_type() {
		return type_;
	}

	inline const std::string & get_src() {
		return src_;
	}

	inline const std::string & get_dst() {
		return dst_;
	}

	inline const std::string & get_data() {
		return data_;
	}

	void dump() {
		LOG_INFO << "type = " << type_ << ", src = " << src_ << ", dst = "
				<< dst_ << " ,data = " << data_;
	}

private:
	std::string type_;
	std::string src_;
	std::string dst_;
	std::string data_;
};

class Peer {
public:
	Peer(const std::string &id, const std::string &token,
			WebsocketSession *session) :
			id_(id), token_(token), session_(session) {
		init_session();
	}

	virtual ~Peer() {
		LOG_WARN << "~Peer() --- destroy";
	}

public:
	inline void send(const std::string &message) {
		if (session_ != NULL) {
			session_->send(message);
		}
	}

	inline const std::string & get_id() {
		return id_;
	}

	inline const std::string & get_token() {
		return token_;
	}

	inline WebsocketSession * get_session() {
		return session_;
	}

	inline void reset_session(WebsocketSession *session) {
		if (session_ != NULL) {
			session_->close();
		}
		session_ = session;
		init_session();
	}

protected:
	virtual void onClose() {
		Close(this);
	}

	virtual void onMessage(const std::string &msg) {
		Message(this, msg);
	}

private:
	void init_session() {
		if (session_ != NULL) {
			session_->Close.connect(boost::bind(&Peer::onClose, this));
			session_->Message.connect(
					boost::bind(&Peer::onMessage, this, ::_1));
		}
	}

public:
	boost::signals2::signal<void(Peer *)> Close;
	boost::signals2::signal<void(Peer *, const std::string &)> Message;

private:
	std::string id_;
	std::string token_;
	WebsocketSession * session_;
};

class PeerServer: public WsServer {
public:
	PeerServer(boost::asio::io_service &ioService, int httpPort);
	virtual ~PeerServer();

protected:
	virtual void onStart();

	virtual void onStop();

	virtual void onHttpRequest(HTTPSession *session);

	virtual void onWebsocketConnection(WebsocketSession *websocketSession);

private:
	void onGet(HTTPSession *session);

	void handle(WebsocketSession *session);

	bool checkKey(const std::string &key, std::string &msg);

	void configureWs(WebsocketSession *session, const std::string &key,
			const std::string &id, const std::string &token);

	void processOutstanding(const std::string &key, const std::string &id);

	void handleTransmission(PeerMessage *pm);

	void handleTransmission(const std::string &key, PeerMessage *pm);

	void onPeerClose(Peer *peer);

	void onPeerMessage(Peer *peer, const std::string &message);

	bool removePeer(const std::string &id);

	Peer* getPeer(const std::string &id);

	void addPeer(const std::string &id, Peer *p);

	bool removePeerMessages(const std::string &id);

	std::vector<PeerMessage*> * getPeerMessages(const std::string &id);

	void addPeerMessages(const std::string &id, PeerMessage *message);

	std::string generateId();

	void send(WebsocketSession *session, const std::string &type);

	void sendError(WebsocketSession *session);

	void sendError(WebsocketSession *session, const std::string &errMsg);

	void sendError(WebsocketSession *session, const std::string &errType,
			const std::string &errMsg);

	void setCleanupIntervals();

	void cleanupIntervals();

	void handleCleanupIntervals(const boost::system::error_code& error);

private:
	std::map<std::string/*id*/, Peer*> peers_;
	std::map<std::string/*dst*/, std::vector<PeerMessage*> *> outstandings_;
	boost::asio::deadline_timer timer_;

};

} /* namespace flint */
#endif /* PEERSERVER_H_ */
