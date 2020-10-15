﻿#pragma once

#include "core_global.h"

#include "coresqldata.h"

CORE_NAMESPACE_S

class CORE_EXPORT assType : public coresqldata {
 Q_GADGET
 public:
  assType();
  void select(const qint64 &ID_);

  void insert() override;
  void updateSQL() override;
  void deleteSQL() override;

  static assTypePtrList getAll(const fileClassPtr &fc_);

  fileClassPtr getFileClass();
  void setFileClass(const fileClassPtrW &value);

  QString getAssType() const;
  QString getAssType(const bool &isZNCH);

  void setAssType(const QString &value);
  void setAssType(const QString &value, const bool &isZNCH);

 private:
  static assTypePtrList batchQuerySelect(sqlQuertPtr &query);

 private:
  QString name;

  qint64 __file_class__;
  fileClassPtrW p_tprw_fileClass;

  znchNamePtr p_ptr_znch;
};

CORE_DNAMESPACE_E

Q_DECLARE_METATYPE(doCore::assTypePtr)