/*
 * MediaInfo.h
 *
 *  Created on: 2015-1-6
 *      Author: manson
 */

#ifndef MEDIAINFO_H_
#define MEDIAINFO_H_

#include <string>
#include <vector>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace flint {

class Image {
public:
	Image(int height, int width, const std::string &url) :
			height_(height), width_(width), url_(url) {
	}

	virtual ~Image() {
	}

	void log();

	static Image *create(const rapidjson::Value& value);

public:
	int getHeight() {
		return height_;
	}

	int getWidth() {
		return width_;
	}

	const std::string & getUrl() {
		return url_;
	}

private:
	int height_;
	int width_;
	std::string url_;
};

class Metadata {
public:
	Metadata(const std::string &studio, const std::string &title,
			const std::string &subtitle, int metadataType,
			std::vector<Image*> &images) :
			studio_(studio), title_(title), subtitle_(subtitle), metadataType_(
					metadataType), images_(images) {
	}

	virtual ~Metadata() {
		std::vector<Image*>::iterator it = images_.begin();
		while (it != images_.end()) {
			delete (*it);
			++it;
		}
	}

	void log();

	static Metadata *create(const rapidjson::Value& value);

public:
	const std::string &getStudio() {
		return studio_;
	}

	const std::string &getTitle() {
		return title_;
	}

	const std::string &getSubtitle() {
		return subtitle_;
	}

	int getMetadataType() {
		return metadataType_;
	}

	const std::vector<Image*> & getImages() {
		return images_;
	}

private:
	std::string studio_;
	std::string title_;
	std::string subtitle_;
	int metadataType_;
	std::vector<Image*> images_;
};

class MediaInfo {
public:
	MediaInfo(const std::string &streamType, int duration,
			const std::string &contentType, const std::string &contentId,
			Metadata *metadata) :
			streamType_(streamType), duration_(duration), contentType_(
					contentType), contentId_(contentId), metadata_(metadata) {

	}

	virtual ~MediaInfo() {
		delete metadata_;
	}

	void log();

	static MediaInfo *create(const rapidjson::Value& value);

public:
	const std::string & getStreamType() {
		return streamType_;
	}

	int getDuration() {
		return duration_;
	}

	const std::string & getContentType() {
		return contentType_;
	}

	const std::string & getContentId() {
		return contentId_;
	}

	Metadata * getMetadata() {
		return metadata_;
	}

private:
	std::string streamType_;
	int duration_;
	std::string contentType_;
	std::string contentId_; // media url
	Metadata *metadata_;
};

} /* namespace flint */
#endif /* MEDIAINFO_H_ */
