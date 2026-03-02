# WorldMapIconPlugin — 功能与集成指南（中文）

## 概要
`WorldMapIconPlugin` 提供小地图 / 世界地图图标系统，支持运行时图标注册、更新、移除，以及 C++ 与 Blueprint 的友好扩展点。本文件包含功能摘要、安装与配置的逐步指引、示例代码和常见故障排查，满足分发/审核对可用文档的要求。

## 主要功能
- 运行时图标管理子系统：`UWroldMapIconSubsystem`（继承 `UTickableWorldSubsystem`），支持按帧分批更新。
- 提供者与监听接口：
  - `IWorldMapActorInterface`（Actor 提供图标）：`IsBeginPlayAddMapIcon()`、`GetMapIconData()`。
  - `IWorldMapIconInterface`（监听/响应）：`OnMapIconDataChange(const FMapIconStruct&, const TArray<FName>&)`。
- 可配置开发者设置：`UWorldMapIconSettings`（`MapIconUpdateNumEveryFrame`、`MapSettingDataTable`、`MapIconSettingDataTable`）。
- 坐标与工具函数：`UWorldMapIconType`（世界↔地图坐标转换、异步加载辅助）。
- Blueprint/C++ 兼容：提供 `Execute_*` 调用、BlueprintCallable/Implementable 接口。

## 先决条件
- 对应工程使用的 Unreal Engine 版本（与仓库一致）。
- 插件不依赖第三方闭源二进制；若增加第三方依赖，需在仓库中提供下载与安装说明。

## 安装（逐步）
1. 将插件放入项目：`Plugins/WorldMapIconPlugin`（本仓库已包含）。
2. 在编辑器启用插件：编辑 → 插件 → 找到 `WorldMapIconPlugin`，启用并重启编辑器（如需）。
3. 生成并编译工程：
   - 在 Windows 上右键 `.uproject` → __Generate Visual Studio project files__，打开解决方案并编译；
   - 或使用 UnrealBuildTool / CI 流程构建模块。
4. 验证加载：在编辑器 Output Log 检查插件加载日志。

## 基本配置
- 编辑器：Project Settings 或 Developer Settings（显示名：“小地图配置”）调整 `WorldMapIconSettings`。
- 代码获取设置：
  - C++: `UWorldMapIconSettings* Settings = UWorldMapIconSettings::GetWorldMapIconSettings();`
  - Blueprint: 使用 `GetWorldMapIconSettings()` 节点。

## Actor 集成（如何提供图标）
- 在 Actor 上实现 `IWorldMapActorInterface`（C++ 或 Blueprint）。
- C++ 最小示例：
````````cpp
#include "WorldMapIconInterface.h"

// 我的Actor.h
class AMyActor : public AActor, public IWorldMapActorInterface
{
    // ... 其他代码 ...
    virtual bool IsBeginPlayAddMapIcon() const override { return true; }
    virtual FMapIconStruct GetMapIconData() const override;
};
````````
- Blueprint：在 Class Settings → Implemented Interfaces 添加 `WorldMapActorInterface`，实现 `IsBeginPlayAddMapIcon` 和 `GetMapIconData`。

## 子系统使用（运行时）
- 获取子系统：
  - C++: `UWroldMapIconSubsystem* Sub = UWroldMapIconSubsystem::GetWroldMapIconSubsystem(this);`
  - Blueprint: 使用 `GetWroldMapIconSubsystem` 节点。
- 注册/更新/移除：调用子系统的相应 API（参见 `UWroldMapIconSubsystem` 头文件获取精确方法名）。若 Actor 返回 `IsBeginPlayAddMapIcon()` 为 true，子系统通常会在 `BeginPlay` 自动注册。

## 响应图标数据变更（UI）
- 在 Widget 或监听对象上实现 `IWorldMapIconInterface`，在 `OnMapIconDataChange` 中更新 UI。
- 安全调用（可能为 Blueprint 实现时）：`IWorldMapIconInterface::Execute_OnMapIconDataChange(TargetObject, NewData, ChangedKeys)`。

## 数据表与设置
- 创建 DataTable（行类型对应 `FMapSettingStruct`、`FMapIconSettingStruct`），并在 `WorldMapIconSettings` 中引用。
- 代码设置示例：
````````cpp
UWorldMapIconSettings* Settings = UWorldMapIconSettings::GetWorldMapIconSettings();
if (Settings && Settings->MapSettingDataTable)
{
    static const FString ContextString(TEXT("MapSettingContext"));
    TArray<FMapSettingStruct*> AllRows;
    Settings->MapSettingDataTable->GetAllRows(ContextString, AllRows);

    for (FMapSettingStruct* Row : AllRows)
    {
        // 使用每一行的数据...
    }
}
````````

## 快速 Blueprint 入门
1. Actor Blueprint 实现 `WorldMapActorInterface`。
2. Widget Blueprint 实现 `WorldMapIconInterface`。
3. 在 Blueprint 中调用 `GetWroldMapIconSubsystem` 进行注册或查询。
4. 调整 `MapIconUpdateNumEveryFrame` 以平衡性能。

## 常见问题与排查
- 插件未显示：检查 `Plugins/WorldMapIconPlugin` 是否包含 `.uplugin`，重新生成项目文件并构建。
- Blueprint 接口未被触发：使用生成的 `Execute_*` 辅助函数或在 Blueprint 中正确实现事件。
- 性能问题：降低 `MapIconUpdateNumEveryFrame`，或增加更新间隔并使用剖析工具（Session Frontend 等）定位瓶颈。

## 合规说明
- 文档涵盖安装、集成、使用与扩展示例。如未来引入第三方工具，须在仓库中提供下载/安装说明或链接以满足分发要求。

## 参考文件
- `Source/WorldMapIconPlugin/Public/WorldMapIconInterface.h`
- `Source/WorldMapIconPlugin/Public/WorldMapActorInterface.h`
- `Source/WorldMapIconPlugin/Public/WorldMapIconSettings.h`
- `Source/WorldMapIconPlugin/Public/WorldMapIconType.h`
- `Source/WorldMapIconPlugin/Private/WorldMapIconInterface.cpp`

