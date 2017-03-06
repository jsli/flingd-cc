/*
 * IMediaPlayer.h
 *
 *  Created on: 2015-1-6
 *      Author: manson
 */

#ifndef IMEDIAPLAYER_H_
#define IMEDIAPLAYER_H_

#include <string>

#include "MediaInfo.h"

namespace flint {

class IMediaPlayer {

// Callbacks
public:
	class Callback {
	public:
		virtual void onReady() {
		}

		virtual void onLoaded() {
		}

		virtual void onSeeked() {
		}

		virtual void onPlaying() {
		}

		virtual void onBuffering() {
		}

		virtual void onPaused() {
		}

		virtual void onStopped() {
		}

		virtual void onIdle() {
		}

		virtual void onEnded() {
		}

		virtual void onError() {
		}

		virtual void onVolumeChanged(double volume) {
		}
	};

public:
	IMediaPlayer() {
	}

	virtual ~IMediaPlayer() {
	}

public:
	virtual void load(const std::string &uri) = 0;

	virtual void play() = 0;

	virtual void pause() = 0;

	virtual void stop() = 0;

	virtual void setVolume(double volume) = 0;

	virtual double getVolume() = 0;

	virtual void setMute(bool mute) = 0;

	virtual bool getMute() = 0;

	virtual void seek(double position) = 0;

	virtual const std::string & getStatus() = 0;

	virtual double getCurrentTime() = 0;

	virtual double getDuration() = 0;
};

} /* namespace flint */
#endif /* IMEDIAPLAYER_H_ */
