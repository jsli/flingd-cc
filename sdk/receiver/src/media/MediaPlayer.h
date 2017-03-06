/*
 * MediaPlayer.h
 *
 *  Created on: 2015-1-9
 *      Author: manson
 */

#ifndef MEDIAPLAYER_H_
#define MEDIAPLAYER_H_

#include "IMediaPlayer.h"

namespace flint {

class MediaPlayer: public IMediaPlayer {
public:
	MediaPlayer();
	virtual ~MediaPlayer();

public:
	void setCallback(IMediaPlayer::Callback *callback) {
		callback_ = callback;
	}

protected:
	IMediaPlayer::Callback *callback_;

};

} /* namespace flint */
#endif /* MEDIAPLAYER_H_ */
