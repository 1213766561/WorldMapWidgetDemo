// Copyright 2026 CJ. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WorldMapIconType.h"
#include "WorldMapIconInterface.h"
#include "WorldMapIconSubsystem.generated.h"

/**
 * WorldMapIcon 子系统广播委托（双语注释）
 *
 * FOnMapIconUpdate:
 *  - 中文：当某个地图图标数据被更新时广播，包含更新后的图标数据和发生变化的字段列表。
 *  - English: Broadcast when a map icon is updated. Provides the updated FMapIconStruct and a list
 *    of changed field names.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMapIconUpdate, const FMapIconStruct&, MapIconData, const TArray<FName>&, UpdateList);

/**
 * 地图图标移除委托（双语）
 *
 * FOnMapIconRemove:
 *  - 中文：当某个地图图标被移除时广播，携带被移除的图标数据（副本）。
 *  - English: Broadcast when a map icon is removed. Provides the removed FMapIconStruct.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMapIconRemove, const FMapIconStruct&, MapIconData);

/**
 * 系统激活委托（双语）
 *
 * FOnSystemActive:
 *  - 中文：当子系统进入激活状态时广播（无参数）。
 *  - English: Broadcast when the subsystem becomes active (no parameters).
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSystemActive);

/**
 * UWroldMapIconSubsystem
 *
 * 中文：
 *  - 世界级（World）子系统，负责在运行时管理游戏世界中的地图图标（注册、更新、移除、坐标转换等）。
 *  - 继承自 UTickableWorldSubsystem，支持每帧 Tick 以处理延迟更新或批处理逻辑。
 *
 * English:
 *  - World-scoped subsystem that manages map icons at runtime (registration, updates, removal,
 *    coordinate conversions, etc.).
 *  - Inherits from UTickableWorldSubsystem to support per-frame processing.
 *
 * 备注：
 *  - 类名保留原始拼写（UWroldMapIconSubsystem）以兼容现有代码与序列化。
 */
UCLASS()
class WORLDMAPICONPLUGIN_API UWroldMapIconSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
public:
	/** 初始化子系统（双语）
	 *  - 中文：引导子系统（绑定事件、恢复状态等）。在世界创建时调用。
	 *  - English: Initialize the subsystem (bind events, restore state, etc.). Called when the world is created.
	 */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** 反初始化子系统（双语）
	 *  - 中文：清理绑定、释放资源。世界销毁时调用。
	 *  - English: Deinitialize the subsystem (cleanup bindings, release resources). Called when the world is destroyed.
	 */
	virtual void Deinitialize() override;

	/** Post-load map 回调句柄（用于解除绑定） */
	FDelegateHandle PostLoadMapWithWorldHandle;

	/** 获取性能统计 id（双语） */
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UWroldMapIconSubsystem, STATGROUP_Tickables); };

	/** 每帧 Tick（双语）
	 *  - 中文：用于处理需要逐帧更新的图标逻辑（例如延迟批量更新/平滑动画等）。
	 *  - English: Used to process per-frame icon logic (e.g. deferred batch updates / smoothing).
	 */
	virtual void Tick(float DeltaTime) override;

	/** 获取子系统实例（Blueprint 可用）（双语）
	 *  - 中文：静态帮助方法，从任意 UObject 获取当前世界的 UWroldMapIconSubsystem。
	 *  - English: Static helper to get the UWroldMapIconSubsystem from any UObject.
	 */
	UFUNCTION(BlueprintPure, Category = "World Map|Subsystem")
	static UWroldMapIconSubsystem* GetWroldMapIconSubsystem(UObject* TargetObject);

	/** 当前世界地图设置（可在蓝图/编辑器读写）（双语）
	 *  - 中文：保存当前生效的地图配置（例如左上角偏移、世界/地图缩放、贴图引用）。
	 *  - English: Holds the current map configuration (top-left offset, world/map scale, texture pointer).
	 */
	UPROPERTY(BlueprintReadWrite, Category = "World Map|Subsystem")
	FMapSettingStruct WorldMapSetting;

	/** 设置新的世界地图配置（双语）
	 *  - 中文：替换当前地图设置并可能触发重新计算/重绘。
	 *  - English: Replace the current map setting and potentially trigger re-calculation/redraw.
	 */
	UFUNCTION(BlueprintCallable, Category = "World Map|Subsystem")
	void SetWorldMapSetting(const FMapSettingStruct& NewSetting);

	/** 子系统是否激活（蓝图可调用）（双语） */
	UFUNCTION(BlueprintCallable, Category = "World Map|Subsystem")
	bool IsActive() { return bIsActive; };

	/** 系统激活事件（蓝图绑定）（双语） */
	UPROPERTY(BlueprintAssignable)
	FOnSystemActive OnSystemActive;

	/** 世界坐标转地图坐标（包装器，使用当前 WorldMapSetting）（双语）
	 *  - 中文：使用当前 WorldMapSetting 的参数将三维世界位置转换为二维地图坐标。
	 *  - English: Convert a 3D world position to 2D map coordinates using current WorldMapSetting.
	 */
	UFUNCTION(BlueprintCallable, Category = "World Map|Subsystem")
	FVector2D WorldToMapCoordinates(const FVector& WorldPos)
	{
		return UWorldMapIconType::WorldToMapCoordinates(WorldPos,
			FVector(WorldMapSetting.WorldMap_LeftTop, 0.f),
			WorldMapSetting.WorldMap_WorldScale, WorldMapSetting.WorldMap_MapScale);
	}

	/** 地图坐标转世界坐标（包装器）（双语）
	 *  - 中文：将二维地图坐标转换回三维世界坐标（Z 使用 WorldMap_LeftTop.Z 或 0）。
	 *  - English: Convert 2D map coordinates back to a 3D world position (Z uses WorldMap_LeftTop.Z or 0).
	 */
	UFUNCTION(BlueprintCallable, Category = "World Map|Subsystem")
	FVector MapToWorldCoordinates(const FVector2D& MapPos)
	{
		return UWorldMapIconType::MapToWorldCoordinates(MapPos,
			FVector(WorldMapSetting.WorldMap_LeftTop, 0.f),
			WorldMapSetting.WorldMap_WorldScale, WorldMapSetting.WorldMap_MapScale);
	}

private:
	/** 注册的 Actor 到 MapIcon ID 映射（弱引用，避免循环引用/GC 问题） */
	TMap<TWeakObjectPtr<AActor>, int> RegisteredMapIconActorMap;

	/** 当前注册的 MapIcon 数据（ID -> 数据） */
	TMap<int, FMapIconStruct> RegisteredMapIconDataMap;

	/** 已更新但待分发的 MapIcon 数据（用于批量通知） */
	TMap<int, FMapIconStruct> UpdatedMapIconDataMap;

	/** MapIcon 类型到 MapIcon ID 的多重映射（用于按类型查询） */
	TMultiMap<FName, int> RegisteredMapIconTypeMap;

	/** 下一个分配的 MapIcon ID（自增） */
	int MapIconID = 0;

	/** 子系统激活标志（双语）
	 *  - 中文：指示子系统当前是否处于激活/就绪状态，可用于控制事件广播与注册逻辑。
	 *  - English: Indicates whether the subsystem is currently active/ready. Used to gate events and registration.
	 */
	bool bIsActive = false;

public:
	/** 根据 Actor 获取其对应的 MapIcon 数据引用（双语）
	 *  - 中文：若 Actor 未注册，可能创建或抛出断言（实现细节在 CPP）。
	 *  - English: Returns a reference to the actor's map icon data. If not registered behavior is defined in CPP.
	 */
	UFUNCTION(BlueprintPure, Category = "World Map|Subsystem")
	FMapIconStruct GetMapIconDataByActor(AActor* TargetActor);

	/** 根据 Actor 获取 MapIcon ID（双语）
	 *  - 中文：返回 Actor 对应的 MapIcon ID（如果存在）。
	 *  - English: Returns the MapIcon ID associated with the actor (if any).
	 */
	UFUNCTION(BlueprintPure, Category = "World Map|Subsystem")
	int GetMapIconIDByActor(AActor* TargetActor);

	/** 根据 ID 获取 MapIcon 数据引用（双语）
	 *  - 中文：通过 MapIcon ID 直接访问对应数据。
	 *  - English: Access map icon data directly by ID.
	 */
	UFUNCTION(BlueprintPure, Category = "World Map|Subsystem")
	FMapIconStruct GetMapIconDataByID(const int& TargetMapIconID);

	/** 获取所有 MapIcon 数据的数组（副本）（双语）
	 *  - 中文：返回当前所有注册地图图标数据的数组副本，供 UI 刷新等使用。
	 *  - English: Return an array copy of all currently registered map icon data (for UI refresh, etc.).
	 */
	UFUNCTION(BlueprintCallable, Category = "World Map|Subsystem")
	TArray<FMapIconStruct> GetAllMapIconDatas();

	/** 通过 Actor 尝试更新 MapIcon（双语）
	 *  - 中文：使用 UpdateData 更新目标 Actor 对应的图标数据，若 Target 为 nullptr 则使用 UpdateData 内的 ID 或创建新条目。
	 *    bRequiresFraming 指示是否需要执行聚焦/框选逻辑。
	 *  - English: Try to update the map icon associated with the given actor. If Target is nullptr the function
	 *    uses the ID inside UpdateData or creates a new entry. bRequiresFraming indicates whether framing/focus is needed.
	 */
	UFUNCTION(BlueprintCallable, Category = "World Map|Subsystem", meta = (AutoCreateRefTerm = "UpdateData,bRequiresFraming"))
	int TryUpdateMapIconByActor(const FMapIconStruct& UpdateData, AActor* Target = nullptr, const bool& bRequiresFraming = true);

	/** 通过 ID 尝试更新 MapIcon（双语）
	 *  - 中文：直接按 ID 更新或创建 MapIcon，TargetMapIconID 默认为 -1 表示创建新条目。
	 *  - English: Try to update or create a map icon by ID. TargetMapIconID == -1 indicates creation of a new entry.
	 */
	UFUNCTION(BlueprintCallable, Category = "World Map|Subsystem", meta = (AutoCreateRefTerm = "UpdateData,TargetMapIconID,bRequiresFraming"))
	int TryUpdateMapIconByID(const FMapIconStruct& UpdateData, const int& TargetMapIconID = -1, const bool& bRequiresFraming = true);

	/** 尝试移除指定 ID 的 MapIcon（双语）
	 *  - 中文：如果 TargetMapIconID == -1 可忽略/无操作（实现细节在 CPP）。
	 *  - English: Try to remove the map icon with the given ID. If TargetMapIconID == -1 behavior is defined in CPP.
	 */
	UFUNCTION(BlueprintCallable, Category = "World Map|Subsystem")
	void TryRemoveMapIcon(int TargetMapIconID = -1);

	/** 按 Actor 尝试移除其对应的 MapIcon（双语） */
	UFUNCTION(BlueprintCallable, Category = "World Map|Subsystem")
	void TryRemoveMapByActor(AActor* Target = nullptr);

	/** 更新广播委托（蓝图绑定）（双语） */
	UPROPERTY(BlueprintAssignable)
	FOnMapIconUpdate OnMapIconUpdate;

	/** 移除广播委托（蓝图绑定）（双语） */
	UPROPERTY(BlueprintAssignable)
	FOnMapIconRemove OnMapIconRemove;

	/** Actor 销毁事件回调（绑定到 Actor 的 OnDestroyed）（双语）
	 *  - 中文：在 Actor 被销毁时清理其注册的 MapIcon，避免悬空引用。
	 *  - English: Clean up registered map icons when an actor is destroyed to avoid dangling references.
	 */
	UFUNCTION()
	void OnActorDestroyEvent(AActor* Target);
};