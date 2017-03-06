/*
 * MediaManager.cpp
 *
 *  Created on: 2015-1-6
 *      Author: manson
 */

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include "MediaManager.h"
#include "MediaInfo.h"
#include "utils/Logging.h"

namespace flint {

static std::string NAMESPACE = "urn:flint:org.openflint.fling.media";

MediaManager::MediaManager(const std::string &appId) :
		ReceiverMessageBus::IListener(), appId_(appId), manager_(NULL), messageBus_(
				NULL), mediaPlayer_(NULL), mediaInfo_(NULL), playerState_(
				"IDLE"), volume_(0), playbackRate_(1), supportedMediaCommands_(
				15), mediaSessionId_(1) {
	// TODO Auto-generated constructor stub
	manager_ = new flint::FlintReceiverManager(appId_);

	messageBus_ = manager_->createMessageBus(NAMESPACE);
	messageBus_->setListener(this);
}

MediaManager::~MediaManager() {
	// TODO Auto-generated destructor stub
}

void MediaManager::open() {
	manager_->open();
}

void MediaManager::setMediaPlayer(MediaPlayer *mediaPlayer) {
	mediaPlayer_ = mediaPlayer;
	if (mediaPlayer_ != NULL) {
		mediaPlayer_->setCallback(this);
	}
}

void MediaManager::send(const std::string &message) {
	messageBus_->send(message);
}

void MediaManager::onLoaded() {
	postPlayerStatus(requestIdLoad_, "PLAYING");
	requestIdLoad_ = 0;
}

void MediaManager::onPlaying() {
	postPlayerStatus(requestIdPlay_, "PLAYING");
	requestIdPlay_ = 0;
}

void MediaManager::onBuffering() {
	postPlayerStatus("BUFFERING");
}

void MediaManager::onPaused() {
	postPlayerStatus(requestIdPause_, "PAUSED");
	requestIdPause_ = 0;
}

void MediaManager::onIdle() {
	postPlayerStatus("IDLE");
}

void MediaManager::onEnded() {
	postPlayerStatus("IDLE");
}

void MediaManager::onError() {
	postPlayerStatus("IDLE");
}

void MediaManager::onSeeked() {
	postPlayerStatus(requestIdSeek_, "PLAYING");
	requestIdSeek_ = 0;
}

void MediaManager::onVolumeChanged(double volume) {
	postPlayerStatus(requestIdSetVolume_, "PAUSED");
	requestIdSetVolume_ = 0;
}

void MediaManager::doLoad(const rapidjson::Value& value, int requestId) {
	MediaInfo *mediaInfo = MediaInfo::create(value);
	if (mediaInfo != NULL) {
		mediaInfo_ = mediaInfo;
		if (mediaPlayer_ != NULL) {
			requestIdLoad_ = requestId;
			mediaPlayer_->load(mediaInfo->getContentId());
		}
	} else {
		LOG_ERROR << "cannot create MediaInfo: " << value.GetString();
	}
}

void MediaManager::doPlay(int requestId) {
	if (mediaPlayer_ != NULL) {
		requestIdPlay_ = requestId;
		mediaPlayer_->play();
	}
}

void MediaManager::doPause(int requestId) {
	if (mediaPlayer_ != NULL) {
		requestIdPause_ = requestId;
		mediaPlayer_->pause();
	}
}

void MediaManager::doSeek(double position, int requestId) {
	LOG_DEBUG << "seek to ------------------> " << position;
	if (mediaPlayer_ != NULL) {
		requestIdSeek_ = requestId;
		mediaPlayer_->seek(position);
	}
}

void MediaManager::doSetVolume(double volume, int requestId) {
	LOG_DEBUG << "set volume to ------------------> " << volume;
	if (mediaPlayer_ != NULL) {
		requestIdSetVolume_ = requestId;
		mediaPlayer_->setVolume(volume);
	}
}

void MediaManager::doGetStatus(int requestId) {
	postPlayerStatus(requestId);
}

void MediaManager::postPlayerStatus() {
	postPlayerStatus(0, "");
}

void MediaManager::postPlayerStatus(int requestId) {
	postPlayerStatus(requestId, "");
}

void MediaManager::postPlayerStatus(const std::string &status) {
	postPlayerStatus(0, status);
}

void MediaManager::postPlayerStatus(int requestId, const std::string &status) {
	if (mediaPlayer_ != NULL) {
		duration_ = mediaPlayer_->getDuration();
		currentTime_ = mediaPlayer_->getCurrentTime();
		volume_ = mediaPlayer_->getVolume();
		muted_ = mediaPlayer_->getMute();
		if (status == "") {
			playerState_ = mediaPlayer_->getStatus();
		} else {
			playerState_ = status;
		}

		rapidjson::StringBuffer s;
		rapidjson::Writer<rapidjson::StringBuffer> writer(s);
		writer.StartObject();

		// type
		writer.String("type");
		writer.String("MEDIA_STATUS");

		// status array, object
		writer.String("status");
		writer.StartArray();
		writer.StartObject();

		// fake data
		writer.String("mediaSessionId");
		writer.Int(mediaSessionId_);
		writer.String("playbackRate");
		writer.Int(playbackRate_);
		writer.String("supportedMediaCommands");
		writer.Int(supportedMediaCommands_);

		// currentTime
		writer.String("currentTime");
		writer.Double(currentTime_);

		// duration
		writer.String("duration");
		writer.Double(duration_);

		// volume object
		writer.String("volume");
		writer.StartObject();
		writer.String("level");
		writer.Double(volume_);
		writer.String("muted");
		writer.Bool(muted_);
		writer.EndObject();

		// player status
		writer.String("playerState");
		writer.String(playerState_.c_str());

		// TODO: idle reason

		// media object
		if (mediaInfo_ != NULL) {
			writer.String("media");
			writer.StartObject();

			// streamType
			std::string streamType = mediaInfo_->getStreamType();
			if (streamType.size() > 0) {
				writer.String("streamType");
				writer.String(streamType.c_str());
			}

			// duration
			writer.String("duration");
			writer.Double(duration_);

			// contentType
			std::string contentType = mediaInfo_->getContentType();
			if (contentType.size() > 0) {
				writer.String("contentType");
				writer.String(contentType.c_str());
			}

			// contentId
			std::string contentId = mediaInfo_->getContentId();
			if (contentId.size() > 0) {
				writer.String("contentId");
				writer.String(contentId.c_str());
			}

			// metadata object
			Metadata *metadata = mediaInfo_->getMetadata();
			if (metadata != NULL) {
				// title
				std::string title = metadata->getTitle();
				if (title.size() > 0) {
					writer.String("title");
					writer.String(title.c_str());
				}

				// subtitle
				std::string subtitle = metadata->getSubtitle();
				if (subtitle.size() > 0) {
					writer.String("subtitle");
					writer.String(subtitle.c_str());
				}

				// images array, object
				std::vector<Image*> images = metadata->getImages();
				if (images.size() > 0) {
					writer.String("images");
					writer.StartArray();
					std::vector<Image*>::iterator it = images.begin();
					while (it != images.end()) {
						Image *image = *it;
						if (image != NULL) {
							std::string url = image->getUrl();
							if (url.size() > 0) {
								writer.StartObject();
								// height
								writer.String("height");
								writer.Int(image->getHeight());
								// width
								writer.String("width");
								writer.Int(image->getWidth());
								// url
								writer.String("url");
								writer.String(url.c_str());
								writer.EndObject();
							}
						}
						++it;
					}
					writer.EndArray();
				}

				// metadataType
				writer.String("metadataType");
				writer.Int(metadata->getMetadataType());
			}

			writer.String("metadata");
			writer.StartObject();
			writer.EndObject();
			writer.EndObject();
		}

		writer.EndObject();
		writer.EndArray();

		writer.String("requestId");
		writer.Int(requestId);

		writer.EndObject();

		send(s.GetString());
	}
}

void MediaManager::onMessage(ReceiverMessageBus *bus,
		const std::string &message, const std::string &senderId) {
//	LOG_DEBUG << "media manager received: [" << message << "]";
	rapidjson::Document d;
	d.Parse(message.c_str());
	if (d.IsNull()) {
		return;
	}

	rapidjson::Value& requestId = d["requestId"];
	if (requestId.IsNull() && requestId.IsInt()) {
		return;
	}
	int requestIdInt = requestId.GetInt();

	rapidjson::Value& type = d["type"];
	if (type.IsNull() && type.IsString()) {
		return;
	}
	std::string typeStr = type.GetString();
	if (typeStr == "LOAD") {
		rapidjson::Value& currentTime = d["currentTime"];
		double currentTimeDouble = 0;
		if (!currentTime.IsNull() && currentTime.IsDouble()) {
			currentTime_ = currentTime.GetDouble();
		}
		doLoad(d["media"], requestIdInt);
	} else if (typeStr == "PAUSE") {
		doPause(requestIdInt);
	} else if (typeStr == "PLAY") {
		doPlay(requestIdInt);
	} else if (typeStr == "SET_VOLUME") {
		rapidjson::Value& volume = d["volume"];
		if (!volume.IsNull() && volume.IsObject()) {
			rapidjson::Value& level = volume["level"];
			if (!level.IsNull() && level.IsDouble()) {
				double volumeDouble = level.GetDouble();
				if (volumeDouble >= 0 && volumeDouble <= 1) {
					doSetVolume(volumeDouble, requestIdInt);
				}
			}
		}
	} else if (typeStr == "SEEK") {
		rapidjson::Value& currentTime = d["currentTime"];
		if (!currentTime.IsNull() && currentTime.IsDouble()) {
			doSeek(currentTime.GetDouble(), requestIdInt);
		}
	} else if (typeStr == "GET_STATUS") {
		doGetStatus(requestIdInt);
	} else {
		LOG_ERROR << "MediaManager received unknow message: " << type;
	}
}

void MediaManager::onSenderConnect(const std::string &senderId) {
	// TODO: ignore
}

void MediaManager::onSenderDisconnect(const std::string &senderId) {
	// TODO: ignore
}

} /* namespace flint */
