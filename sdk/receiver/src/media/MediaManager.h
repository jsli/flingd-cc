/*
 * MediaManager.h
 *
 *  Created on: 2015-1-6
 *      Author: manson
 */

#ifndef MEDIAMANAGER_H_
#define MEDIAMANAGER_H_

#include <string>

#include "FlintReceiverManager.h"
#include "ReceiverMessageBus.h"
#include "MediaPlayer.h"

namespace flint {

class MediaManager: public ReceiverMessageBus::IListener,
		public MediaPlayer::Callback {
public:
	MediaManager(const std::string &appId);
	virtual ~MediaManager();

	void open();

	void setMediaPlayer(MediaPlayer *mediaPlayer);

// extends from ReceiverMessageBus::IListener
public:
	virtual void onMessage(ReceiverMessageBus *bus, const std::string &message,
			const std::string &senderId);

	virtual void onSenderConnect(const std::string &senderId);

	virtual void onSenderDisconnect(const std::string &senderId);

// extends from IMediaPlayer::Callback
public:
	virtual void onLoaded();

	virtual void onSeeked();

	virtual void onPlaying();

	virtual void onBuffering();

	virtual void onPaused();

	virtual void onIdle();

	virtual void onEnded();

	virtual void onError();

	virtual void onVolumeChanged(double volume);

private:
	void doLoad(const rapidjson::Value& value, int requestId);

	void doPlay(int requestId);

	void doPause(int requestId);

	void doSeek(double position, int requestId);

	void doSetVolume(double volume, int requestId);

	void doGetStatus(int requestId);

	void postPlayerStatus();

	void postPlayerStatus(int requestId);

	void postPlayerStatus(const std::string &status);

	void postPlayerStatus(int requestId, const std::string &status);

	void send(const std::string &message);

protected:
	std::string appId_;
	FlintReceiverManager *manager_;
	ReceiverMessageBus * messageBus_;
	MediaPlayer *mediaPlayer_;
	MediaInfo *mediaInfo_;

	bool muted_;
	std::string playerState_;
	double volume_;
	double duration_;
	double currentTime_;

	int playbackRate_; // 1
	int supportedMediaCommands_; //15
	int mediaSessionId_; // 1

	int requestIdLoad_;
	int requestIdPlay_;
	int requestIdPause_;
	int requestIdSetVolume_;
	int requestIdSeek_;
};

} /* namespace flint */
#endif /* MEDIAMANAGER_H_ */
