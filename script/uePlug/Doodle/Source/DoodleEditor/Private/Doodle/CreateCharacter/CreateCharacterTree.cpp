#include "CreateCharacterTree.h"

#include "CharacterEditorViewport.h"
#include "Engine/SkeletalMeshSocket.h"              // 骨骼 Socket
#include "Widgets/Input/SSlider.h"                  // 滑动条
#include "SScrubControlPanel.h"                     // 时间控制
#include "Framework/Docking/TabManager.h"           // 选项卡布局
#include "BoneSelectionWidget.h"                    // 骨骼树
#include "Widgets/Text/SInlineEditableTextBlock.h"  // 内联编辑小部件

#include "Doodle/CreateCharacter/CoreData/DoodleCreateCharacterConfig.h"

#define LOCTEXT_NAMESPACE "SCreateCharacterConfigTreeItem"
class SCreateCharacterConfigTreeItem : public SMultiColumnTableRow<SCreateCharacterTree::TreeVirwWeightItemType> {
 public:
  using Super = SMultiColumnTableRow<SCreateCharacterTree::TreeVirwWeightItemType>;

  SLATE_BEGIN_ARGS(SCreateCharacterConfigTreeItem)
      : _ItemData(),
        _InConfig() {}

  SLATE_ARGUMENT(SCreateCharacterTree::TreeVirwWeightItemType, ItemData)
  SLATE_ARGUMENT(UDoodleCreateCharacterConfig*, InConfig)

  SLATE_END_ARGS()

  // 这里是内容创建函数
  void Construct(const FArguments& Arg, const TSharedRef<STableViewBase>& OwnerTableView) {
    ItemData    = Arg._ItemData;
    Config_Data = Arg._InConfig;
    Super::Construct(Super::FArguments{}, OwnerTableView);
    if (ItemData) {
      Item_Name = ItemData->ShowName;
    }
  }

  TSharedRef<SWidget> GenerateWidgetForColumn(const FName& InColumnName) override {
    // 只是题头部件
    TSharedPtr<SHorizontalBox> L_Box = SNew(SHorizontalBox) + SHorizontalBox::Slot().AutoWidth()[SNew(SExpanderArrow, SharedThis(this)).ShouldDrawWires(true)];

    TSharedPtr<SInlineEditableTextBlock> L_InlineEditableTextBlock_Ptr =
        SNew(SInlineEditableTextBlock)
            //.ColorAndOpacity(this, &FSkeletonTreeVirtualBoneItem::GetBoneTextColor, InIsSelected)
            .Text(TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateSP(this, &SCreateCharacterConfigTreeItem::Get_ItemName)))
            //.HighlightText(FilterText)
            .Font(this, &SCreateCharacterConfigTreeItem::GetTextFont)
            //.ToolTipText(ToolTip)
            .OnEnterEditingMode(this, &SCreateCharacterConfigTreeItem::OnItemNameEditing)
            .OnVerifyTextChanged(this, &SCreateCharacterConfigTreeItem::OnVerifyItemNameChanged)
            .OnTextCommitted(this, &SCreateCharacterConfigTreeItem::OnCommitItemBoneName)
        //.IsSelected(InIsSelected)
        ;

    if (ItemData) {
      ItemData->OnRenameRequested.BindSP(L_InlineEditableTextBlock_Ptr.Get(), &SInlineEditableTextBlock::EnterEditingMode);
    }

    if (ItemData->ItemKeys.IsEmpty()) {
      if (InColumnName == SCreateCharacterTree::G_Name) {
        L_Box->AddSlot().AutoWidth()[L_InlineEditableTextBlock_Ptr.ToSharedRef()];
      }
    } else {
      if (InColumnName == SCreateCharacterTree::G_Name) {
        L_Box->AddSlot().AutoWidth()[L_InlineEditableTextBlock_Ptr.ToSharedRef()];
      } else if (InColumnName == SCreateCharacterTree::G_Value) {
        L_Box->AddSlot().FillWidth(1.0f)[

            SNew(SSlider)
                .Value(Slider_Value)
                .MaxValue(ItemData->MaxValue)
                .MinValue(ItemData->MinValue)
                .OnValueChanged(FOnFloatValueChanged::CreateSP(this, &SCreateCharacterConfigTreeItem::On_FloatValueChanged))

        ];
      }
    }
    return L_Box.ToSharedRef();
  }

  virtual int32 DoesItemHaveChildren() const override {
    if (!ItemData->Childs.IsEmpty()) return 1;

    return Super::DoesItemHaveChildren();
  };

  void On_FloatValueChanged(float In_Value) {
    Slider_Value = In_Value;
    UE_LOG(LogTemp, Warning, TEXT("FileManipulation: %f"), In_Value);
  }

  virtual bool IsItemExpanded() const override { return Super::IsItemExpanded() || !ItemData->Childs.IsEmpty(); };

  virtual void ToggleExpansion() override { Super::ToggleExpansion(); };

 private:
  void OnItemNameEditing() {
  }
  FText Get_ItemName() {
    return FText::FromName(Item_Name);
  }

  bool OnVerifyItemNameChanged(const FText& InText, FText& OutErrorMessage) {
    bool bVerifyName      = true;

    FString InTextTrimmed = FText::TrimPrecedingAndTrailing(InText).ToString();

    // FString NewName       = VirtualBoneNameHelpers::AddVirtualBonePrefix(InTextTrimmed);

    if (InTextTrimmed.IsEmpty()) {
      OutErrorMessage = LOCTEXT("EmptyVirtualBoneName_Error", "Virtual bones must have a name!");
      bVerifyName     = false;
    } else {
      if (InTextTrimmed != Item_Name.ToString()) {
        // 判断是否存在
        bVerifyName = !((ItemData && Config_Data.IsValid()) ? Config_Data.Get()->Has_UI_ShowName(ItemData->ConfigNode, InTextTrimmed) : true);

        // Needs to be checked on verify.
        if (!bVerifyName) {
          // Tell the user that the name is a duplicate
          OutErrorMessage = LOCTEXT("DuplicateVirtualBone_Error", "Name in use!");
          bVerifyName     = false;
        }
      }
    }

    return bVerifyName;
  }

  void OnCommitItemBoneName(const FText& InText, ETextCommit::Type CommitInfo) {
    FString NewNameString = FText::TrimPrecedingAndTrailing(InText).ToString();
    FName NewName(*NewNameString);

    // 通知所有到更改
    Item_Name = NewName;
    if (!ItemData)
      return;
    ItemData->ShowName = NewName;

    if (!Config_Data.IsValid())
      return;

    Config_Data.Get()->Rename_UI_ShowName(ItemData->ConfigNode, NewName);
  }
  FSlateFontInfo GetTextFont() const {
    return FAppStyle::GetWidgetStyle<FTextBlockStyle>("SkeletonTree.NormalFont").Font;
  }

  SCreateCharacterTree::TreeVirwWeightItemType ItemData;
  TWeakObjectPtr<UDoodleCreateCharacterConfig> Config_Data;
  float Slider_Value;

  FName Item_Name;
};

#undef LOCTEXT_NAMESPACE

#define LOCTEXT_NAMESPACE "SCreateCharacterTree"

const FName SCreateCharacterTree::Name{"Doodle_SCreateCharacterTree"};

const FName SCreateCharacterTree::G_Name{"Name"};
const FName SCreateCharacterTree::G_Value{"Value"};

void SCreateCharacterTree::Construct(const FArguments& Arg) {
  Config     = Arg._CreateCharacterConfig;
  OnEditItem = Arg._OnEditItem;

  CreateUITree();

  Super::Construct(
      Super::FArguments{}
          .TreeItemsSource(&CreateCharacterConfigTreeData)
          .OnGenerateRow(TreeVirwWeightType::FOnGenerateRow::CreateSP(
              this, &SCreateCharacterTree::CreateCharacterConfigTreeData_Row
          ))
          .OnGetChildren(TreeVirwWeightType::FOnGetChildren::CreateSP(
              this, &SCreateCharacterTree::CreateCharacterConfigTreeData_GetChildren
          ))
          .HeaderRow(
              // clang-format off
                           SNew(SHeaderRow) 
                         + SHeaderRow::Column(G_Name)
                         .DefaultLabel(LOCTEXT("Construct", "Name")) 
                         + SHeaderRow::Column(G_Value)
                         .DefaultLabel(LOCTEXT("Construct", "Value"))
              // clang-format on
          )
          .OnContextMenuOpening(FOnContextMenuOpening::CreateSP(this, &SCreateCharacterTree::Create_ContextMenuOpening))
          .OnSelectionChanged(
              TreeVirwWeightType::FOnSelectionChanged::CreateSP(this, &SCreateCharacterTree::On_SelectionChanged)
          )
          .OnMouseButtonDoubleClick(
              FOnMouseButtonDoubleClick::CreateSP(this, &SCreateCharacterTree::On_MouseButtonDoubleClick)
          )
  );
}

TSharedRef<SDockTab> SCreateCharacterTree::OnSpawnAction(const FSpawnTabArgs& SpawnTabArgs) {
  return SNew(SDockTab).TabRole(ETabRole::NomadTab)[SNew(SCreateCharacterTree)];  //
}

TSharedRef<class ITableRow> SCreateCharacterTree::CreateCharacterConfigTreeData_Row(TreeVirwWeightItemType In_Value, const TSharedRef<class STableViewBase>& In_Table) {
  return SNew(SCreateCharacterConfigTreeItem, In_Table).ItemData(In_Value);
}

void SCreateCharacterTree::CreateCharacterConfigTreeData_GetChildren(TreeVirwWeightItemType In_Value, TreeVirwWeightDataType& In_List) {
  In_List = In_Value->Childs;
}

TSharedPtr<SWidget> SCreateCharacterTree::Create_ContextMenuOpening() {
  FMenuBuilder L_Builder{true, UICommandList, Extender};
  {
    L_Builder.BeginSection("Create_ContextMenuOpening_Add_Bone", LOCTEXT("Create_ContextMenuOpening_Add_Bone1", "Add"));

    // 添加
    L_Builder.AddMenuEntry(
        LOCTEXT("Create_ContextMenuOpening_Add_Bone2", "Add Classify"),
        LOCTEXT("Create_ContextMenuOpening_Add_Bone2_Tip", "Add Classify"), FSlateIcon{"Subtitle", "EventIcon"},
        FUIAction{FExecuteAction::CreateLambda([this]() { AddBone(); })}
    );
    if (CurrentSelect && CurrentSelect->ConfigNode && CurrentSelect->ConfigNode->Parent != INDEX_NONE) {
      L_Builder.AddSubMenu(
          LOCTEXT("Create_ContextMenuOpening_Add_Bone4", "Binding"),
          LOCTEXT("Create_ContextMenuOpening_Add_Bone4_Tip", "Binding Bone"),
          FNewMenuDelegate::CreateSP(this, &SCreateCharacterTree::AddBoneTreeMenu)
      );
    }

    L_Builder.EndSection();
  }

  return L_Builder.MakeWidget();
}

void SCreateCharacterTree::On_SelectionChanged(TreeVirwWeightItemType TreeItem, ESelectInfo::Type SelectInfo) {
  CurrentSelect = TreeItem;
}

void SCreateCharacterTree::On_MouseButtonDoubleClick(TreeVirwWeightItemType TreeItem) {
  TreeItem->OnRenameRequested.ExecuteIfBound();
}

void SCreateCharacterTree::AddBoneTreeMenu(FMenuBuilder& In_Builder) {
  UDoodleCreateCharacterConfig* L_Config = Config.Get();
  if (!L_Config)
    return;
  const bool bShowVirtualBones          = false;
  TSharedRef<SBoneTreeMenu> MenuContent = SNew(SBoneTreeMenu)
                                              .bShowVirtualBones(bShowVirtualBones)
                                              .Title(LOCTEXT("TargetBonePickerTitle", "Pick Target Bone..."))
                                              .OnBoneSelectionChanged_Lambda([this](FName In_BoneName) {
                                                FSlateApplication::Get().DismissAllMenus();
                                                this->Add_TreeNode(In_BoneName);
                                              })
                                              .OnGetReferenceSkeleton_Lambda([L_Config]() -> const FReferenceSkeleton& {
                                                return L_Config->GetSkeletalMesh()->GetRefSkeleton();
                                              });

  In_Builder.AddWidget(MenuContent, FText::GetEmpty(), true);

  MenuContent->RegisterActiveTimer(
      0.0f,
      FWidgetActiveTimerDelegate::CreateLambda([FilterTextBox = MenuContent->GetFilterTextWidget()](double, float) {
        FSlateApplication::Get().SetKeyboardFocus(FilterTextBox);
        return EActiveTimerReturnType::Stop;
      })
  );
}

void SCreateCharacterTree::Add_TreeNode(const FName& In_Bone_Name) {
  if (!CurrentSelect)
    return;
  if (!CurrentSelect->Childs.IsEmpty())
    return;
  UDoodleCreateCharacterConfig* L_Config = Config.Get();
  if (!L_Config) return;

  TOptional<FString> L_Key = L_Config->Add_ConfigNode(In_Bone_Name, (CurrentSelect && CurrentSelect->ConfigNode) ? L_Config->ListTrees.Find(*CurrentSelect->ConfigNode) : INDEX_NONE);

  if (L_Key && !CurrentSelect->ItemKeys.Contains(*L_Key)) {
    CurrentSelect->ItemKeys.Add(*L_Key);
  }
  this->RebuildList();
}

void SCreateCharacterTree::CreateUITree() {
  UDoodleCreateCharacterConfig* L_Config = Config.Get();

  if (!L_Config) return;

  typedef TDelegate<
      void(const SCreateCharacterTree::TreeVirwWeightItemType& InParent, UDoodleCreateCharacterConfig* InConfig, const TArray<int32>& InChildIndex)>
      FDOODLE_IMPL_AddNode;
  ;
  FDOODLE_IMPL_AddNode L_IMPL{};

  L_IMPL.BindLambda([L_IMPL](
                        const SCreateCharacterTree::TreeVirwWeightItemType& InParent,
                        UDoodleCreateCharacterConfig* InConfig,
                        const TArray<int32>& InChildIndex
                    ) {
    for (auto i : InChildIndex) {
      auto& L_Nodes = InConfig->ListTrees[i];
      // if (L_Nodes.Childs.IsEmpty()) continue;

      SCreateCharacterTree::TreeVirwWeightItemType L_Ptr =
          InParent->Childs.Add_GetRef(MakeShared<SCreateCharacterTree::TreeVirwWeightItemType::ElementType>());
      L_Ptr->ShowName   = L_Nodes.ShowUIName;
      // 添加子项
      L_Ptr->ItemKeys   = L_Nodes.Keys;
      L_Ptr->MaxValue   = L_Nodes.MaxValue;
      L_Ptr->MinValue   = L_Nodes.MinValue;
      L_Ptr->ConfigNode = &L_Nodes;
      L_IMPL.Execute(L_Ptr, InConfig, L_Nodes.Childs);
    }
  });

  CreateCharacterConfigTreeData.Empty(L_Config->ListConfigNode.Num());
  for (auto&& i : L_Config->ListTrees) {
    if (i.Parent != INDEX_NONE) {
      continue;
    }
    TreeVirwWeightItemType L_Ptr =
        CreateCharacterConfigTreeData.Add_GetRef(MakeShared<TreeVirwWeightItemType::ElementType>());
    L_Ptr->MaxValue   = i.MaxValue;
    L_Ptr->MinValue   = i.MinValue;
    L_Ptr->ConfigNode = &i;
    L_IMPL.Execute(L_Ptr, L_Config, i.Childs);
  }
}

void SCreateCharacterTree::AddBone() {
  UDoodleCreateCharacterConfig* L_Config = Config.Get();

  if (!L_Config)
    return;

  FDoodleCreateCharacterConfigUINode* L_UI_Node = L_Config->Add_TreeNode(
      (CurrentSelect && CurrentSelect->ConfigNode) ? L_Config->ListTrees.Find(*CurrentSelect->ConfigNode) : INDEX_NONE
  );
  if (!L_UI_Node)
    return;
  const bool L_Has_Parent{CurrentSelect};

  TreeVirwWeightItemType L_Ptr{
      L_Has_Parent ? CurrentSelect->Childs.Add_GetRef(MakeShared<TreeVirwWeightItemType::ElementType>())
                   : MakeShared<TreeVirwWeightItemType::ElementType>()};

  if (!L_Has_Parent) CreateCharacterConfigTreeData.Add(L_Ptr);

  L_UI_Node->ShowUIName = FName{"Add_Bone"};

  L_Ptr->ConfigNode     = L_UI_Node;
  L_Ptr->MaxValue       = L_UI_Node->MaxValue;
  L_Ptr->MinValue       = L_UI_Node->MinValue;
  L_Ptr->ShowName       = L_UI_Node->ShowUIName;

  // CreateUITree();
  this->RequestTreeRefresh();
  // this->RebuildList();
}

#undef LOCTEXT_NAMESPACE