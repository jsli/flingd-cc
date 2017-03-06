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

#include "boost/algorithm/string.hpp"
#include "boost/xpressive/xpressive_dynamic.hpp"
#include "boost/algorithm/string_regex.hpp"
#include "boost/regex.hpp"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "PeerServer.h"
#include "utils/Logging.h"
#include "utils/UUID.h"
#include "http/UrlParser.h"

namespace flint {

PeerServer::PeerServer(boost::asio::io_service &ioService, int httpPort) :
		WsServer(ioService, httpPort), timer_(ioService) {
	// TODO Auto-generated constructor stub
	setCleanupIntervals();
}

PeerServer::~PeerServer() {
	// TODO Auto-generated destructor stub
}

void PeerServer::onStart() {
	LOG_INFO << "PeerServer start!";
	WsServer::onStart();
}

void PeerServer::onStop() {
	LOG_INFO << "PeerServer stop!";
	timer_.cancel();

	WsServer::onStop();
}

void PeerServer::onHttpRequest(HTTPSession *session) {
	// ignore
	std::string resource = session->getResource();
	LOG_INFO << "PeerServer received HTTP request [" << resource << "]";

	std::string method = session->getMethod();
	if (boost::iequals(method, "GET")) {
		onGet(session);
	} else {
		LOG_ERROR << "unsupport method:" << method;
		response(session, websocketpp::http::status_code::bad_request);
	}
}

void PeerServer::onGet(HTTPSession *session) {
	try {
		Url url;
		UrlParser::parse(url, session->getResource());
		if (!url.validated) {
			return;
		}
		//	url.dump();
		std::string path = url.path;
		if (boost::iequals("/peerjs/id", path)
				|| boost::iequals("/peerjs/id/", path)) {
			std::string uuid = UUID::generateUUID();
			std::string id = uuid.substr(0, 8) + uuid.substr(uuid.length() - 8);
			std::map<std::string, std::string> headers;
			headers.insert(
					std::pair<std::string, std::string>("Content-Type",
							"text/html"));
			response(session, websocketpp::http::status_code::ok, headers, id);
		} else {
			LOG_WARN << "PeerServer - Invalidated path, unsupport path!!! "
					<< path;
			response(session, websocketpp::http::status_code::bad_request);
		}
	} catch (std::exception &e) {
		LOG_ERROR << "onGet error : " << e.what();
	}
}

void PeerServer::onWebsocketConnection(WebsocketSession *session) {
	LOG_ERROR << "PeerServer HANDLE WEBSOCKET [" << session->getResource()
			<< "]";
	Url url;
	UrlParser::parse(url, session->getResource());
	if (!url.validated) {
		return;
	}
	url.dump();
	std::string path = url.path;
	if (!boost::iequals("peerjs", path) && !boost::iequals("/peerjs", path)
			&& !boost::iequals("/peerjs/", path)) {
		LOG_WARN << "PeerServer - Invalidated path, unknow !!! " << path;
		return;
	}

	std::string query = url.query;
	std::string key = "";
	std::string id = "";
	std::string token = "";

	std::map<std::string, std::string> queries;
	UrlParser::parse_query(query, queries);
	key = UrlParser::get_value(queries, "key");
	id = UrlParser::get_value(queries, "id");
	token = UrlParser::get_value(queries, "token");

	LOG_INFO << "key = " << key << ", id = " << id << ", token = " << token;

	if (boost::iequals(key, "") || boost::iequals(id, "")
			|| boost::iequals(token, "")) {
		sendError(session, "No id, token, or key supplied to peerjs server");
		return;
	}

	Peer *p = getPeer(id);
	if (p == NULL) {
		std::string errMsg = "error";
		bool res = checkKey(key, errMsg);
		if (res) {
			p = new Peer(id, token, session);
			addPeer(id, p);

			send(session, "OPEN");
			configureWs(session, key, id, token);
		} else {
			sendError(session, errMsg);
		}
	} else {
		p->reset_session(session);
		configureWs(session, key, id, token);
	}
}

void PeerServer::configureWs(WebsocketSession *session, const std::string &key,
		const std::string &id, const std::string &token) {
	Peer *p = getPeer(id);
	if (p == NULL) {
		return;
	}

	if (!boost::iequals(p->get_token(), token)) {
		sendError(session, "ID-TAKEN", "ID is taken");
		return;
	}

	processOutstanding(key, id);

	p->Close.connect(boost::bind(&PeerServer::onPeerClose, this, ::_1));
	p->Message.connect(
			boost::bind(&PeerServer::onPeerMessage, this, ::_1, ::_2));
}

void PeerServer::processOutstanding(const std::string &key,
		const std::string &id) {
	LOG_INFO << "processOutstanding - " << key << " | " << id;
	std::vector<PeerMessage*> * messages = getPeerMessages(id);
	if (messages == NULL) {
		return;
	}

	std::vector<PeerMessage*>::iterator it = messages->begin();
	while (it != messages->end()) {
		handleTransmission(key, *it);
		++it;
	}
	removePeerMessages(id);
}

void PeerServer::handleTransmission(PeerMessage *pm) {
	handleTransmission("peerjs", pm);
}

void PeerServer::handleTransmission(const std::string &key, PeerMessage *pm) {
	LOG_INFO << "handleTransmission : ";
//	message->dump();
	std::string type = pm->get_type();
	std::string src = pm->get_src();
	std::string dst = pm->get_dst();
	std::string data = pm->get_data();

	Peer *p = getPeer(dst);
	if (p != NULL) {
		WebsocketSession *session = p->get_session();
		if (session != NULL) {
			try {
				LOG_INFO << "from [" << src << "] to [" << dst << " : " << data;
				session->send(data);
				delete pm;
			} catch (std::exception &e) {
				removePeer(dst);
				PeerMessage *errMsg = new PeerMessage("LEAVE", dst, src);
				handleTransmission(key, errMsg);
			}
		} else {
			LOG_ERROR << "Peer dead";
			removePeer(pm->get_dst());
			PeerMessage *errMsg = new PeerMessage("LEAVE", dst, src);
			handleTransmission(key, errMsg);
		}
	} else {
		if (!boost::iequals(type, "LEAVE") && !boost::iequals(type, "EXPIRE")
				&& !boost::iequals(dst, "")) {
			addPeerMessages(dst, pm);
		} else if (boost::iequals(type, "LEAVE") && boost::iequals(dst, "")) {
			removePeer(src);
		} else {
			// ignore
		}
	}
}

void PeerServer::setCleanupIntervals() {
	cleanupIntervals();
}

void PeerServer::cleanupIntervals() {
	timer_.expires_from_now(boost::posix_time::seconds(5));
	timer_.async_wait(
			boost::bind(&PeerServer::handleCleanupIntervals, this,
					boost::asio::placeholders::error));
}

void PeerServer::handleCleanupIntervals(
		const boost::system::error_code& error) {
	std::map<std::string/*dst*/, std::vector<PeerMessage*> *>::iterator it =
			outstandings_.begin();
	while (it != outstandings_.end()) {
		std::string dst = it->first;
		std::vector<PeerMessage*> *pending_vector = it->second;
		std::vector<PeerMessage*>::iterator _it = pending_vector->begin();
		while (_it != pending_vector->end()) {
			PeerMessage *pm = *_it;
			PeerMessage *_pm = new PeerMessage(pm->get_type(), pm->get_dst(),
					pm->get_src());
			handleTransmission(_pm);
			delete pm;
			++_it;
		}
		pending_vector->clear();
		++it;
		delete pending_vector;
	}
	outstandings_.clear();

	cleanupIntervals();
}

void PeerServer::onPeerClose(Peer *peer) {
	LOG_ERROR << "onPeerClose!!!" << peer->get_id();
	removePeer(peer->get_id());
}

void PeerServer::onPeerMessage(Peer *peer, const std::string &message) {
	LOG_ERROR << "onPeerMessage!!!" << message;
	try {
		rapidjson::Document d;
		d.Parse(message.c_str());
		rapidjson::Value& type = d["type"];
		if (type.IsNull()) {
			return;
		}
		std::string typeStr = type.GetString();
		if (boost::iequals(typeStr, "LEAVE")
				|| boost::iequals(typeStr, "CANDIDATE")
				|| boost::iequals(typeStr, "OFFER")
				|| boost::iequals(typeStr, "ANSWER")) {
			rapidjson::Value& dst = d["dst"];
			if (dst.IsNull()) {
				return;
			}
			rapidjson::Value& payload = d["payload"];
			if (payload.IsNull()) {
				return;
			}

			rapidjson::Value v(rapidjson::kStringType);
			v.SetString(peer->get_id().c_str(), d.GetAllocator());
			d.AddMember("src", v, d.GetAllocator());

			rapidjson::StringBuffer sb;
			rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
			d.Accept(writer);
			std::string data = sb.GetString();

			PeerMessage *pm = new PeerMessage(typeStr, peer->get_id(),
					dst.GetString(), data);
			handleTransmission(pm);
		} else {
			LOG_ERROR << "Message unrecognized " << typeStr;
		}
	} catch (std::exception &e) {
		LOG_ERROR << "onPeerMessage error: " << e.what();
	}
}

bool PeerServer::checkKey(const std::string &key, std::string &msg) {
	if (!boost::iequals("peerjs", key)) {
		msg = "Invalid key provided";
		LOG_ERROR << msg;
		return false;
	}

	if (peers_.size() > 5000) {
		msg = "Server has reached its concurrent user limit";
		LOG_ERROR << msg;
		return false;
	}

	return true;
}

bool PeerServer::removePeer(const std::string &id) {
	LOG_INFO << "remove 1 - peers size = " << peers_.size();
	std::map<std::string, Peer*>::iterator it = peers_.begin();
	while (it != peers_.end()) {
		if (boost::iequals(id, it->first)) {
			delete it->second;
			peers_.erase(it);
			LOG_INFO << "remove 2 - peers size = " << peers_.size();
			return true;
		}
		++it;
	}
	return false;
}

void PeerServer::addPeer(const std::string &id, Peer *p) {
	LOG_INFO << "add 1 - peers size = " << peers_.size();
	if (getPeer(id) == NULL) {
		peers_.insert(std::pair<std::string, Peer*>(id, p));
	}
	LOG_INFO << "add 2 - peers size = " << peers_.size();
}

Peer* PeerServer::getPeer(const std::string &id) {
	std::map<std::string, Peer*>::iterator it = peers_.begin();
	while (it != peers_.end()) {
		if (boost::iequals(id, it->first)) {
			return it->second;
		}
		++it;
	}
	return NULL;
}

bool PeerServer::removePeerMessages(const std::string &id) {
	LOG_INFO << "remove 1 - Pending Message Map size = "
			<< outstandings_.size();
	std::map<std::string, std::vector<PeerMessage*> *>::iterator it =
			outstandings_.begin();
	while (it != outstandings_.end()) {
		if (boost::iequals(id, it->first)) {
			delete it->second;
			outstandings_.erase(it);
			LOG_INFO << "erase pending message : " << id;
			LOG_INFO << "remove 2 - Pending Message Map size = "
					<< outstandings_.size();
			return true;
		}
		++it;
	}
	return false;
}

std::vector<PeerMessage*> * PeerServer::getPeerMessages(const std::string &id) {
	std::map<std::string, std::vector<PeerMessage*> *>::iterator it =
			outstandings_.begin();
	while (it != outstandings_.end()) {
		if (boost::iequals(id, it->first)) {
			return it->second;
		}
		++it;
	}
	return NULL;
}

void PeerServer::addPeerMessages(const std::string &id, PeerMessage *message) {
	std::vector<PeerMessage*> * messages = getPeerMessages(id);
	if (messages == NULL) {
		messages = new std::vector<PeerMessage*>();
		outstandings_.insert(
				std::pair<std::string, std::vector<PeerMessage*> *>(id,
						messages));
		LOG_INFO << "add - Pending Message Map size = " << outstandings_.size();
	} else {
		messages->push_back(message);
		LOG_INFO << "add - " << id << " has " << messages->size()
				<< " Pending Messages";
	}
}

std::string PeerServer::generateId() {
	std::string uuid = UUID::generateUUID();
	std::string first = uuid.substr(0, 8);
	std::string end = uuid.substr(uuid.length() - 8);
	std::string id = first + end;
	return id;
}

void PeerServer::send(WebsocketSession *session, const std::string &type) {
	rapidjson::StringBuffer s;
	rapidjson::Writer<rapidjson::StringBuffer> writer(s);
	writer.StartObject();
	writer.String("type");
	writer.String(type.c_str());
	writer.EndObject();
	session->send(s.GetString());
}

void PeerServer::sendError(WebsocketSession *session) {
	sendError(session, "ERROR");
}

void PeerServer::sendError(WebsocketSession *session,
		const std::string &errMsg) {
	sendError(session, "ERROR", errMsg);
}

void PeerServer::sendError(WebsocketSession *session,
		const std::string &errType, const std::string &errMsg) {
	rapidjson::StringBuffer s;
	rapidjson::Writer<rapidjson::StringBuffer> writer(s);
	writer.StartObject();
	writer.String("type");
	writer.String(errType.c_str());
	writer.String("payload");
	writer.StartObject();
	writer.String("msg");
	writer.String(errMsg.c_str());
	writer.EndObject();
	writer.EndObject();
	session->send(s.GetString());
	session->close();
}

} /* namespace flint */
