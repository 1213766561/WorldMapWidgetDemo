// Copyright 2026 CJ. All Rights Reserved.


#include "WorldMapIconType.h"
#include "Engine/AssetManager.h"

FVector2D UWorldMapIconType::WorldToMapCoordinates(const FVector& WorldPos,
    const FVector& WorldMap_WorldRoot,
    const FVector2D& WorldMap_WorldScale,
    const FVector2D& WorldMap_MapScale)
{
    float MapY = abs(WorldPos.X - WorldMap_WorldRoot.X) / WorldMap_WorldScale.X * WorldMap_MapScale.Y;
    float MapX = abs(WorldPos.Y - WorldMap_WorldRoot.Y) / WorldMap_WorldScale.Y * WorldMap_MapScale.X;
    return FVector2D(MapX, MapY);
}

FVector UWorldMapIconType::MapToWorldCoordinates(const FVector2D& MapPos,
    const FVector& WorldMap_WorldRoot,
    const FVector2D& WorldMap_WorldScale,
    const FVector2D& WorldMap_MapScale)
{
    float MapY = MapPos.X / WorldMap_MapScale.X * WorldMap_WorldScale.Y + WorldMap_WorldRoot.Y;
    float MapX = MapPos.Y / WorldMap_MapScale.Y * WorldMap_WorldScale.X + WorldMap_WorldRoot.X;
    return FVector(MapX, MapY,0.f);

}

void UWorldMapIconType::AsyncLoadClass(const TSoftClassPtr<UObject>& SoftClassPtr, FOnSoftClassLoad LoadedDelegate)
{
    FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
    Streamable.RequestAsyncLoad(SoftClassPtr.ToSoftObjectPath(), FStreamableDelegate::CreateLambda([LoadedDelegate, SoftClassPtr]()
        {
            if (LoadedDelegate.IsBound())
            {
                LoadedDelegate.Execute(SoftClassPtr.Get());
            }
        }));
}
