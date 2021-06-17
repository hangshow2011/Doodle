//
// Created by TD on 2021/4/29.
//
#include "MetadataWidget.h"
#include <DoodleLib/DoodleApp.h>
#include <DoodleLib/Exception/Exception.h>
#include <DoodleLib/FileWarp/ImageSequence.h>
#include <DoodleLib/FileWarp/Ue4Project.h>
#include <DoodleLib/Metadata/ContextMenu.h>
#include <DoodleLib/Metadata/Episodes.h>
#include <DoodleLib/Metadata/MetadataFactory.h>
#include <DoodleLib/Metadata/Model/AssetsTree.h>
#include <DoodleLib/Metadata/Model/ListAttributeModel.h>
#include <DoodleLib/Metadata/Model/ProjectManage.h>
#include <DoodleLib/Metadata/Project.h>
#include <DoodleLib/Metadata/Shot.h>
#include <DoodleLib/core/CoreSet.h>


namespace doodle {

MetadataWidget::MetadataWidget(wxWindow* in_window, wxWindowID in_id)
    : wxFrame(in_window, in_id, ConvStr<wxString>("Metadata")),
      p_metadata_flctory_ptr_(std::make_shared<MetadataFactory>()),
      p_tree_id_(NewControlId()),
      p_List_id_(NewControlId()),
      p_assets_tree_view_ctrl_(new wxDataViewCtrl{this, p_tree_id_}),
      p_assets_attribute_view_ctrl_(new wxDataViewCtrl{this, p_List_id_}),
      p_project_view_ctrl_(new wxDataViewCtrl{this, NewControlId()}),
      p_project_model(new ProjectManage{}),
      p_assets_tree_model(new AssetsTree{}),
      p_assets_attribute_model(new ListAttributeModel{}) {
  auto k_layout   = new wxBoxSizer{wxOrientation::wxVERTICAL};
  auto k_layout_1 = new wxBoxSizer{wxOrientation::wxHORIZONTAL};

  //设置模型
  p_project_view_ctrl_->AssociateModel(p_project_model.get());
  p_assets_tree_view_ctrl_->AssociateModel(p_assets_tree_model.get());
  p_assets_attribute_view_ctrl_->AssociateModel(p_assets_attribute_model.get());

  // 项目树
  p_project_view_ctrl_->AppendTextColumn(
      ConvStr<wxString>("名称"),
      0,
      wxDataViewCellMode::wxDATAVIEW_CELL_INERT);
  p_project_view_ctrl_->AppendTextColumn(
      ConvStr<wxString>("拼音名称"),
      1,
      wxDataViewCellMode::wxDATAVIEW_CELL_INERT);
  p_project_view_ctrl_->AppendTextColumn(
      ConvStr<wxString>("路径"),
      2,
      wxDataViewCellMode::wxDATAVIEW_CELL_INERT);

  // 标签树
  p_assets_tree_view_ctrl_->AppendTextColumn(
      ConvStr<wxString>("标签树"), 0,
      wxDataViewCellMode::wxDATAVIEW_CELL_INERT);
  p_assets_tree_view_ctrl_->SetMinSize(wxSize{300, 600});

  p_assets_attribute_view_ctrl_->AppendTextColumn(
      ConvStr<wxString>("id"), 0,
      wxDataViewCellMode::wxDATAVIEW_CELL_INERT);
  p_assets_attribute_view_ctrl_->AppendTextColumn(
      ConvStr<wxString>("版本"), 1,
      wxDataViewCellMode::wxDATAVIEW_CELL_INERT);
  p_assets_attribute_view_ctrl_->AppendTextColumn(
      ConvStr<wxString>("名称"), 2,
      wxDataViewCellMode::wxDATAVIEW_CELL_INERT);
  auto k_com_col = p_assets_attribute_view_ctrl_->AppendTextColumn(
      ConvStr<wxString>("评论"), 3,
      wxDataViewCellMode::wxDATAVIEW_CELL_EDITABLE, 250);
  auto k_time_col = p_assets_attribute_view_ctrl_->AppendTextColumn(
      ConvStr<wxString>("时间"), 4,
      wxDataViewCellMode::wxDATAVIEW_CELL_EDITABLE, 170);
  p_assets_attribute_view_ctrl_->AppendTextColumn(
      ConvStr<wxString>("制作人"), 5,
      wxDataViewCellMode::wxDATAVIEW_CELL_INERT);

  /// 各种布局
  k_layout->Add(p_project_view_ctrl_, wxSizerFlags{1}.Expand().Border(0));
  k_layout->Add(k_layout_1, wxSizerFlags{6}.Expand().Border(wxDirection::wxALL, 0));
  k_layout_1->Add(p_assets_tree_view_ctrl_, wxSizerFlags{2}.Expand().Border(wxDirection::wxALL, 0));
  k_layout_1->Add(p_assets_attribute_view_ctrl_, wxSizerFlags{7}.Expand().Border(wxDirection::wxALL, 0));

  /// 绑定各种函数
  /// 上下文菜单
  p_project_view_ctrl_->Bind(
      wxEVT_DATAVIEW_ITEM_CONTEXT_MENU,
      [this](wxDataViewEvent& in_event) { context_menu(in_event, p_project_model.get()); });
  p_assets_tree_view_ctrl_->Bind(
      wxEVT_DATAVIEW_ITEM_CONTEXT_MENU,
      [this](wxDataViewEvent& in_event) { context_menu(in_event, p_assets_tree_model.get()); });
  p_assets_attribute_view_ctrl_->Bind(
      wxEVT_DATAVIEW_ITEM_CONTEXT_MENU,
      [this](wxDataViewEvent& in_event) { context_menu(in_event, p_assets_attribute_model.get()); });

  ///双击回调
  p_project_view_ctrl_->Bind(
      wxEVT_DATAVIEW_ITEM_ACTIVATED,
      [](wxDataViewEvent& in_event) {
        auto item = in_event.GetItem();
        if (!item.IsOk())
          return;

        MetadataSet::Get().setProject_(reinterpret_cast<Project*>(item.GetID()));
      });
  ///project 双击回调

  p_assets_tree_view_ctrl_->Bind(
      wxEVT_DATAVIEW_ITEM_ACTIVATED,
      [this](wxDataViewEvent& in_event) {
        auto item = in_event.GetItem();
        if (!item.IsOk())
          return;

        auto k_m = reinterpret_cast<Metadata*>(item.GetID());
        this->p_assets_attribute_model->setRoot(k_m->shared_from_this());
        this->p_assets_tree_model->set_current(k_m->shared_from_this());
        this->p_assets_tree_view_ctrl_->Refresh();
      });

  MetadataSet::Get().sig_projectChange.connect(
      [this](doodle::Project* in_prj, int index) {
        this->p_assets_tree_model->setRoot(std::dynamic_pointer_cast<Project>(in_prj->shared_from_this()));
      });
  // auto k_p_text_renderer = new wxDataViewTextRenderer{"string", wxDATAVIEW_CELL_EDITABLE};
  // auto k_col             = new wxDataViewColumn{ConvStr<wxString>("标签树"), k_p_text_renderer, 0, 100};
  // p_assets_tree_view_ctrl_->AppendColumn(k_col);
  // p_assets_tree_view_ctrl_->AssociateModel(new AssetsTree{});
  // k_p_text_renderer = new wxDataViewTextRenderer{"string", wxDATAVIEW_CELL_EDITABLE};
  // k_col             = new wxDataViewColumn{ConvStr<wxString>("文件"), k_p_text_renderer, 0, 100};
  // p_assets_attribute_view_ctrl_->AppendColumn(k_col);
  // p_assets_attribute_view_ctrl_->SetMinSize(wxSize{300, 600});

  SetSizer(k_layout);
  k_layout->SetSizeHints(this);
  this->Center();
  MetadataSet::Get().setProject_(MetadataSet::Get().Project_());
}

void MetadataWidget::context_menu(wxDataViewEvent& in_event, wxDataViewModel* in_model) {
  wxMenu k_wx_menu{};
  ContextMenu k_context_menu{this, &k_wx_menu, in_model};
  auto k_data = in_event.GetItem();
  if (k_data.IsOk()) {
    auto k_item = reinterpret_cast<Metadata*>(k_data.GetID());
    k_item->createMenu(&k_context_menu);
  } else {
    k_context_menu.createMenuAfter();
  }
  PopupMenu(&k_wx_menu);
}

}  // namespace doodle
