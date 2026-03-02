// Copyright 2026 CJ. All Rights Reserved.


#include "WorldMapIconSubsystem.h"
#include "WorldMapIconPlugin.h"
#include "WorldMapIconSettings.h"
#include "Engine/AssetManager.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"

void UWroldMapIconSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	auto OnMapLoadEvent = [this](UWorld* LoadedWorld) 
	{
		if (!LoadedWorld) return;
		FString MapName = UWorld::RemovePIEPrefix(LoadedWorld->GetMapName());
		UWorldMapIconSettings* WorldMapIconSettings = UWorldMapIconSettings::GetWorldMapIconSettings();
		if (!WorldMapIconSettings)
		{
			UE_LOG(LogWorldMapIcon, Log, TEXT("UWroldMapIconSubsystem::Initialize: WorldMapIconSettings not valid!"));
			return;
		}
		auto MapSettingDataTable = WorldMapIconSettings->MapSettingDataTable;
		if(!MapSettingDataTable.IsValid())
		{
			UE_LOG(LogWorldMapIcon, Log, TEXT("UWroldMapIconSubsystem::Initialize: MapSettingDataTable not valid !"));
			return;
		}
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		StreamableManager.RequestAsyncLoad(
			MapSettingDataTable.ToSoftObjectPath(),
			FStreamableDelegate::CreateLambda([this, MapSettingDataTable, MapName]()
				{
					if (!MapSettingDataTable.Get())
					{
						UE_LOG(LogWorldMapIcon, Log, TEXT("UWroldMapIconSubsystem::Initialize: MapSettingDataTable not find !"));
						return;
					}
					FMapSettingStruct* FindData = MapSettingDataTable.Get()->FindRow<FMapSettingStruct>(*MapName, TEXT("Context"));
					if (!FindData)
					{
						UE_LOG(LogWorldMapIcon, Log, TEXT("UWroldMapIconSubsystem::Initialize: MapSettingData: %s not find !"), *MapName);
						return;
					}
					SetWorldMapSetting(*FindData);
				}));
	};
	PostLoadMapWithWorldHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddLambda(OnMapLoadEvent);
	if (GetWorld())
	{
		OnMapLoadEvent(GetWorld());
	}
}

void UWroldMapIconSubsystem::Deinitialize()
{
	if (PostLoadMapWithWorldHandle.IsValid())
	{
		FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(PostLoadMapWithWorldHandle);
	}
}

void UWroldMapIconSubsystem::Tick(float DeltaTime)
{
	if (!bIsActive) return;

	if (UpdatedMapIconDataMap.Num() <= 0) return;
	int UpdateNumThisFrame = 0;
	if (UWorldMapIconSettings* WorldMapIconSettings = UWorldMapIconSettings::GetWorldMapIconSettings())
	{
		UpdateNumThisFrame = WorldMapIconSettings->MapIconUpdateNumEveryFrame;
	}
	else
	{
		UpdateNumThisFrame = 10;
	}
	TArray<int> Keys;
	UpdatedMapIconDataMap.GetKeys(Keys);
	for (int i = 0; i < Keys.Num(); i++)
	{
		auto T = GetMapIconDataByID(Keys[i]);
		auto UpdateData = UpdatedMapIconDataMap.Find(Keys[i]);
		if (T.MapIcon_ID >= 0 && UpdateData)
		{
			TArray<FName> ChangedList;
			T.UpdateMapIconData(*UpdateData, ChangedList);
			if (ChangedList.Num() > 0)
			{
				OnMapIconUpdate.Broadcast(T, ChangedList);
			}
			UpdatedMapIconDataMap.Remove(Keys[i]);
		}
		if (--UpdateNumThisFrame <= 0) break;
	}
}

UWroldMapIconSubsystem* UWroldMapIconSubsystem::GetWroldMapIconSubsystem(UObject* TargetObject)
{
	if (TargetObject 
		&& TargetObject->GetWorld() 
		&& TargetObject->GetWorld()->GetSubsystem<UWroldMapIconSubsystem>() 
		&& TargetObject->GetWorld()->GetSubsystem<UWroldMapIconSubsystem>()->IsActive())
	{
		return TargetObject->GetWorld()->GetSubsystem<UWroldMapIconSubsystem>();
	}
	return nullptr;
}


void UWroldMapIconSubsystem::SetWorldMapSetting(const FMapSettingStruct& NewSetting)
{
	WorldMapSetting = NewSetting;
	bIsActive = true;
	if(OnSystemActive.IsBound())
	{
		OnSystemActive.Broadcast();
	}
}



FMapIconStruct UWroldMapIconSubsystem::GetMapIconDataByActor(AActor* TargetActor)
{
	if (TargetActor && RegisteredMapIconActorMap.Contains(TargetActor))
	{
		return GetMapIconDataByID(*RegisteredMapIconActorMap.Find(TargetActor));
	}
	return FMapIconStruct();
}

int UWroldMapIconSubsystem::GetMapIconIDByActor(AActor* TargetActor)
{
	if (TargetActor && RegisteredMapIconActorMap.Contains(TargetActor))
	{
		return *RegisteredMapIconActorMap.Find(TargetActor);
	}
	return -1;
}

FMapIconStruct UWroldMapIconSubsystem::GetMapIconDataByID(const int& TargetMapIconID)
{
	if (RegisteredMapIconDataMap.Contains(TargetMapIconID))
	{
		return *RegisteredMapIconDataMap.Find(TargetMapIconID);
	}
	return FMapIconStruct();
}

TArray<FMapIconStruct> UWroldMapIconSubsystem::GetAllMapIconDatas()
{
	TArray<FMapIconStruct> AllMapIconDatas;
	RegisteredMapIconDataMap.GenerateValueArray(AllMapIconDatas);
	return AllMapIconDatas;
}

int UWroldMapIconSubsystem::TryUpdateMapIconByActor(const FMapIconStruct& UpdateData, AActor* Target, const bool& bRequiresFraming)
{
	if (!Target) return -1;
	auto T = GetMapIconIDByActor(Target);
	if (T < 0)
	{
		RegisteredMapIconActorMap.Add(Target, MapIconID);
		Target->OnDestroyed.AddDynamic(this, &UWroldMapIconSubsystem::OnActorDestroyEvent);
	}
	return TryUpdateMapIconByID(UpdateData, T);
}

int UWroldMapIconSubsystem::TryUpdateMapIconByID(const FMapIconStruct& UpdateData, const int& TargetMapIconID, const bool& bRequiresFraming)
{
	auto T = GetMapIconDataByID(TargetMapIconID);
	TArray<FName> ChangedList;
	if (T.MapIcon_ID < 0)
	{
		FMapIconStruct MapIconData = FMapIconStruct(MapIconID++);
		MapIconData.UpdateMapIconData(UpdateData, ChangedList);
		OnMapIconUpdate.Broadcast(MapIconData, ChangedList);
		RegisteredMapIconDataMap.Add(MapIconData.MapIcon_ID, MapIconData);
		RegisteredMapIconTypeMap.Add(MapIconData.MapIcon_Type, MapIconData.MapIcon_ID);
		return MapIconData.MapIcon_ID;
	}
	else
	{
		if (UWorldMapIconSettings* WorldMapIconSettings = UWorldMapIconSettings::GetWorldMapIconSettings())
		{
			if (WorldMapIconSettings->MapIconUpdateNumEveryFrame > 0 && bRequiresFraming)
			{
				UpdatedMapIconDataMap.Add(T.MapIcon_ID, UpdateData);
				return T.MapIcon_ID;
			}
		}
		T.UpdateMapIconData(UpdateData, ChangedList);
		if (ChangedList.Num() > 0)
		{
			OnMapIconUpdate.Broadcast(T, ChangedList);
		}
		return T.MapIcon_ID;
	}
}

void UWroldMapIconSubsystem::TryRemoveMapIcon(int TargetMapIconID)
{
	if (TargetMapIconID < 0) return;
	if (auto T = RegisteredMapIconDataMap.Find(TargetMapIconID))
	{
		OnMapIconRemove.Broadcast(*T);
		RegisteredMapIconTypeMap.RemoveSingle(T->MapIcon_Type,T->MapIcon_ID);
		RegisteredMapIconDataMap.Remove(T->MapIcon_ID);
	}
}

void UWroldMapIconSubsystem::TryRemoveMapByActor(AActor* Target)
{
	if (!Target) return;
	auto T = GetMapIconIDByActor(Target);
	if (T < 0) return;
	RegisteredMapIconActorMap.Remove(Target);
	Target->OnDestroyed.RemoveDynamic(this, &UWroldMapIconSubsystem::OnActorDestroyEvent);
	TryRemoveMapIcon(T);
}

void UWroldMapIconSubsystem::OnActorDestroyEvent(AActor* Target)
{
	TryRemoveMapByActor(Target);
}

