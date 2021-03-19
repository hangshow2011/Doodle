#include "mainWindows.h"
//logger是boost库使用者，放到qt上面能好点
#include <loggerlib/Logger.h>

#include <QListWidget>
#include <QMenuBar>
#include <QStatusBar>

#include <QtWidgets/qdockwidget.h>
#include <QtWidgets/qsizepolicy.h>
#include <doodle_GUI/source/mainWidght/DragPushBUtton.h>
#include <doodle_GUI/source/mainWidght/systemTray.h>

DOODLE_NAMESPACE_S

mainWindows::mainWindows(QWidget *parent)
    : QMainWindow(parent),
      exitAction(nullptr),
      refreshAction(nullptr),
      openSetWindows(nullptr),
      p_menu_bar_(nullptr),
      p_menu_(nullptr),
      p_status_bar_(nullptr),
      centralWidget(nullptr),
      p_b_box_layout_(nullptr) {
  setDockNestingEnabled(true);
  //添加动作和菜单
  doodle_createAction();
  doodle_init();
}

void mainWindows::doodle_init() {
  //初始化自身
  /* if (objectName().isEmpty())  */
  setObjectName(QString{"mainWindows"});

  // resize(1200, 800);
  setWindowTitle(tr("工具箱"));

  //设置中央小部件
  centralWidget = new QWidget(this);
  centralWidget->setObjectName(QString::fromUtf8("mainWindowsCentral"));
  //添加中央小部件
  setCentralWidget(centralWidget);

  auto layout          = new QGridLayout(centralWidget);
  auto k_exMaya_button = new DragPushBUtton();
  //导出maya文件
  k_exMaya_button->setText(tr("从maya导出相机和文件"));
  k_exMaya_button->setToolTip(tr(R"(注意:
请把导出文件拖拽到此处, 可以拖拽多个文件, 会依照顺序导出
默认导出路径是在文件所在的目录
)"));
  // 创建视频
  auto k_create_image = new DragPushBUtton();
  k_create_image->setText(tr("从图片创建视频"));
  k_create_image->setToolTip(tr(R"(注意:
图片连接为视频时, 是按照名称来创建顺序的)"));

  auto k_create_video = new DragPushBUtton();
  k_create_video->setText(tr("连接视频"));
  k_create_video->setToolTip(tr(R"(注意:
连接拍屏时是按照文件名称排序的, 请一定要注意文件名称)"));

  layout->addWidget(k_exMaya_button, 0, 0, 1, 1);
  layout->addWidget(k_create_image, 1, 0, 1, 1);
  layout->addWidget(k_create_video, 2, 0, 1, 1);

  //托盘创建
  auto tray = new systemTray(this);
  tray->showMessage("doodle", "hello");
  tray->setIcon(QIcon(":/resource/icon.png"));
  tray->setVisible(true);
  tray->show();
}

void mainWindows::doodle_createAction() {
  //添加菜单栏
  p_menu_bar_ = new QMenuBar(this);
  p_menu_bar_->setObjectName(QString::fromUtf8("p_menu_bar_"));
  p_menu_bar_->setGeometry(QRect(0, 0, 640, 31));
  this->setMenuBar(p_menu_bar_);

  //添加菜单
  p_menu_ = new QMenu(p_menu_bar_);
  p_menu_->setObjectName(QString::fromUtf8("p_menu_"));
  p_menu_->setTitle(tr("&File"));
  p_menu_bar_->addAction(p_menu_->menuAction());

  //添加菜单动作
  refreshAction = new QAction(this);
  refreshAction->setObjectName(QString::fromUtf8("refreshAction"));
  refreshAction->setText(tr("Refresh"));
  refreshAction->setStatusTip(tr("刷新"));
  refreshAction->setToolTip(tr("Refresh"));
  p_menu_->addAction(refreshAction);

  openSetWindows = new QAction(this);
  openSetWindows->setObjectName(QString::fromUtf8("openSetWindows"));
  openSetWindows->setText(tr("Open Setting"));
  openSetWindows->setStatusTip(tr("打开设置"));
  openSetWindows->setToolTip(tr("Open Setting"));
  connect(openSetWindows, &QAction::triggered, this, &mainWindows::openSetting);
  p_menu_->addAction(openSetWindows);

  exitAction = new QAction(this);
  exitAction->setObjectName(QString::fromUtf8("exitAction"));
  exitAction->setText(tr("Exit"));
  exitAction->setStatusTip(tr("退出"));
  exitAction->setToolTip(tr("Exit"));
  p_menu_->addAction(exitAction);

  //添加状态栏
  p_status_bar_ = new QStatusBar(this);
  p_status_bar_->setObjectName(QString::fromUtf8("p_status_bar_"));
  setStatusBar(p_status_bar_);
}

void mainWindows::openSetting() {
}

void mainWindows::setProject() {
}
DOODLE_NAMESPACE_E
