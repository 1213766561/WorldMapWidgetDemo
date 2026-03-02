// Copyright 2026 CJ. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WorldMapIconType.h"
#include "WorldMapActorInterface.generated.h"

/*
 * WorldMapActorInterface.h
 *
 * 中/英文说明：
 * 本接口标识那些希望在世界地图（World Map / MiniMap）上显示图标的 Actor。
 * Implement this interface on an Actor that can provide map-icon data so it can be automatically
 * or manually added to the world map UI.
 *
 * 重要点：
 * - 方法使用 BlueprintNativeEvent：可以在 Blueprint 中实现，也可以在 C++ 中提供 _Implementation。
 * - 调用接口时：
 *   - 若直接在 C++ 中调用已实现的函数，可直接调用接口方法（如果类以 C++ 实现）。
 *   - 若调用可能由 Blueprint 实现的接口函数（跨对象），请使用 IWorldMapActorInterface::Execute_FunctionName(TargetActor)。
 *
 * This header depends on WorldMapIconType.h which defines FMapIconStruct (MapIcon data layout).
 */

 // This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWorldMapActorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * IWorldMapActorInterface
 *
 * 中文：
 * - 实现此接口的 Actor 应提供用于地图显示的图标数据（FMapIconStruct），并可选择在 BeginPlay 时自动添加图标。
 * - C++ 实现者：重写 <MethodName>_Implementation() 函数，例如 bool IsBeginPlayAddMapIcon_Implementation()。
 * - Blueprint 实现者：在 Blueprint 中实现对应的事件。
 *
 * English:
 * - Actors implementing this interface provide map icon information (FMapIconStruct) and
 *   indicate whether they should be added to the map automatically on BeginPlay.
 * - C++ implementers override <MethodName>_Implementation().
 * - Blueprint implementers implement the Blueprint event.
 *
 * 调用示例 / Usage:
 * - 在 C++ 中直接调用（如果类在 C++ 中实现接口）:
 *     if (IMyInterface* I = Cast<IWorldMapActorInterface>(MyActor)) { bool b = I->IsBeginPlayAddMapIcon(); }
 * - 跨对象（支持 Blueprint 实现）安全调用（推荐）:
 *     bool b = IWorldMapActorInterface::Execute_IsBeginPlayAddMapIcon(MyActor);
 *
 * 注意 / Note:
 * - FMapIconStruct 定义在 WorldMapIconType.h，包含 MapIcon_ID、MapIcon_Type、MapIcon_ExtraPram 等字段。
 * - BlueprintNativeEvent 方法会在编译生成函数声明与默认 _Implementation，若在 C++ 实现默认行为请实现 <Name>_Implementation。
 */
class WORLDMAPICONPLUGIN_API IWorldMapActorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	 * 是否在 BeginPlay 时自动将此 Actor 的图标添加到世界地图
	 *
	 * 中文：
	 * 返回 true 表示在 Actor::BeginPlay 时自动创建/注册地图图标（由地图系统决定如何处理）。
	 * 在 C++ 中实现时请实现 IsBeginPlayAddMapIcon_Implementation()。
	 *
	 * English:
	 * Return true to request automatic addition/registration of this Actor's map icon during BeginPlay.
	 * C++ implementations should override IsBeginPlayAddMapIcon_Implementation().
	 *
	 * 调用（跨 Blueprint 安全）:
	 *   bool b = IWorldMapActorInterface::Execute_IsBeginPlayAddMapIcon(TargetActor);
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "World Map|Actor")
	bool IsBeginPlayAddMapIcon();

	virtual bool IsBeginPlayAddMapIcon_Implementation()
	{
		return false;
	}

	/**
	 * 获取用于在世界地图上显示的图标数据
	 *
	 * 中文：
	 * 返回包含图标 ID、类型和扩展参数的 FMapIconStruct。扩展参数（MapIcon_ExtraPram）使用键值对存储
	 * 例如位置/方向/缩放/自定义标识等，具体解析由地图系统或 Widget 决定。
	 * 在 C++ 中实现时请实现 GetMapIconData_Implementation()。
	 *
	 * English:
	 * Returns the FMapIconStruct that describes this Actor's map icon (ID, type and extra parameters).
	 * Extra parameters are stored as a TMap<FName, FString> and can include location, rotation, scale, owner ID, etc.
	 * C++ implementations should override GetMapIconData_Implementation().
	 *
	 * 调用（跨 Blueprint 安全）:
	 *   FMapIconStruct Data = IWorldMapActorInterface::Execute_GetMapIconData(TargetActor);
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "World Map|Actor")
	FMapIconStruct GetMapIconData();

	virtual FMapIconStruct GetMapIconData_Implementation()
	{
		return FMapIconStruct();
	}
};