﻿//
// Created by teXiao on 2020/10/14.
//
#pragma once

#include "doodle_global.h"
#include "core_global.h"

#include <QListView>
#include <QAbstractListModel>


#include "doodle_global.h"
#include "core_global.h"

DOODLE_NAMESPACE_S
class fileClassShotModel : public QAbstractListModel {
 Q_OBJECT
 private:
  doCore::fileClassPtrList list_fileClass;
  doCore::shotPtr p_shot;

 public:
  explicit fileClassShotModel(QObject *parent = nullptr);
  ~fileClassShotModel() override = default;;

  //返回总行数
  int rowCount(const QModelIndex &parent) const override;
  //返回数据
  QVariant data(const QModelIndex &index, int role) const override;
  [[nodiscard]] doCore::fileClassPtr dataRow(const QModelIndex &index) const;
  //返回标题
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

  //返回设置标识
  Qt::ItemFlags flags(const QModelIndex &index) const override;
  //设置数据
  bool setData(const QModelIndex &index, const QVariant &value, int role) override;

  //添加和删除数据
  bool insertRows(int position, int rows, const QModelIndex &index) override;
  bool removeRows(int position, int rows, const QModelIndex &index) override;

 public slots:
  void init(const doCore::shotPtr &shot);
  void clear();
};
DOODLE_NAMESPACE_E