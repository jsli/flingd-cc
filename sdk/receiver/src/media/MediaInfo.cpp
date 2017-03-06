/*
 * MediaInfo.cpp
 *
 *  Created on: 2015-1-6
 *      Author: manson
 */

#include "MediaInfo.h"
#include "utils/Logging.h"

namespace flint {

Image * Image::create(const rapidjson::Value& value) {
	if (value.IsNull() || value["url"].IsNull() || value["height"].IsNull()
			|| value["width"].IsNull()) {
		return NULL;
	}

	return new Image(value["height"].GetInt(), value["width"].GetInt(),
			value["url"].GetString());
}

void Image::log() {
	LOG_INFO << "[Image] : width=" << width_ << ", height=" << height_
			<< ", url=" << url_;
}

Metadata * Metadata::create(const rapidjson::Value& value) {
	if (value.IsNull() || value["studio"].IsNull() || value["title"].IsNull()
			|| value["subtitle"].IsNull() || value["metadataType"].IsNull()
			|| value["images"].IsNull()) {
		return NULL;
	}

	std::string studio = value["studio"].GetString();
	std::string title = value["title"].GetString();
	std::string subtitle = value["subtitle"].GetString();
	int metadataType = value["metadataType"].GetInt();
	std::vector<Image*> images;
	if (value["images"].IsArray() && value["images"].Size() > 0) {
		rapidjson::Value::ConstValueIterator it = value["images"].Begin();
		while (it != value["images"].End()) {
			Image* image = Image::create(*it);
			images.push_back(image);
			++it;
		}
	}
	return new Metadata(studio, title, subtitle, metadataType, images);
}

void Metadata::log() {
	LOG_INFO << "[Metadata] : studio=" << studio_ << ", title=" << title_
			<< ", subtitle=" << subtitle_ << ", metadataType=" << metadataType_;
	std::vector<Image*>::iterator it = images_.begin();
	while (it != images_.end()) {
		(*it)->log();
		++it;
	}
}

MediaInfo * MediaInfo::create(const rapidjson::Value& value) {
	if (value.IsNull() || value["streamType"].IsNull()
			|| value["duration"].IsNull() || value["contentType"].IsNull()
			|| value["contentId"].IsNull() || value["metadata"].IsNull()) {
		return NULL;
	}

	std::string streamType = value["streamType"].GetString();
	int duration = value["duration"].GetInt();
	std::string contentType = value["contentType"].GetString();
	std::string contentId = value["contentId"].GetString();
	Metadata *metadata = Metadata::create(value["metadata"]);

	return new MediaInfo(streamType, duration, contentType, contentId, metadata);
}

void MediaInfo::log() {
	LOG_INFO << "[MediaInfo] : streamType=" << streamType_ << ", duration="
			<< duration_ << ", contentType=" << contentType_ << ", contentId="
			<< contentId_;
	if (metadata_ != NULL) {
		metadata_->log();
	}
}

} /* namespace flint */
