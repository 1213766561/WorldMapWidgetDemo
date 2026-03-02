# WorldMapIconPlugin — Features and Integration Guide (English)

## Summary
WorldMapIconPlugin provides a minimap / world-map icon system for Unreal Engine projects. This document lists features and gives step‑by‑step instructions for installing, configuring, integrating, and extending the plugin in both C++ and Blueprint. It also includes minimal example snippets and troubleshooting steps required by distribution/review policies.

## Features (short)
- Runtime icon management subsystem: `UWroldMapIconSubsystem` (tickable) for registration, updates, removal, batching.
- Actor/provider interfaces:
  - `IWorldMapActorInterface` — `IsBeginPlayAddMapIcon()` and `GetMapIconData()`.
  - `IWorldMapIconInterface` — `OnMapIconDataChange(const FMapIconStruct&, const TArray<FName>&)`.
- Configurable developer settings: `UWorldMapIconSettings` with `MapIconUpdateNumEveryFrame`, `MapSettingDataTable`, `MapIconSettingDataTable`.
- Coordinate & helper utilities in `UWorldMapIconType` (World↔Map conversions, async loaders).
- Blueprint- and C++-friendly APIs and generated `Execute_*` helpers for safe cross-language calls.

## Requirements
- Unreal Engine (project built with the engine version used in this repository).
- No external third-party binaries required by the plugin. If you add third-party tools, include separate installation instructions.

## Installation (step‑by‑step)
1. Place the plugin folder in your project:
   - `Plugins/WorldMapIconPlugin` (already present in this repo).
2. Enable the plugin:
   - Editor: Edit → Plugins → locate "WorldMapIconPlugin" and enable it; restart the editor if prompted.
3. Regenerate project files and build:
   - In Windows: Right-click `.uproject` → Generate Visual Studio project files → open solution and build.
   - Or run UnrealBuildTool / your CI build to compile the plugin module.
4. Verify plugin is loaded:
   - In editor: Window → Developer Tools → Output Log, check logs for plugin load messages.

## Basic configuration
- Open Project Settings → search for "WorldMapIconPlugin" or Developer Settings (Display name: "小地图配置") to edit `WorldMapIconSettings`.
- Programmatic access:
  - In C++: `UWorldMapIconSettings* Settings = UWorldMapIconSettings::GetWorldMapIconSettings();`
  - In Blueprint: call `GetWorldMapIconSettings()` node.

## How to provide icons (Actor integration)
1. Implement `IWorldMapActorInterface` on any Actor (C++ or Blueprint).
2. C++ minimal example:
   ```cpp
   // In YourActor.h
   #include "WorldMapIconType.h" // for FMapIconStruct
   #include "WorldMapIconInterface.h" // for IWorldMapActorInterface
   
   UCLASS()
   class YOURPROJECT_API AYourActor : public AActor, public IWorldMapActorInterface
   {
       GENERATED_BODY()
   
   public:
       // Called when the game starts or when spawned
       virtual void BeginPlay() override;
   
       // Implement IWorldMapActorInterface
       virtual bool IsBeginPlayAddMapIcon() const override { return true; }
       virtual FMapIconStruct GetMapIconData() const override;
   };
   
   // In YourActor.cpp
   #include "YourActor.h"
   
   void AYourActor::BeginPlay()
   {
       Super::BeginPlay();
       // Additional initialization
   }
   
   FMapIconStruct AYourActor::GetMapIconData() const
   {
       FMapIconStruct Data;
       // Fill data fields, e.g. ID, type, position
       return Data;
   }
   ```
3. Blueprint implementation:
   - Inherit from `WorldMapActorInterface`.
   - Implement `IsBeginPlayAddMapIcon` and `GetMapIconData` in Blueprint editor.
   - Optionally override `BeginPlay` for initialization.

## Common performance considerations
- `MapIconUpdateNumEveryFrame`: tune this setting in `WorldMapIconSettings` to control the number of icons updated each frame. A lower value reduces CPU/GPU load but may impact icon visibility updates.
- Use profiler (e.g. Unreal Frontend, Session Frontend) to analyze and optimize any performance issues.
