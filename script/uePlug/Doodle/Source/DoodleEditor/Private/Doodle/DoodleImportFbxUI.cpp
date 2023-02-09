#include "DoodleImportFbxUI.h"
#include "Widgets/SCanvas.h"
// 目录选择器
#include "Widgets/Input/SDirectoryPicker.h"
// 文件选择器
#include "Widgets/Input/SFilePathPicker.h"
#include "AssetRegistry/IAssetRegistry.h"
// 我们自己的多路径文件选择器
#include "Doodle/FilePathsPicker.h"

// fbx读取需要
#include "FbxImporter.h"
#include "Rendering/SkeletalMeshLODImporterData.h"

#include "fbxsdk/scene/geometry/fbxcameraswitcher.h"
#include "fbxsdk/scene/geometry/fbxcamera.h"
#include "fbxsdk/scene/animation/fbxanimstack.h"
#include "fbxsdk/scene/animation/fbxanimlayer.h"
#include "fbxsdk/scene/geometry/fbxnode.h"
#include "fbxsdk/scene/animation/fbxanimcurve.h"

// 读写文件
#include "Misc/FileHelper.h"
// 元数据
#include "UObject/MetaData.h"
// 算法
#include "Algo/AllOf.h"
/// 自动导入类需要
#include "AssetImportTask.h"

/// 正则
#include "Internationalization/Regex.h"
/// 一般的导入任务设置
#include "AssetImportTask.h"
/// 导入模块
#include "AssetToolsModule.h"
/// 导入fbx需要
#include "Factories/Factory.h"
#include "Factories/FbxAnimSequenceImportData.h"
#include "Factories/FbxFactory.h"
#include "Factories/FbxImportUI.h"
#include "Factories/FbxSkeletalMeshImportData.h"
#include "Factories/FbxTextureImportData.h"
#include "Factories/ImportSettings.h"
#include "Factories/MaterialImportHelpers.h"
/// 进度框
#include "Misc/ScopedSlowTask.h"
/// 属性按钮
#include "PropertyCustomizationHelpers.h"
/// 内容游览器模块
#include "ContentBrowserModule.h"
/// 内容游览器
#include "IContentBrowserSingleton.h"
/// 导入abc
#include "AbcImportSettings.h"
/// 编辑器笔刷效果
#include "EditorStyleSet.h"

/// 导入相机需要的头文件
#include "Camera/CameraComponent.h"  // 相机组件
#include "CineCameraActor.h"         // 相机
#include "ILevelSequenceEditorToolkit.h"
#include "LevelSequence.h"
#include "MovieSceneToolHelpers.h"
#include "Sections/MovieSceneCameraCutSection.h"  // 相机剪切
#include "SequencerUtilities.h"                   // 创建相机

#define LOCTEXT_NAMESPACE "SDoodleImportFbxUI"
const FName SDoodleImportFbxUI::Name{TEXT("DoodleImportFbxUI")};

namespace {
struct grop_SDoodleImportFbxUI {
  UnFbx::FFbxImporter* FbxImporterData;
  grop_SDoodleImportFbxUI(UnFbx::FFbxImporter* In) : FbxImporterData(In) {}
  ~grop_SDoodleImportFbxUI() {
    FbxImporterData->ClearAllCaches();
    FbxImporterData->ReleaseScene();
  }
};

FString MakeName(const ANSICHAR* Name) {
  const TCHAR SpecialChars[]    = {TEXT('.'), TEXT(','), TEXT('/'), TEXT('`'), TEXT('%')};

  FString TmpName               = FString{ANSI_TO_TCHAR(Name)};

  // Remove namespaces
  int32 LastNamespaceTokenIndex = INDEX_NONE;
  if (TmpName.FindLastChar(TEXT(':'), LastNamespaceTokenIndex)) {
    const bool bAllowShrinking = true;
    //+1 to remove the ':' character we found
    TmpName.RightChopInline(LastNamespaceTokenIndex + 1, bAllowShrinking);
  }

  // Remove the special chars
  for (int32 i = 0; i < UE_ARRAY_COUNT(SpecialChars); i++) {
    TmpName.ReplaceCharInline(SpecialChars[i], TEXT('_'), ESearchCase::CaseSensitive);
  }

  return FSkeletalMeshImportData::FixupBoneName(TmpName);
}

FString GetNamepace(const ANSICHAR* Name) {
  FString TmpName               = FString{ANSI_TO_TCHAR(Name)};
  // Remove namespaces
  int32 LastNamespaceTokenIndex = INDEX_NONE;
  const bool bAllowShrinking    = true;
  if (TmpName.FindLastChar(TEXT(':'), LastNamespaceTokenIndex)) {
    //+1 to remove the ':' character we found
    TmpName.LeftChopInline(TmpName.Len() - LastNamespaceTokenIndex, bAllowShrinking);
  } else {
    return {};
  }
  LastNamespaceTokenIndex = INDEX_NONE;
  if (TmpName.FindLastChar(TEXT(':'), LastNamespaceTokenIndex)) {
    //+1 to remove the ':' character we found
    TmpName.RightChopInline(LastNamespaceTokenIndex + 1, bAllowShrinking);
  }
  return FSkeletalMeshImportData::FixupBoneName(TmpName);
}

void FindSkeletonNode(fbxsdk::FbxNode* Parent, TArray<fbxsdk::FbxNode*>& In_Skeketon) {
  if (Parent &&
      ((Parent->GetMesh() && Parent->GetMesh()->GetDeformerCount(fbxsdk::FbxDeformer::EDeformerType::eSkin) > 0) ||
       (Parent->GetNodeAttribute() && (Parent->GetNodeAttribute()->GetAttributeType() == fbxsdk::FbxNodeAttribute::eSkeleton ||
                                       Parent->GetNodeAttribute()->GetAttributeType() == fbxsdk::FbxNodeAttribute::eNull)))) {
    In_Skeketon.Add(Parent);
  }

  int32 NodeCount = Parent->GetChildCount();
  for (int32 NodeIndex = 0; NodeIndex < NodeCount; ++NodeIndex) {
    fbxsdk::FbxNode* Child = Parent->GetChild(NodeIndex);
    FindSkeletonNode(Child, In_Skeketon);
  }
}

void Debug_To_File(const FStringView& In_String) {
  FString LFile_Path = FPaths::CreateTempFilename(*FPaths::ProjectSavedDir(), TEXT("Doodle"));
  // Always first check if the file that you want to manipulate exist.
  if (FFileHelper::SaveStringToFile(In_String, *LFile_Path)) {
    UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Sucsesfuly Written: to the text file"));
  } else {
    UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Failed to write FString to file."));
  }
}

}  // namespace

class SDoodleImportFbxUiItem : public SMultiColumnTableRow<TSharedPtr<doodle_ue4::FFbxImport>> {
 public:
  SLATE_BEGIN_ARGS(SDoodleImportFbxUiItem) : _ItemShow() {}

  SLATE_ARGUMENT(TSharedPtr<doodle_ue4::FFbxImport>, ItemShow)

  SLATE_END_ARGS()

 public:
  void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView) {
    ItemShow = InArgs._ItemShow;

    FSuperRowType::Construct(FSuperRowType::FArguments().Padding(0), InOwnerTableView);
  }

 public:  // override SMultiColumnTableRow
  virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override {
    if (ColumnName == TEXT("Fbx File"))  // 判断列名为Fbx File，次名称在创建View时，通过SHeaderRow::Column指定
    {
      return SNew(STextBlock)
          .Text(FText::FromString(ItemShow->ImportFbxPath));
    } else if (ColumnName == TEXT("Import Path Dir")) {
      return SNew(STextBlock)
          .Text(FText::FromString(ItemShow->ImportPathDir));
    } else {
      // clang-format off
       return SNew(SHorizontalBox) 
        + SHorizontalBox::Slot()
        .Padding(1.f)
        .HAlign(HAlign_Left)
        [
          SNew(STextBlock)
          .Text_Lambda([this]() -> FText {
            return FText::FromString(FString::Printf(TEXT("%s"), *( ItemShow->SkinObj != nullptr ?
                                                                    ItemShow->SkinObj->GetPackage()->GetPathName() : FString{TEXT("")})));
          })
        ]
        + SHorizontalBox::Slot()///  
        .AutoWidth()
        .HAlign(HAlign_Right)
        [
          SNew(SHorizontalBox) 
          + SHorizontalBox::Slot()/// ⬅️, 将选中的给到属性上
          .HAlign(HAlign_Right)
          [
            PropertyCustomizationHelpers::MakeUseSelectedButton(FSimpleDelegate::CreateRaw(this,&SDoodleImportFbxUiItem::DoodleUseSelected))/// 委托转发
          ]
          + SHorizontalBox::Slot()/// 🔍 将属性显示在资产编辑器中
          .HAlign(HAlign_Right)
          [
            PropertyCustomizationHelpers::MakeBrowseButton(FSimpleDelegate::CreateRaw(this,&SDoodleImportFbxUiItem::DoodleBrowse))/// 委托转发
          ]
          + SHorizontalBox::Slot()/// 重置, 将属性给空
          .HAlign(HAlign_Right)
          [
            PropertyCustomizationHelpers::MakeResetButton(FSimpleDelegate::CreateRaw(this,&SDoodleImportFbxUiItem::DoodleReset))/// 委托转发
          ]
        ]
          // clang-format on
          ;
      // return SNew(STextBlock)
      //     .Text(FText::FromString(FString::Printf(TEXT("%s"), *ItemShow->SkinObj->GetPackage()->GetPathName())));
    }
  }

  void DoodleUseSelected() {
    FContentBrowserModule& L_ContentBrowserModle =
        FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(
            "ContentBrowser"
        );
    TArray<FAssetData> L_SelectedAss;
    L_ContentBrowserModle.Get().GetSelectedAssets(L_SelectedAss);

    FAssetData* L_It = Algo::FindByPredicate(L_SelectedAss, [](const FAssetData& InAss) -> bool {
      return Cast<USkeleton>(InAss.GetAsset()) != nullptr;
    });
    if (L_It != nullptr) {
      ItemShow->SkinObj = Cast<USkeleton>(L_It->GetAsset());
    }
  }
  void DoodleBrowse() {
    if (ItemShow->SkinObj != nullptr) {
      FContentBrowserModule& L_ContentBrowserModle =
          FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(
              "ContentBrowser"
          );
      L_ContentBrowserModle.Get().SyncBrowserToAssets(TArray<FAssetData>{FAssetData{ItemShow->SkinObj}});
    }
  }
  void DoodleReset() {
    ItemShow->SkinObj = nullptr;
  }

 private:
  TSharedPtr<doodle_ue4::FFbxImport> ItemShow;
};

class SDoodleImportAbcUiItem : public SMultiColumnTableRow<TSharedPtr<doodle_ue4::FFbxImport>> {
 public:
  SLATE_BEGIN_ARGS(SDoodleImportAbcUiItem) : _ItemShow() {}

  SLATE_ARGUMENT(TSharedPtr<doodle_ue4::FAbcImport>, ItemShow)

  SLATE_END_ARGS()

 public:
  void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView) {
    ItemShow = InArgs._ItemShow;

    FSuperRowType::Construct(FSuperRowType::FArguments().Padding(0), InOwnerTableView);
  }

 public:  // override SMultiColumnTableRow
  virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override {
    if (ColumnName == TEXT("Abc File"))  // 判断列名为Fbx File，次名称在创建View时，通过SHeaderRow::Column指定
    {
      return SNew(STextBlock)
          .Text(FText::FromString(ItemShow->ImportAbcPath));
    } else if (ColumnName == TEXT("Import Path Dir")) {
      return SNew(STextBlock)
          .Text(FText::FromString(ItemShow->ImportPathDir));
    } else {
      // clang-format off
       return SNew(STextBlock)
          .Text_Lambda([this]() -> FText {
            return FText::FromString(FString::Printf(TEXT("%d - %d"), ItemShow->StartTime, ItemShow->EndTime));
          })
          // clang-format on
          ;
    }
  }

 private:
  TSharedPtr<doodle_ue4::FAbcImport> ItemShow;
};

class SDoodleImportFbxCameraUiItem : public SMultiColumnTableRow<TSharedPtr<doodle_ue4::FFbxCameraImport>> {
 public:
  SLATE_BEGIN_ARGS(SDoodleImportFbxCameraUiItem) : _ItemShow() {}

  SLATE_ARGUMENT(TSharedPtr<doodle_ue4::FFbxCameraImport>, ItemShow)

  SLATE_END_ARGS()

 public:
  void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView) {
    ItemShow = InArgs._ItemShow;

    FSuperRowType::Construct(FSuperRowType::FArguments().Padding(0), InOwnerTableView);
  }

 public:  // override SMultiColumnTableRow
  virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override {
    if (ColumnName == TEXT("Fbx File"))  // 判断列名为Fbx File，次名称在创建View时，通过SHeaderRow::Column指定
    {
       return SNew(STextBlock).Text(FText::FromString(ItemShow->ImportFbxPath));
    } else {
       return SNew(STextBlock).Text(FText::FromString(ItemShow->ImportPathDir));
    }
    // if (ColumnName == TEXT("Import Path Dir"))
  }

 private:
  TSharedPtr<doodle_ue4::FFbxCameraImport> ItemShow;
};

void SDoodleImportFbxUI::Construct(const FArguments& Arg) {
  const FSlateFontInfo Font = FEditorStyle::GetFontStyle(TEXT("SourceControl.LoginWindow.Font"));

#if PLATFORM_WINDOWS
  const FString FileFilterText = TEXT("fbx and abc |*.fbx;*.abc|fbx (*.fbx)|*.fbx|abc (*.abc)|*.abc");
#else
  const FString FileFilterText = FString::Printf(TEXT("%s"), *FileFilterType.ToString());
#endif
  // clang-format off
  ChildSlot
  [
    SNew(SBorder)
      .BorderBackgroundColor(FLinearColor(0.3, 0.3, 0.3, 0.0f))
      .BorderImage(new FSlateBrush())
      .HAlign(HAlign_Fill)
      [
        SNew(SVerticalBox) 
        // 扫描文件目录槽
        + SVerticalBox::Slot()
        .AutoHeight()
        .VAlign(VAlign_Center)
        .Padding(2.0f)
        [
	      SNew(STextBlock)
	      .Text(LOCTEXT("BinaryPathLabel", "将文件和文件夹拖入到这个窗口中, 会自动扫描文件夹下后缀为abc和fbx的子文件,并将所有的文件添加到导入列表中"))
	      .ToolTipText(LOCTEXT("BinaryPathLabel_Tooltip", "search path"))
	      .Font(Font)
        ]
        // 前缀槽
        + SVerticalBox::Slot()
        .AutoHeight()
        .VAlign(VAlign_Center)
        .Padding(2.0f)
        [
          /// 生成的前缀
          SNew(SEditableTextBox)
          .Text_Lambda([this]()-> FText {
            return FText::FromString(this->Path_Prefix);
          })
          .OnTextChanged_Lambda([this](const FText& In_Text) {
            GenPathPrefix(In_Text.ToString());
          })
          .OnTextCommitted_Lambda([this](const FText& In_Text, ETextCommit::Type) {
            GenPathPrefix(In_Text.ToString());
          })
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .VAlign(VAlign_Center)
        .Padding(2.0f)
		[
	      SNew(STextBlock)
	      .Text(LOCTEXT("fbxTitle", "导入的fbx 动画文件"))
	      .Font(Font)
		]

        /// 主要的列表小部件(Fbx)
        + SVerticalBox::Slot()
		.FillHeight(3.0f)
		.VAlign(VAlign_Top)
        .Padding(2.0f)
        [
          SAssignNew(ListImportFbx,SListView<TSharedPtr<doodle_ue4::FFbxImport>>)
          .ItemHeight(80) // 小部件高度
          .ListItemsSource(&ListImportFbxData)
          .ScrollbarVisibility(EVisibility::All)
          .OnGenerateRow_Lambda(// 生成小部件
            [](TSharedPtr<doodle_ue4::FFbxImport> InItem, 
               const TSharedRef<STableViewBase>& OwnerTable) -> TSharedRef<ITableRow> {
              return SNew(SDoodleImportFbxUiItem, OwnerTable)
                    .ItemShow(InItem);
            }
          )
          .SelectionMode(ESelectionMode::Type::Single) //单选
          .HeaderRow ///题头元素
          (
            SNew(SHeaderRow)
            + SHeaderRow::Column(TEXT("Fbx File"))
            [
              SNew(SBorder)
              .Padding(5)
              [
                SNew(STextBlock)
                .Text(FText::FromString(TEXT("Fbx File")))
              ]
            ]
            +SHeaderRow::Column(TEXT("Skeleton Path"))
            .DefaultLabel(LOCTEXT("Skeleton Path","Skeleton Path"))

            +SHeaderRow::Column(TEXT("Import Path Dir"))
            .DefaultLabel(LOCTEXT("Import Path Dir","Import Path Dir"))
          )
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        .VAlign(VAlign_Center)
        .Padding(2.0f)
		[
	      SNew(STextBlock)
	      .Text(LOCTEXT("fbxTitle2", "导入的fbx 摄像机文件"))
	      .Font(Font)
		]
        /// 主要的列表小部件(Fbx Camera)
        + SVerticalBox::Slot()
		.FillHeight(1.0f)
		.VAlign(VAlign_Top)
        .Padding(2.0f)
        [
          SAssignNew(ListImportFbxCam,SListView<TSharedPtr<doodle_ue4::FFbxCameraImport>>)
          .ItemHeight(80) // 小部件高度
          .ListItemsSource(&ListImportFbxCamData)
          .ScrollbarVisibility(EVisibility::All)
          .OnGenerateRow_Lambda(// 生成小部件
            [](TSharedPtr<doodle_ue4::FFbxCameraImport> InItem, 
               const TSharedRef<STableViewBase>& OwnerTable) -> TSharedRef<ITableRow> {
              return SNew(SDoodleImportFbxCameraUiItem, OwnerTable)
                    .ItemShow(InItem);
            }
          )
          .SelectionMode(ESelectionMode::Type::Single) //单选
          .HeaderRow ///题头元素
          (
            SNew(SHeaderRow)
            + SHeaderRow::Column(TEXT("Fbx File"))
            [
              SNew(SBorder)
              .Padding(5)
              [
                SNew(STextBlock)
                .Text(FText::FromString(TEXT("Fbx File")))
              ]
            ]
            +SHeaderRow::Column(TEXT("Import Path Dir"))
            .DefaultLabel(LOCTEXT("Import Path Dir","Import Path Dir"))
          )
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        .VAlign(VAlign_Center)
        .Padding(2.0f)
		[
	      SNew(STextBlock)
	      .Text(LOCTEXT("fbxTitle3", "导入的abc解算文件"))
	      .Font(Font)
		]
        /// 主要的列表小部件(Abc)
        + SVerticalBox::Slot()
		.FillHeight(3.0f)
		.VAlign(VAlign_Top)
        .Padding(2.0f)
        [
          SAssignNew(ListImportAbc,SListView<TSharedPtr<doodle_ue4::FAbcImport>>)
          .ItemHeight(80) // 小部件高度
          .ListItemsSource(&ListImportAbcData)
          .ScrollbarVisibility(EVisibility::All)
          .OnGenerateRow_Lambda(// 生成小部件
            [](TSharedPtr<doodle_ue4::FAbcImport> InItem, 
               const TSharedRef<STableViewBase>& OwnerTable) -> TSharedRef<ITableRow> {
              return SNew(SDoodleImportAbcUiItem, OwnerTable)
                    .ItemShow(InItem);
            }
          )
          .SelectionMode(ESelectionMode::Type::Single) //单选
          .HeaderRow ///题头元素
          (
            SNew(SHeaderRow)
            + SHeaderRow::Column(TEXT("Abc File"))
            [
              SNew(SBorder)
              .Padding(5)
              [
                SNew(STextBlock)
                .Text(FText::FromString(TEXT("Abc File")))
              ]
            ]
            +SHeaderRow::Column(TEXT("Time Ranges"))
            .DefaultLabel(LOCTEXT("Time Ranges","Time Ranges"))

            +SHeaderRow::Column(TEXT("Import Path Dir"))
            .DefaultLabel(LOCTEXT("Import Path Dir","Import Path Dir"))
          )
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        .VAlign(VAlign_Center)
        .Padding(2.0f)
        [
          SNew(SHorizontalBox)
          +SHorizontalBox::Slot()
          .FillWidth(1.0f)
          [
            SNew(SButton)
            .Text(LOCTEXT("Search USkeleton","Search USkeleton"))
            .ToolTipText(LOCTEXT("Search USkeleton Tip","寻找骨骼"))
            .OnClicked_Lambda([this](){
               GetAllSkinObjs();
               MatchFbx();
               ListImportFbx->RebuildList();
			   ListImportFbxCam->RebuildList();
			   ListImportAbc->RebuildList();
               return FReply::Handled();
            })
          ]
          +SHorizontalBox::Slot()
          .FillWidth(1.0f)
          [
            SNew(SButton)
            .Text(LOCTEXT("Search USkeleton And Import","Search USkeleton And Import"))
            .ToolTipText(LOCTEXT("Search USkeleton Tip2","寻找骨骼并导入Fbx"))
            .OnClicked_Lambda([this](){
               GetAllSkinObjs();
               MatchFbx();
               ImportFbx();
               ImportAbc();
               ListImportFbx->RebuildList();
			   ListImportFbxCam->RebuildList();
			   ListImportAbc->RebuildList();
               return FReply::Handled();
            })
          ]
          +SHorizontalBox::Slot()
          .FillWidth(1.0f)
          [
            SNew(SButton)
            .Text(LOCTEXT("Search USkeleton Import","Search USkeleton Direct Import"))
            .ToolTipText(LOCTEXT("Search USkeleton Tip3","不寻找骨骼, 直接导入 Fbx, 如果已经寻找过则使用寻找的数据"))
            .OnClicked_Lambda([this](){
               ImportFbx();
               ImportAbc();
               ListImportFbx->RebuildList();
			   ListImportFbxCam->RebuildList();
			   ListImportAbc->RebuildList();
               return FReply::Handled();
            })
          ]
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        .VAlign(VAlign_Center)
        .Padding(2.0f)
        [
          SNew(SButton)
          .Text(LOCTEXT("Clear USkeleton","Clear USkeleton"))
          .ToolTipText(LOCTEXT("Clear USkeleton Tip","清除所有"))
          .OnClicked_Lambda([this](){
    //         this->ListImportFbxData.Empty();
    //         this->ListImportAbcData.Empty();
			 //this->ListImportFbxCamData.Empty();
			 //ListImportFbxCam->RebuildList();
    //         ListImportFbx->RebuildList();
			 //ListImportAbc->RebuildList();
			 this->ImportCamera();
             return FReply::Handled();
          })
        ]
      ]



    ];
  // clang-format on
}

void SDoodleImportFbxUI::AddReferencedObjects(FReferenceCollector& collector) {}

TSharedRef<SDockTab> SDoodleImportFbxUI::OnSpawnAction(const FSpawnTabArgs& SpawnTabArgs) {
  return SNew(SDockTab).TabRole(ETabRole::NomadTab)[SNew(SDoodleImportFbxUI)];  // 这里创建我们自己的界面
}

void SDoodleImportFbxUI::GetAllSkinObjs() {
  FScopedSlowTask L_Task_Scoped{2.0f, LOCTEXT("Import_Fbx2", "扫描所有的Skin")};
  L_Task_Scoped.MakeDialog();

  this->AllSkinObjs.Empty();
  FARFilter LFilter{};
  LFilter.bIncludeOnlyOnDiskAssets = false;
  LFilter.bRecursivePaths          = true;
  LFilter.bRecursiveClasses        = true;

#if (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 26) || \
    (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION == 27) || \
    (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION == 0)
  LFilter.ClassNames.Add(FName{USkeleton::StaticClass()->GetName()});
#else if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION == 1)
  LFilter.ClassPaths.Add(USkeleton::StaticClass()->GetClassPathName());
#endif

  IAssetRegistry::Get()->EnumerateAssets(LFilter, [this](const FAssetData& InAss) -> bool {
    USkeleton* L_SK = Cast<USkeleton>(InAss.GetAsset());
    if (L_SK) {
      doodle_ue4::FUSkeletonData& L_Ref_Data = this->AllSkinObjs.Emplace_GetRef();
      L_Ref_Data.SkinObj                     = L_SK;

      for (auto&& L_Item : L_SK->GetReferenceSkeleton().GetRawRefBoneInfo())
        L_Ref_Data.BoneNames.Add(L_Item.ExportName);
    }
    return true;
  });
  L_Task_Scoped.EnterProgressFrame(1.0f);
  SetAllSkinTag();
  L_Task_Scoped.EnterProgressFrame(1.0f);
  // LFilter.ClassNames.Add(FName{USkeletalMesh::StaticClass()->GetName()});
  // IAssetRegistry::Get()->EnumerateAssets(LFilter, [&, this](const FAssetData& InAss) -> bool {
  //   USkeletalMesh* L_SK = Cast<USkeletalMesh>(InAss.GetAsset());
  //   if (L_SK && L_SK->GetSkeleton()) {
  //     FString L_BaseName = FPaths::GetBaseFilename(L_SK->GetAssetImportData()->GetFirstFilename());
  //     this->AllSkinObjs_Map.Add(MakeTuple(L_BaseName, L_SK->GetSkeleton()));
  //   }
  //   return true;
  // });
}

void SDoodleImportFbxUI::MatchFbx() {
  UnFbx::FFbxImporter* FbxImporter = UnFbx::FFbxImporter::GetInstance();
  FbxImporter->ClearAllCaches();

  TArray<TSharedPtr<doodle_ue4::FFbxImport>> L_RemoveList;
  FScopedSlowTask L_Task_Scoped{(float_t)ListImportFbxData.Num(), LOCTEXT("DoingSlowWork1", "加载 fbx 文件中...")};
  L_Task_Scoped.MakeDialog();
  // FString L_Debug_str{};

  for (auto&& L_Fbx_Path : ListImportFbxData) {
    grop_SDoodleImportFbxUI L_grop_SDoodleImportFbxUI{FbxImporter};
    FbxImporter->ImportFromFile(L_Fbx_Path->ImportFbxPath, FPaths::GetExtension(L_Fbx_Path->ImportFbxPath));

    TArray<fbxsdk::FbxNode*> L_Fbx_Node_list{};
    FString L_NameSpace{};
    L_Task_Scoped.EnterProgressFrame(1.0f);

    FScopedSlowTask L_Task_Scoped2{
        (float_t)FbxImporter->Scene->GetNodeCount(), LOCTEXT("DoingSlowWork2", "扫描 fbx 文件骨骼中...")};
    for (size_t i = 0; i < FbxImporter->Scene->GetNodeCount(); ++i) {
      FString L_Name = MakeName(FbxImporter->Scene->GetNode(i)->GetName());
      L_Fbx_Path->FbxNodeNames.Add(L_Name);
      // 获取名称空间
      if (L_NameSpace.IsEmpty()) L_NameSpace = GetNamepace(FbxImporter->Scene->GetNode(i)->GetName());

      L_Task_Scoped2.EnterProgressFrame(1.0f);
    }
    TArray<fbxsdk::FbxCamera*> L_Cameras{};
    MovieSceneToolHelpers::GetCameras(FbxImporter->Scene->GetRootNode(), L_Cameras);
    if (!L_Cameras.IsEmpty()) {
      ListImportFbxCamData.Emplace(MakeShared<doodle_ue4::FFbxCameraImport>(L_Fbx_Path->ImportFbxPath));
    }
    if (L_NameSpace.IsEmpty()) {
      L_RemoveList.Add(L_Fbx_Path);
      continue;
    }

    FScopedSlowTask L_Task_Scoped3{
        (float_t)FbxImporter->Scene->GetNodeCount(), LOCTEXT("DoingSlowWork3", "寻找 fbx 文件骨骼匹配的SK中...")};
    for (auto&& L_SK_Data : this->AllSkinObjs) {
      FString L_BaseName = FPaths::GetBaseFilename(L_Fbx_Path->ImportFbxPath);
      if (L_Fbx_Path->FbxNodeNames.Num() >= L_SK_Data.BoneNames.Num()) {
        if ((
                L_SK_Data.SkinTag.IsEmpty()
                    ? true
                    : L_BaseName.Find(L_SK_Data.SkinTag) != INDEX_NONE
            )  /// 先确认字串节省资源
            && Algo::AllOf(L_SK_Data.BoneNames, [&](const FString& IN_Str) {
                 return L_Fbx_Path->FbxNodeNames.Contains(IN_Str);
               })  /// 进一步确认骨骼内容
        )
          L_Fbx_Path->SkinObj = L_SK_Data.SkinObj;
        L_Task_Scoped3.EnterProgressFrame(1.0f);
      }
    }
  }

  /// 删除没有名称空间的
  for (auto&& L_R : L_RemoveList) {
    ListImportFbxData.RemoveSingle(L_R);
  }
}

void SDoodleImportFbxUI::ImportFbx() {
  TArray<UAssetImportTask*> ImportDataList{};
  for (auto&& L_Fbx : ListImportFbxData) {
    UAssetImportTask* l_task                                       = NewObject<UAssetImportTask>();
    l_task->bAutomated                                             = true;
    l_task->bReplaceExisting                                       = true;
    l_task->bReplaceExistingSettings                               = true;
    /// @fix 此处不需要自动保存, 否则会出现一个ue4.27中发现的bug 会将
    /// UAssetImportTask::ImportedObjectPaths 值转换为乱码

    // l_task->bSave = true;
    l_task->DestinationPath                                        = L_Fbx->ImportPathDir;
    l_task->Filename                                               = L_Fbx->ImportFbxPath;

    l_task->Factory                                                = DuplicateObject<UFbxFactory>(GetDefault<UFbxFactory>(), l_task);

    UFbxFactory* k_fbx_f                                           = Cast<UFbxFactory>(l_task->Factory);

    k_fbx_f->ImportUI->MeshTypeToImport                            = FBXIT_SkeletalMesh;
    k_fbx_f->ImportUI->OriginalImportType                          = FBXIT_SkeletalMesh;
    k_fbx_f->ImportUI->bImportAsSkeletal                           = true;
    k_fbx_f->ImportUI->bImportMesh                                 = true;
    k_fbx_f->ImportUI->bImportAnimations                           = true;
    k_fbx_f->ImportUI->bImportRigidMesh                            = true;
    k_fbx_f->ImportUI->bImportMaterials                            = false;
    k_fbx_f->ImportUI->bImportTextures                             = false;
    k_fbx_f->ImportUI->bResetToFbxOnMaterialConflict               = false;

    k_fbx_f->ImportUI->SkeletalMeshImportData->bImportMorphTargets = true;
    k_fbx_f->ImportUI->bAutomatedImportShouldDetectType            = false;
    k_fbx_f->ImportUI->AnimSequenceImportData->AnimationLength     = FBXALIT_ExportedTime;
    k_fbx_f->ImportUI->AnimSequenceImportData->bImportBoneTracks   = true;
    k_fbx_f->ImportUI->bAllowContentTypeImport                     = true;
    k_fbx_f->ImportUI->TextureImportData->MaterialSearchLocation   = EMaterialSearchLocation::UnderRoot;
    if (L_Fbx->SkinObj) {
      k_fbx_f->ImportUI->Skeleton                                    = L_Fbx->SkinObj;
      k_fbx_f->ImportUI->MeshTypeToImport                            = FBXIT_Animation;
      k_fbx_f->ImportUI->OriginalImportType                          = FBXIT_SkeletalMesh;
      k_fbx_f->ImportUI->bImportAsSkeletal                           = true;
      k_fbx_f->ImportUI->bImportMesh                                 = false;
      k_fbx_f->ImportUI->bImportAnimations                           = true;
      k_fbx_f->ImportUI->bImportRigidMesh                            = false;
      k_fbx_f->ImportUI->bImportMaterials                            = false;
      k_fbx_f->ImportUI->bImportTextures                             = false;
      k_fbx_f->ImportUI->bResetToFbxOnMaterialConflict               = false;

      k_fbx_f->ImportUI->SkeletalMeshImportData->bImportMorphTargets = true;
      k_fbx_f->ImportUI->bAutomatedImportShouldDetectType            = false;
      k_fbx_f->ImportUI->AnimSequenceImportData->AnimationLength     = FBXALIT_ExportedTime;
      k_fbx_f->ImportUI->AnimSequenceImportData->bImportBoneTracks   = true;
      k_fbx_f->ImportUI->bAllowContentTypeImport                     = true;
    }
    ImportDataList.Add(l_task);
  }
  FAssetToolsModule& AssetToolsModule =
      FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");
  TArray<FString> import_Paths{};
  AssetToolsModule.Get().ImportAssetTasks(ImportDataList);
}

void SDoodleImportFbxUI::ImportAbc() {
  TArray<UAssetImportTask*> ImportDataList{};
  for (auto&& L_Abc : ListImportAbcData) {
    UAssetImportTask* l_task = NewObject<UAssetImportTask>();
    for (TObjectIterator<UClass> it{}; it; ++it) {
      if (it->IsChildOf(UFactory::StaticClass())) {
        if (it->GetName() == "AlembicImportFactory") {
          l_task->Factory = it->GetDefaultObject<UFactory>();
        }
      }
    }
    l_task->bAutomated               = true;
    l_task->bReplaceExisting         = true;
    l_task->bReplaceExistingSettings = true;
    l_task->DestinationPath          = L_Abc->ImportPathDir;
    l_task->Filename                 = L_Abc->ImportAbcPath;

    /// 获取abc默认设置并修改
    UAbcImportSettings* k_abc_stting = UAbcImportSettings::Get();
    // UAbcImportSettings *k_abc_stting = DuplicateObject<
    //     UAbcImportSettings>(UAbcImportSettings::Get(), l_task);
    k_abc_stting->ImportType =
        EAlembicImportType::GeometryCache;                    // 导入为几何缓存
    k_abc_stting->MaterialSettings.bCreateMaterials = false;  // 不创建材质
    k_abc_stting->MaterialSettings.bFindMaterials   = true;   // 寻找材质
    k_abc_stting->ConversionSettings.Preset =
        EAbcConversionPreset::Max;  // 导入预设为3dmax
    k_abc_stting->ConversionSettings.bFlipV            = true;
    k_abc_stting->ConversionSettings.Scale.X           = 1.0;
    k_abc_stting->ConversionSettings.Scale.Y           = -1.0;
    k_abc_stting->ConversionSettings.Scale.Z           = 1.0;
    k_abc_stting->ConversionSettings.Rotation.X        = 90.0;
    k_abc_stting->ConversionSettings.Rotation.Y        = 0.0;
    k_abc_stting->ConversionSettings.Rotation.Z        = 0.0;

    k_abc_stting->GeometryCacheSettings.bFlattenTracks = true;              // 合并轨道
    k_abc_stting->SamplingSettings.bSkipEmpty          = true;              // 跳过空白帧
    k_abc_stting->SamplingSettings.FrameStart          = L_Abc->StartTime;  // 开始帧
    k_abc_stting->SamplingSettings.FrameEnd            = L_Abc->EndTime;    // 结束帧
    k_abc_stting->SamplingSettings.FrameSteps          = 1;                 // 帧步数

    l_task->Options                                    = k_abc_stting;
    ImportDataList.Emplace(l_task);
  }
  FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");
  TArray<FString> import_Paths{};
  AssetToolsModule.Get().ImportAssetTasks(ImportDataList);
}

void SDoodleImportFbxUI::ImportCamera() {
  ListImportFbxCamData.Empty();
  auto& L_C          = ListImportFbxCamData.Emplace_GetRef(MakeShared<doodle_ue4::FFbxCameraImport>(
      "F:\\doodle_plug_dev_4.27\\test_file\\ad\\RJ_EP029_SC008_AN_camera_1001-1096.fbx"
  ));
  L_C->ImportPathDir = "/Game/TEST/2/LV";

  int32 in_start     = 1001;
  int32 in_end       = 1200;

  auto& l_ass_tool   = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
  for (auto&& Cam : ListImportFbxCamData) {
    ULevelSequence* L_ShotSequence = LoadObject<ULevelSequence>(nullptr, *Cam->ImportPathDir);

    // 创建定序器
    if (!L_ShotSequence) {
      for (TObjectIterator<UClass> it{}; it; ++it) {
        if (it->IsChildOf(UFactory::StaticClass())) {
          if (it->GetName() == "LevelSequenceFactoryNew") {
            L_ShotSequence = CastChecked<ULevelSequence>(l_ass_tool.CreateAsset(
                FPaths::GetBaseFilename(Cam->ImportPathDir), FPaths::GetPath(Cam->ImportPathDir),
                ULevelSequence::StaticClass(), it->GetDefaultObject<UFactory>()
            ));
          }
        }
      }
    }
    // 设置定序器属性
    L_ShotSequence->GetMovieScene()->SetDisplayRate(FFrameRate{25, 1});
    L_ShotSequence->GetMovieScene()->SetTickResolutionDirectly(FFrameRate{25, 1});
    L_ShotSequence->GetMovieScene()->Modify();

    /// 设置范围
    L_ShotSequence->GetMovieScene()->SetWorkingRange((in_start - 10) / 25, (in_end + 10) / 25);
    L_ShotSequence->GetMovieScene()->SetViewRange((in_start - 10) / 25, (in_end + 10) / 25);
    L_ShotSequence->GetMovieScene()->SetPlaybackRange(TRange<FFrameNumber>{in_start, in_end}, true);
    L_ShotSequence->Modify();

    // FSoftObjectPath L_LevelSequenceSoftPath{Cam->ImportPathDir};
    // UObject* L_LoadedObject                       = L_LevelSequenceSoftPath.TryLoad();
    UAssetEditorSubsystem* L_AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
    L_AssetEditorSubsystem->OpenEditorForAsset(L_ShotSequence);

    IAssetEditorInstance* L_AssetEditor = L_AssetEditorSubsystem->FindEditorForAsset(L_ShotSequence, true);

    ILevelSequenceEditorToolkit* L_LevelSequenceEditor = static_cast<ILevelSequenceEditorToolkit*>(L_AssetEditor);
    ISequencer* L_ShotSequencer                        = L_LevelSequenceEditor->GetSequencer().Get();
    UMovieScene* L_Move                                = L_ShotSequence->GetMovieScene();
    ACineCameraActor* L_CameraActor{};
    // 相机task
    UMovieSceneTrack* L_Task = L_ShotSequence->GetMovieScene()->GetCameraCutTrack();
    if (!L_Task) FSequencerUtilities::CreateCamera(L_ShotSequencer->AsShared(), true, L_CameraActor);
    L_Task = L_ShotSequence->GetMovieScene()->GetCameraCutTrack();

    // Cast<FStructProperty>(L_Task->GetClass()->FindPropertyByName("CameraBindingID"))->;

    UCameraComponent* L_cam{};
    for (auto&& L_Section : L_Task->GetAllSections()) {
      L_cam =
          Cast<UMovieSceneCameraCutSection>(L_Section)->GetFirstCamera(*L_ShotSequencer, MovieSceneSequenceID::Root);
      if (L_cam) break;
    }
    if (!L_cam) return;

    UE_LOG(LogTemp, Log, TEXT("camera name %s"), *L_cam->GetOwner()->GetActorNameOrLabel());
    if (!L_CameraActor) L_CameraActor = Cast<ACineCameraActor>(L_cam->GetOwner());

    // MovieSceneToolHelpers::CreateCameraCutSectionForCamera(L_Move,)

    FGuid l_cam_guid{};
    for (auto i = 0; i < L_Move->GetSpawnableCount(); i++) {
      if (L_Move->GetSpawnable(i).GetObjectTemplate()->GetClass()->IsChildOf(ACameraActor::StaticClass())) {
        l_cam_guid = L_Move->GetSpawnable(i).GetGuid();
        break;
      }
    }

    for (auto i = 0; i < L_Move->GetPossessableCount(); i++) {
      if (L_Move->GetPossessable(i).GetPossessedObjectClass()->IsChildOf(ACameraActor::StaticClass())) {
        l_cam_guid = L_Move->GetPossessable(i).GetGuid();
        break;
      }
    }

    UE_LOG(LogTemp, Log, TEXT("guid %s"), *l_cam_guid.ToString());

    // L_Move->Find;
    UE_LOG(
        LogTemp, Log, TEXT("guid2 %s"),
        *L_ShotSequence->FindPossessableObjectId(*L_CameraActor, L_ShotSequence).ToString()
    );
    UE_LOG(
        LogTemp, Log, TEXT("guid3 %s"), *L_ShotSequence->FindBindingFromObject(L_CameraActor, L_ShotSequence).ToString()
    );
    UE_LOG(
        LogTemp, Log, TEXT("guid4 %s"),
        *L_ShotSequence->FindPossessableObjectId(*L_CameraActor, L_CameraActor->GetWorld()).ToString()
    );
    UE_LOG(
        LogTemp, Log, TEXT("guid5 %s"),
        *L_ShotSequence->FindBindingFromObject(L_CameraActor, L_CameraActor->GetWorld()).ToString()
    );
    // L_Move->GetCameraCutTrack()->;
    TMap<FGuid, FString> L_Map{};
    L_Map.Add(l_cam_guid, L_cam->GetOwner()->GetActorNameOrLabel());
    // 打开fbx
    UnFbx::FFbxImporter* FbxImporter = UnFbx::FFbxImporter::GetInstance();
    FbxImporter->ClearAllCaches();
    FbxImporter->ImportFromFile(Cam->ImportFbxPath, FPaths::GetExtension(Cam->ImportFbxPath));

    MovieSceneToolHelpers::ImportFBXCameraToExisting(
        FbxImporter, L_ShotSequence, L_ShotSequencer, MovieSceneSequenceID::Root, L_Map, false, true
    );
  }
}

FString SDoodleImportFbxUI::GetImportPath(const FString& In_Path) {
  FRegexPattern L_Reg_Ep_Pattern{LR"([ep|EP|Ep]_?(\d+))"};
  FRegexMatcher L_Reg_Ep{L_Reg_Ep_Pattern, In_Path};
  int64 L_eps{};
  int64 L_sc{};
  FString L_Sc_ab{};

  if (L_Reg_Ep.FindNext()) {
    L_eps = FCString::Atoi64(*L_Reg_Ep.GetCaptureGroup(1));
  }

  FRegexPattern L_Reg_ScPattern{LR"([sc|SC|Sc]_?(\d+)([a-z])?)"};
  FRegexMatcher L_Reg_Sc{L_Reg_ScPattern, In_Path};

  if (L_Reg_Sc.FindNext()) {
    L_sc = FCString::Atoi64(*L_Reg_Sc.GetCaptureGroup(1));
    if (L_Reg_Sc.GetEndLimit() > 2) {
      L_Sc_ab = L_Reg_Sc.GetCaptureGroup(2);
    }
  }

  FString L_Path = FString::Printf(
      TEXT("/Game/Shot/ep%.4d/%s%.4d_sc%.4d%s"),
      L_eps, *Path_Prefix, L_eps, L_sc, *L_Sc_ab
  );
  return L_Path;
}

void SDoodleImportFbxUI::GenPathPrefix(const FString& In_Path_Prefix) {
  Path_Prefix = In_Path_Prefix;
  for (auto&& L_Fbx : ListImportFbxData) {
    L_Fbx->ImportPathDir = GetImportPath(L_Fbx->ImportFbxPath) / "Fbx_Import";
  }
  for (auto&& L_Abc : ListImportAbcData) {
    L_Abc->ImportPathDir = GetImportPath(L_Abc->ImportAbcPath) / "Abcs_Import";
  }
  for (auto&& L_Cam : ListImportFbxCamData) {
    L_Cam->ImportPathDir = GetImportPath(L_Cam->ImportFbxPath) / "Abcs_Import";
  }
}

void SDoodleImportFbxUI::SetAllSkinTag() {
  FRegexPattern L_Reg_Ep_Pattern{LR"(SK_(\w+)_Skeleton)"};
  for (auto&& L_Sk : AllSkinObjs) {
    FRegexMatcher L_Reg{L_Reg_Ep_Pattern, L_Sk.SkinObj->GetName()};
    if (L_Reg.FindNext())
      L_Sk.SkinTag = L_Reg.GetCaptureGroup(1);
  }
}

TTuple<int32_t, int32_t> SDoodleImportFbxUI::GenStartAndEndTime(const FString& In_Path) {
  FRegexPattern L_Reg_Time_Pattern{LR"(_(\d+)-(\d+))"};
  FRegexMatcher L_Reg_Time{L_Reg_Time_Pattern, FPaths::GetBaseFilename(In_Path)};
  int64 L_Start{1000};
  int64 L_End{1001};

  if (L_Reg_Time.FindNext() && L_Reg_Time.GetEndLimit() > 2) {
    L_Start = FCString::Atoi64(*L_Reg_Time.GetCaptureGroup(1));
    L_End   = FCString::Atoi64(*L_Reg_Time.GetCaptureGroup(2));
  }
  return MakeTuple(L_Start, L_End);
}

void SDoodleImportFbxUI::AddFile(const FString& In_File) {
  /// @brief 先扫描前缀
  if (this->Path_Prefix.IsEmpty()) {
    int32 L_Index      = INDEX_NONE;
    FString L_FileName = FPaths::GetBaseFilename(In_File);
    if (L_FileName.FindChar('_', L_Index)) {
      L_FileName.LeftChopInline(L_FileName.Len() - L_Index, true);
      this->Path_Prefix = L_FileName;
    }
  }
  /// 扫描fbx 和abc 文件
  if (FPaths::FileExists(In_File) && FPaths::GetExtension(In_File, true) == TEXT(".fbx")) {
    /// @brief 寻找到相同的就跳过
    if (ListImportFbxData.FindByPredicate([&](const TSharedPtr<doodle_ue4::FFbxImport>& In_FBx) {
          return In_FBx->ImportFbxPath == In_File;
        })) {
      return;
    };

    TSharedPtr<doodle_ue4::FFbxImport> L_ptr = MakeShared<doodle_ue4::FFbxImport>(In_File);
    ListImportFbxData.Emplace(L_ptr);
  }
  if (FPaths::FileExists(In_File) && FPaths::GetExtension(In_File, true) == TEXT(".abc")) {
    /// @brief 寻找到相同的就跳过
    if (ListImportAbcData.FindByPredicate([&](const TSharedPtr<doodle_ue4::FAbcImport>& In_Abc) {
          return In_Abc->ImportAbcPath == In_File;
        })) {
      return;
    };

    TSharedPtr<doodle_ue4::FAbcImport> L_ptr = MakeShared<doodle_ue4::FAbcImport>(In_File);
    TTuple<int32_t, int32_t> L_Time_Ranges   = this->GenStartAndEndTime(In_File);
    L_ptr->StartTime                         = L_Time_Ranges.Get<0>();
    L_ptr->EndTime                           = L_Time_Ranges.Get<1>();

    ListImportAbcData.Emplace(L_ptr);
  }
}
void SDoodleImportFbxUI::CreateSequencer(const FString& In_CreatePath) {}
// DragBegin
FReply SDoodleImportFbxUI::OnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent) {
  auto L_Opt = InDragDropEvent.GetOperationAs<FExternalDragOperation>();
  return L_Opt && L_Opt->HasFiles() ? FReply::Handled() : FReply::Unhandled();
}

FReply SDoodleImportFbxUI::OnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent) {
  auto L_Opt = InDragDropEvent.GetOperationAs<FExternalDragOperation>();

  if (!(L_Opt && L_Opt->HasFiles())) return FReply::Unhandled();

  FScopedSlowTask L_Task_Scoped{6.0f, LOCTEXT("Import_Fbx", "加载 Fbx abc")};
  L_Task_Scoped.MakeDialog();

  ListImportFbxData.Empty();
  ListImportAbcData.Empty();
  ListImportFbxCamData.Empty();
  AllSkinObjs.Empty();

  for (auto&& Path : L_Opt->GetFiles()) {
    if (FPaths::DirectoryExists(Path)) {
      // 目录进行迭代
      IFileManager::Get().IterateDirectoryRecursively(*Path, [this](const TCHAR* InPath, bool in_) -> bool {
        AddFile(InPath);
        return true;
      });
    } else if (FPaths::FileExists(Path)) {
      // 文件直接添加
      AddFile(Path);
    }
  }

  L_Task_Scoped.EnterProgressFrame(1.0f);
  GenPathPrefix(this->Path_Prefix);

  L_Task_Scoped.EnterProgressFrame(1.0f);
  GetAllSkinObjs();
  L_Task_Scoped.EnterProgressFrame(3.0f);
  MatchFbx();
  L_Task_Scoped.EnterProgressFrame(1.0f);
  ListImportFbx->RebuildList();
  ListImportAbc->RebuildList();
  ListImportFbxCam->RebuildList();

  return FReply::Handled();
}

// DragEnd

#undef LOCTEXT_NAMESPACE