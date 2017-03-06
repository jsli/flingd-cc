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

#include "WebsocketHandler.h"
#include "utils/Logging.h"

namespace flint {

WebsocketHandler::WebsocketHandler(WebsocketSession *session) {
	// TODO Auto-generated constructor stub
	session_ = session;
	session_->Close.connect(boost::bind(&WebsocketHandler::onClose, this));
	session_->Message.connect(
			boost::bind(&WebsocketHandler::onMessage, this, ::_1));
}

WebsocketHandler::~WebsocketHandler() {
	// TODO Auto-generated destructor stub
}

void WebsocketHandler::send(const std::string &message) {
	session_->send(message);
}

void WebsocketHandler::close() {
	session_->close();
}

void WebsocketHandler::onClose() {
	delete this;
}

} /* namespace flint */
