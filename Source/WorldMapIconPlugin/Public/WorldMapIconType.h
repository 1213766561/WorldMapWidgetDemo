// Copyright 2026 CJ. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/Texture2D.h"
#include "Engine/DataTable.h"
#include "WorldMapIconType.generated.h"

/**
 * World map icon related types and utilities.
 * 世界地图图标相关类型与工具函数声明
 */

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSoftClassLoad, UClass*, LoadedObject);

USTRUCT(BlueprintType)
struct FMapSettingStruct : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	// 地图名字（唯一标识）
	// Map name (unique identifier)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapSettingStruct")
	FName MapName = NAME_None;

	// 世界地图左上角位置（世界坐标）
	// World map top-left position (in world coordinates)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapSettingStruct")
	FVector2D WorldMap_LeftTop = FVector2D::ZeroVector;

	// 地图在世界中的尺寸（世界单位）
	// Map size in world units (world scale)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapSettingStruct")
	FVector2D WorldMap_WorldScale;

	// 地图在UI或贴图上的尺寸（贴图/地图坐标）
	// Map size in map/texture coordinates (map scale)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapSettingStruct")
	FVector2D WorldMap_MapScale;

	// 地图贴图（延迟加载）
	// Map texture (soft pointer for async loading)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapSettingStruct")
	TSoftObjectPtr<UTexture2D> WorldMap_Texture;
};

USTRUCT(BlueprintType)
struct FMapIconStruct
{
	GENERATED_USTRUCT_BODY()
public:
	// 唯一标识ID（地图图标实例ID）
	// Unique identifier ID (map icon instance ID)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapIconStruct")
	int MapIcon_ID = -1;

	// 类型（用于查找对应的图标样式/Widget）
	// Type (used to lookup icon style / widget)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapIconStruct")
	FName MapIcon_Type = TEXT("Default");

	// 附加属性字典：比如 Location/Rotation/Scale/OwnID 等，使用字符串存储以便导表和序列化
	// Extra parameter map: e.g. Location/Rotation/Scale/OwnID etc. Stored as strings for data table/serialization convenience.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapIconStruct")
	TMap<FName, FString> MapIcon_ExtraPram;

	FMapIconStruct() {};
	FMapIconStruct(int _MapIcon_ID)
		:MapIcon_ID(_MapIcon_ID)
	{

	};

	// 将目标数据合并到当前实例，并把发生变化的键加入 ChangedList（用于增量更新）
	// Merge target map icon data into this instance. Any keys that change are appended to ChangedList (used for incremental updates).
	void UpdateMapIconData(const FMapIconStruct& TargetMapIconStruct, TArray<FName>& ChangedList)
	{
		MapIcon_Type = TargetMapIconStruct.MapIcon_Type;
		for (auto& T : TargetMapIconStruct.MapIcon_ExtraPram)
		{
			if (!MapIcon_ExtraPram.Contains(T.Key))
			{
				ChangedList.Add(T.Key);
				MapIcon_ExtraPram.Add(T.Key, T.Value);
				continue;
			}
			if (*MapIcon_ExtraPram.Find(T.Key) != T.Value)
			{
				ChangedList.Add(T.Key);
				MapIcon_ExtraPram[T.Key] = T.Value;
				continue;
			}
		}
	};
};

USTRUCT(BlueprintType)
struct FMapIconSettingStruct : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	// 图标类型名（用于匹配 FMapIconStruct::MapIcon_Type）
	// Icon type name (used to match FMapIconStruct::MapIcon_Type)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapIconSetting")
	FName MapIconType = NAME_None;

	// 图标对应的 Widget Class（建议继承自 UWorldMapIconInterface）
	// Widget class for the icon (recommended to implement UWorldMapIconInterface)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapIconSetting")
	TSoftClassPtr<class UObject> MapIconWidget;

	// 图标基础贴图（可选）
	// Base texture for the icon (optional)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapIconSetting")
	TSoftObjectPtr<UTexture2D> MapIcon_BaseTexture = nullptr;

	// 图标显示尺寸（像素）
	// Base display size for the icon (in pixels)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapIconSetting")
	FVector2D MapIcon_BaseSize = FVector2D(64.f);
};

UENUM(BlueprintType)
enum class EWorldMapType : uint8
{
	// 无（未指定）
	// None (unspecified)
	None,
	// 大地图（全局/世界地图）
	// World map (global)
	WorldMap,
	// 小地图（局部/迷你地图）
	// Mini world map (local / minimap)
	MiniWorldMap
};

UCLASS()
class WORLDMAPICONPLUGIN_API UWorldMapIconType : public UObject
{
	GENERATED_BODY()
public:
	/**
	 * 将世界坐标转换为地图坐标
	 * Converts world coordinates to map coordinates.
	 *
	 * 参数说明：
	 * @param WorldPos 世界坐标位置（3D）
	 * @param WorldMap_WorldRoot 世界地图的参考原点（世界坐标，通常为地图左上角或指定参考点）
	 * @param WorldMap_WorldScale 世界坐标空间中的地图尺寸（用于归一化）
	 * @param WorldMap_MapScale 地图/纹理坐标空间中的地图尺寸（用于缩放到贴图或UI）
	 * @return 返回 FVector2D，表示在地图坐标/像素中的位置
	 */
	UFUNCTION(BlueprintPure, Category = "World Map|Utilities")
	static FVector2D WorldToMapCoordinates(const FVector& WorldPos,
		const FVector& WorldMap_WorldRoot,
		const FVector2D& WorldMap_WorldScale,
		const FVector2D& WorldMap_MapScale);

	/**
	 * 将地图坐标转换为世界坐标
	 * Converts map coordinates to world coordinates.
	 *
	 * 参数说明：
	 * @param MapPos 地图坐标（2D）
	 * @param WorldMap_WorldRoot 世界地图的参考原点（世界坐标）
	 * @param WorldMap_WorldScale 世界坐标空间中的地图尺寸（用于反算）
	 * @param WorldMap_MapScale 地图/纹理坐标空间中的地图尺寸（用于反算）
	 * @return 返回 FVector，表示世界空间中的位置
	 */
	UFUNCTION(BlueprintPure, Category = "World Map|Utilities")
	static FVector MapToWorldCoordinates(const FVector2D& MapPos,
		const FVector& WorldMap_WorldRoot,
		const FVector2D& WorldMap_WorldScale,
		const FVector2D& WorldMap_MapScale);

	/**
	 * 异步加载一个软引用的 Class 并在加载完成后回调
	 * Asynchronously load a soft class pointer and invoke the delegate when loaded.
	 *
	 * @param SoftClassPtr 要加载的软类引用（TSoftClassPtr）
	 * @param LoadedDelegate 加载完成后的回调，回传 UClass*（如果加载失败可能为 nullptr）
	 */
	UFUNCTION(BlueprintCallable, Category = "World Map|Utilities", meta = (AutoCreateRefTerm = "SoftClassPtr"))
	static void AsyncLoadClass(const TSoftClassPtr<UObject>& SoftClassPtr, FOnSoftClassLoad LoadedDelegate);
};