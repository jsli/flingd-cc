/*
 * MessageChannel.h
 *
 *  Created on: 2014-12-21
 *      Author: manson
 */

#ifndef MESSAGECHANNEL_H_
#define MESSAGECHANNEL_H_

#include <string>
#include <vector>

#include <boost/signals2.hpp>

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

namespace flint {

class MessageChannel {
public:
	typedef websocketpp::client<websocketpp::config::asio_client> client;

public:
	MessageChannel(const std::string &name);
	virtual ~MessageChannel();

	void open(const std::string & url, boost::asio::io_service *ioService);

	void open(const std::string & url);

	void close();

	void send(const std::string &message);

	void send(const std::string &message, const std::string &senderId);

	bool isOpened();

	std::string const & getChannelName();

	std::string const & getUrl();

public:
	boost::signals2::signal<void(MessageChannel *)> Open;
	boost::signals2::signal<void(MessageChannel *)> Close;
	boost::signals2::signal<void(MessageChannel *)> Failed;

private:
	void onOpen(websocketpp::connection_hdl);

	void onClose(websocketpp::connection_hdl);

	void onFail(websocketpp::connection_hdl);

	void onWsMessage(websocketpp::connection_hdl hdl,
			websocketpp::config::asio_client::message_type::ptr message);

	void closeImpl();

	void sendImpl(const std::string & message);

protected:
	virtual void onMessage(const std::string &message) = 0;

private:
	static int CONNECTING; // 0, The connection is not yet open.
	static int OPEN; // 1, The connection is open and ready to communicate.
	static int CLOSING; // 2, The connection is in the process of closing.
	static int CLOSED; // 3, The connection is closed or couldn't be opened.

	bool opened_;
	std::string channelName_;
	std::string url_;

	websocketpp::client<websocketpp::config::asio_client> client_;
	websocketpp::connection_hdl hdl_;

	std::vector<std::string> penddingMessages_;
};

} /* namespace flint */
#endif /* MESSAGECHANNEL_H_ */
