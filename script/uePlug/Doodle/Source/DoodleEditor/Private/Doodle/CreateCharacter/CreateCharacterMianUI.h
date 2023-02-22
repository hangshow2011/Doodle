#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

class SCreateCharacterMianUI : public SCompoundWidget, FGCObject {
 public:
  SLATE_BEGIN_ARGS(SCreateCharacterMianUI) {}
  SLATE_END_ARGS()

  // ���������ݴ�������
  void Construct(const FArguments& Arg);

  // ��������
  virtual void AddReferencedObjects(FReferenceCollector& collector) override;

  const static FName Name;

  static TSharedRef<SDockTab> OnSpawnAction(const FSpawnTabArgs& SpawnTabArgs);
};