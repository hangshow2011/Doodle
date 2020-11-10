﻿#pragma once

#include "ftp_global.h"

#include "ftphandle.h"

#include <curl/curl.h>

class QFile;
class QUrl;

FTPSPACE_S
struct oFileInfo {
  dstring filepath;
  bool isFolder;
  time_t fileMtime;
  double fileSize;
};

class FTP_EXPORT ftpSession  {

 public:
  ~ftpSession() ;

  bool down(const dstring &localFile, const dstring &remoteFile);
  bool downFolder(const dstring &localFile, const dstring &remoteFile);
  bool upload(const dstring &localFile, const dstring &remoteFile);
  bool uploadFolder(const dstring &localFolder, const dstring &remoteFolder);
  oFileInfo fileInfo(const dstring &remoteFile);
  //获得文件列表
  std::vector<oFileInfo> list(const QString &remoteFolder);

  friend ftpSessionPtr ftphandle::session(const dstring &host,
                                          int prot,
                                          const dstring &name,
                                          const dstring &password);

 private:
  ftpSession();
  void setInfo(const dstring &host,
               int prot,
               const dstring &name,
               const dstring &password);
  static size_t writeFileCallbask(void *buff, size_t size, size_t nmemb, void *data);
  static size_t readFileCallbask(void *buff, size_t size, size_t nmemb, void *data);
  static size_t notCallbask(void *buff, size_t size, size_t nmemb, void *data);
  static size_t writeStringCallbask(void *ptr, size_t size, size_t nmemb, void *data);
  CURLcode perform();

 private:
  std::shared_ptr<QFile> outfile;
  std::shared_ptr<QFile> inputfile;
  std::shared_ptr<QUrl> ptrUrl;

  mutable CURL *curlSession;
};

FTPSPACE_E
