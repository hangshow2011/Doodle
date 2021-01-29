﻿#pragma once

#include "pinyinlib/pinyin_global.h"

PINYIN_NAMESPACE_S

class PINYIN_EXPORT convert {
 public:
  std::string toEn(const std::string &conStr);
  static convert &Get() noexcept;

  convert &operator=(const convert &) = delete;
  convert(const convert &)            = delete;

 private:
  convert();
  ~convert();

 private:
  std::vector<std::string> p_list;
};

DNAMESPACE_E
