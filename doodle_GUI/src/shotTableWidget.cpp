﻿//
// Created by teXiao on 2020/10/13.
//

#include <QJsonArray>
#include <QMenu>
#include <QContextMenuEvent>
#include <QHeaderView>

#include "src/mayaArchive.h"
#include "src/mayaArchiveShotFbx.h"
#include "src/coreset.h"
#include "src/filetype.h"
#include "src/shotfilesqlinfo.h"

#include "Logger.h"

#include "shotTableModel.h"
#include "shotTableWidget.h"

#include <QMimeData>
#include <QFileDialog>
#include <QMessageBox>
#include <QClipboard>
#include <QGuiApplication>
#include <QProgressDialog>

#include <boost/process.hpp>

#include <memory>
#include <thread>

DOODLE_NAMESPACE_S
//-----------------------------------自定义shot小部件---------------------------------------------//
shotTableWidget::shotTableWidget(QWidget *parent)
    : QTableView(parent),
      p_model_(),
      p_menu_(),
      p_type_ptr_(),
      exportList(),
      movieList() {
  setSelectionBehavior(QAbstractItemView::SelectRows);//行选
  setSelectionMode(QAbstractItemView::SingleSelection);//单选
  setAcceptDrops(true);
}
void shotTableWidget::init(const doCore::fileTypePtr &file_type_ptr) {
  p_type_ptr_ = file_type_ptr;
  p_model_->init(file_type_ptr);
  horizontalHeader()->setVisible(true);
  horizontalHeader()->setMinimumHeight(20);
  horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
}
void shotTableWidget::contextMenuEvent(QContextMenuEvent *event) {
  if (!p_menu_)
    p_menu_ = new QMenu(this);
  p_menu_->clear();
  if (p_type_ptr_) {
    //上传文件
    auto k_sub_file = new QAction();
    connect(k_sub_file, &QAction::triggered,
            this, &shotTableWidget::getSelectPath);
    k_sub_file->setText(tr("提交文件"));
    k_sub_file->setStatusTip(tr("提交所需要的文件"));
    p_menu_->addAction(k_sub_file);
    //上传拍屏
    auto k_sub_fb = new QAction();
    connect(k_sub_fb, &QAction::triggered,
            this, &shotTableWidget::createFlipbook_slot);
    k_sub_fb->setText(tr("提交拍屏"));
    p_menu_->addAction(k_sub_fb);

    if (selectionModel()->hasSelection()) {
      auto index = p_model_->index(selectionModel()->currentIndex().row(), 4);
      //打开文件位置
      auto k_openFile = new QAction();
      k_openFile->setText(tr("打开文件所在位置"));
      connect(k_openFile, &QAction::triggered,
              this, [=] {
            openPath(p_model_->data(index, Qt::UserRole)
                         .value<doCore::shotInfoPtr>(), true);
          });
      p_menu_->addAction(k_openFile);


      //导出fbx脚本
      auto k_exportFbx = new QAction();
      k_exportFbx->setText(tr("导出fbx文件"));
      connect(k_exportFbx, &QAction::triggered,
              this, &shotTableWidget::exportFbx);
      p_menu_->addAction(k_exportFbx);
      //复制文件目录
    }
  }

  p_menu_->popup(event->globalPos());
}
void shotTableWidget::clear() {
  p_type_ptr_ = nullptr;
  p_model_->clear();
}
void shotTableWidget::setModel(QAbstractItemModel *model) {
  auto p_model = dynamic_cast<shotTableModel *>(model);
  if (p_model)
    p_model_ = p_model;
  QTableView::setModel(model);
}


void shotTableWidget::dragEnterEvent(QDragEnterEvent *event) {
  if (event->mimeData()->hasUrls()) {
    event->acceptProposedAction();
    enableBorder(true);
  } else
    event->ignore();
//  QAbstractItemView::dragEnterEvent(event);
}
void shotTableWidget::dragLeaveEvent(QDragLeaveEvent *event) {
  enableBorder(false);
//  QAbstractItemView::dragLeaveEvent(event);
}
void shotTableWidget::dragMoveEvent(QDragMoveEvent *event) {
//  QAbstractItemView::dragMoveEvent(event);
}
void shotTableWidget::dropEvent(QDropEvent *event) {
  QAbstractItemView::dropEvent(event);
  if (!event->mimeData()->hasUrls()) return enableBorder(false);
  if (event->mimeData()->urls().size() != 1) return enableBorder(false);
  auto url = event->mimeData()->urls()[0];
  DOODLE_LOG_INFO << "文件拖入窗口" << url;

  const QFileInfo &kFileInfo = QFileInfo(url.toLocalFile());
  if (kFileInfo.isFile()) {
    if (kFileInfo.suffix() == "ma" || kFileInfo.suffix() == "mb")
      insertShot(url.toLocalFile());
    else if(kFileInfo.suffix() == "mp4" || kFileInfo.suffix() == "avi" || kFileInfo.suffix() == "mov")
      createFlipbook(url.toLocalFile());
  }else if (kFileInfo.isDir()){
    auto dir = QDir(url.toLocalFile());
    auto image_list = dir.entryInfoList({"*.png", "*.jpg", "*.tga", "*.exr"}, QDir::Files, QDir::Name);
    if (!image_list.isEmpty()) 
      createFlipbook(url.toLocalFile());
  }

  enableBorder(false);
}
void shotTableWidget::enableBorder(const bool &isEnable) {
  if (isEnable)
    setStyleSheet("border:3px solid #165E23");
  else
    setStyleSheet("");
}


void shotTableWidget::getSelectPath() {
  auto path = QFileDialog::getOpenFileName(this,
                                           tr("提交文件"),
                                           QString(),
                                           "files (*.mb *.ma *.uproject *.max *.fbx *.png *.tga *.jpg))");
  insertShot(path);
}
void shotTableWidget::insertShot(const QString &path) {
  DOODLE_LOG_INFO << "提交文件";
  if (path.isEmpty()) return;
  //获得最大版本
  auto version = p_model_->data(p_model_->index(0, 0), Qt::UserRole)
      .toInt();
  //插入新的数据
  p_model_->insertRow(0, QModelIndex());
  auto data = p_model_->data(p_model_->index(0, 4), Qt::UserRole)
      .value<doCore::shotInfoPtr>();
  data->setVersionP(version + 1);
  //创建上传类
  auto file = std::make_shared<doCore::mayaArchive>(data);
  if (path.isEmpty()) return;
  //开始上传
  file->update(path);
  //更新列表
  p_model_->init(p_type_ptr_);

}
void shotTableWidget::openPath(const doCore::fileSqlInfoPtr &info_ptr, const bool &openEx) {
  auto path = doCore::coreSet::getCoreSet().getPrjectRoot().path() + info_ptr->getFileList()[0].path();
  DOODLE_LOG_INFO << "打开路径: " << QDir::cleanPath(path);

  if (QDir(path).exists()) {
    if (openEx)
      boost::process::system(QString("explorer.exe %1").arg(QDir::cleanPath(path).replace("/", "\\")).toStdString());
//      std::system(QString("explorer.exe %1").arg(QDir::cleanPath(path).replace("/", "\\"))
//                      .toStdString().c_str());
    else
      QGuiApplication::clipboard()->setText(path);
  } else {
    DOODLE_LOG_CRIT << QString("没有在服务器中找到目录:\n %1").arg(path);
    QMessageBox::warning(this, tr("没有目录"),
                         tr("没有在服务器中找到目录:\n %1").arg(path),
                         QMessageBox::Yes);
  }
}
void shotTableWidget::exportFbx() {
  //迭代检查导出是否成功
  bool is_success = true;
  for (auto &&item :exportList) {
    if (item->isState() == doCore::fileArchive::state::fail) {
      auto info = item->getInfo();
      QMessageBox::warning(this, "导出文件失败", QString("集数: %1, 镜头: %2")
          .arg(info["episodes"])
          .arg(info["shot"]));
      is_success &= false;
    }
  }
  if (is_success) exportList.clear();
  //是否选择导出物体
  if (!selectionModel()->hasSelection()) return;
  //获得选择数据
  auto index = p_model_->index(selectionModel()->currentIndex().row(), 4);

  auto data = p_model_->data(index, Qt::UserRole).value<doCore::shotInfoPtr>();
  if (!data) return;

  p_model_->insertRow(0, QModelIndex());

  auto export_data = p_model_->data(p_model_->index(0, 4), Qt::UserRole)
      .value<doCore::shotInfoPtr>();

  //创建上传类
  auto k_fileexport = std::make_shared<doCore::mayaArchiveShotFbx>(export_data);
  //将导出类插入到管理中
  exportList.push_back(k_fileexport);
  //开始导出
  std::thread export_th{&doCore::mayaArchiveShotFbx::exportFbx, k_fileexport.get(), data};
  export_th.detach();

  //更新列表
  p_model_->init(p_type_ptr_);
}
void shotTableWidget::createFlipbook_slot() {
  auto k_path_str = QFileDialog::getExistingDirectory(this, "选择镜头拍屏图片文件夹");
  if (!k_path_str.isEmpty())
    createFlipbook(k_path_str);
}

void shotTableWidget::createFlipbook(const QString &image_folder) {

  p_model_->insertRow(0, QModelIndex());
  auto movide_data = p_model_->data(p_model_->index(0, 4), Qt::UserRole)
      .value<doCore::shotInfoPtr>();

  auto k_movie = std::make_shared<doCore::movieArchive>(movide_data);

  auto th = std::thread(&doCore::movieArchive::update, *k_movie, std::vector{image_folder});
  th.detach();
  p_model_->init(p_type_ptr_);
}

DOODLE_NAMESPACE_E

