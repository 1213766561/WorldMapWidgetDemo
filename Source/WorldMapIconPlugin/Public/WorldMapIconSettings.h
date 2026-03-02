// Copyright 2026 CJ. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "WorldMapIconType.h"
#include "WorldMapIconSettings.generated.h"

/**
 * 小地图插件全局配置（开发者设置）
 * World map icon plugin global settings (Developer settings)
 *
 * 这个类通过 UE 的 DeveloperSettings 系统暴露可在编辑器中配置的参数，
 * 并且会被序列化到配置文件（Config = WorldMapIconSettings）。
 * This class exposes editable plugin settings through UE's DeveloperSettings system
 * and will be serialized to the configuration file (Config = WorldMapIconSettings).
 */
UCLASS(Config = WorldMapIconSettings, defaultconfig, meta = (DisplayName = "小地图配置"))
class WORLDMAPICONPLUGIN_API UWorldMapIconSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	// 构造函数：用于初始化默认值
	// Constructor: initialize defaults
	UWorldMapIconSettings();

	// 获取设置容器名称（Project 或 Editor）
	// Gets the settings container name (Project or Editor)
	virtual FName GetContainerName() const { return "Project"; };

	// 获取设置的类别，用于在设置树中分组（如 Editor、Engine、Game 等）
	// Gets the category name used to group settings in the settings tree (e.g., Editor, Engine, Game)
	virtual FName GetCategoryName() const { return "WorldMapIconPlugin"; };

	// 获取设置节的唯一名称（使用类的 FName）
	// Gets the unique section name for these settings (uses the class's FName)
	virtual FName GetSectionName() const { return "WorldMapIconSettings"; };

	// 获取全局可修改的设置实例（Blueprint 可调用）
	// Returns the mutable default settings instance (callable from Blueprint)
	UFUNCTION(BlueprintPure,Category = "World Map|WorldMapIconSettings")
	static UWorldMapIconSettings* GetWorldMapIconSettings()
	{
		return GetMutableDefault<UWorldMapIconSettings>();
	};

	// 从默认资源/路径加载地图图标配置（实现位于 .cpp）
	// Load default map icon settings from assets/resources (implementation in .cpp)
	void LoadDefaultMapIconSettings();

	// 从默认资源/路径加载地图配置（实现位于 .cpp）
	// Load default map settings from assets/resources (implementation in .cpp)
	void LoadDefaultMapSettings();

	// 每帧最多更新的地图图标数量（用于分帧更新，避免卡顿）
	// Maximum number of map icons to update per frame (used to spread work across frames to avoid hitches)
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "General", meta = (Tooltip = "每一帧更新地图图标数量"))
	int MapIconUpdateNumEveryFrame = 10;

	// 地图配置表（DataTable，行类型为 FMapSettingStruct）
	// Map settings data table (UDataTable with rows of FMapSettingStruct)
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "General", meta = (Tooltip = "地图配置表"))
	TSoftObjectPtr<UDataTable> MapSettingDataTable;

	// 地图图标配置表（DataTable，行类型为 FMapIconSettingStruct）
	// Map icon settings data table (UDataTable with rows of FMapIconSettingStruct)
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "General", meta = (Tooltip = "地图图标配置表"))
	TSoftObjectPtr<UDataTable> MapIconSettingDataTable;
};