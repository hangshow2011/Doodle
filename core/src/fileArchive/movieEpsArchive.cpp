﻿/*
 * @Author: your name
 * @Date: 2020-11-23 17:47:24
 * @LastEditTime: 2020-12-02 13:35:03
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \Doodlef:\source\qt_test\doodle\core\src\movieepsarchive.cpp
 */
//
// Created by teXiao on 2020/11/23.
//

#include "movieEpsArchive.h"
#include <src/shots/shotfilesqlinfo.h>
#include <src/shots/shot.h>
#include <src/shots/shottype.h>
#include <src/core/coreDataManager.h>
#include <src/exeWrap/ffmpegWrap.h>
CORE_NAMESPACE_S
movieEpsArchive::movieEpsArchive(shotInfoPtr eps)
    : fileArchive(),
      p_info_ptr_(std::move(eps)) {
}
void movieEpsArchive::insertDB() {
  p_info_ptr_->setFileList(p_Path);
  if (p_info_ptr_->getInfoP().empty())
    p_info_ptr_->setInfoP("整集拍屏文件");
  p_info_ptr_->insert();
}
void movieEpsArchive::_generateFilePath() {
  if (!p_soureFile.empty()) {
    p_Path.push_back(p_info_ptr_->generatePath("movie", ".mp4").generic_string());
  } else if (!p_info_ptr_->getFileList().empty()) {
    p_Path.push_back(p_info_ptr_->getFileList()[0].generic_string());
  }
}
bool movieEpsArchive::epsMove() {
  p_Path.push_back(p_info_ptr_->generatePath("movie", ".mp4").generic_string());
  generateCachePath();
  p_soureFile = p_Path;
  p_Path.clear();
  if (p_info_ptr_->getEpisdes()) {
    shotInfoPtrList list{};
    for (const auto &item : coreDataManager::get().getShotL()) {
      auto info = shotFileSqlInfo::getAll(item, shotType::findShotType("flipbook"));
      if (info.front()) {
        p_Path.push_back(info.front()->getFileList().front());
      }
    }
    _down(p_cacheFilePath.front().parent_path());
    dpathList pathlist{};
    for (const auto &path : p_Path) {
      auto path_mov = p_cacheFilePath.front().parent_path() / path.filename();
      if (boost::filesystem::exists(path_mov))
        pathlist.push_back(path_mov);
    }
    auto ffmpeg = std::make_unique<ffmpegWrap>();
    return ffmpeg->connectVideo(pathlist, p_cacheFilePath.front());
  } else {
    return false;
  }
}
bool movieEpsArchive::update() {
  p_info_ptr_->setShotType(shotType::findShotType("flipbook"));
  if (epsMove()) {
    p_soureFile = {p_cacheFilePath.front()};
    p_Path.clear();
    return fileArchive::update(p_soureFile);
  } else {
    return false;
  }
}

CORE_NAMESPACE_E