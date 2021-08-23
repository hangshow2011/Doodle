// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DoodleAssetImportData.generated.h"


class UFactory;
class FJsonObject;

/**
 * 
 */
UCLASS()
class DOODLEEDITOR_API UDoodleAssetImportData : public UObject
{
	GENERATED_BODY()
public:
    enum class import_file_type : uint8 {
        None,
        Abc,
        Fbx
    };

    bool is_valid() const;

    void initialize(TSharedPtr<FJsonObject> InImportGroupJsonData);
public:
    /* �����ļ���·��(�ļ�����) */
    FString import_file_path;
    /* �����ļ���·��(Ŀ¼) */
    FString import_file_save_dir;

    /* �����ļ�ʱfbx skeleton �����ļ���*/
    FString fbx_skeleton_dir;
    /* �����ļ�ʱfbx skeleton �ļ����� */
    FString fbx_skeleton_file_name;


    /* �����ļ�ʱ��json ���� */
    TSharedPtr<FJsonObject> ImportGroupJsonData;

    import_file_type import_type;

    std::uint64_t start_frame;
    std::uint64_t end_frame;
};
