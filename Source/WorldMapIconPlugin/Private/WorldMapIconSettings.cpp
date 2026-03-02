// Copyright 2026 CJ. All Rights Reserved.


#include "WorldMapIconSettings.h"
#include <Engine/DataTable.h>

UWorldMapIconSettings::UWorldMapIconSettings()
{
	LoadDefaultMapIconSettings();
	LoadDefaultMapSettings();
}

void UWorldMapIconSettings::LoadDefaultMapIconSettings()
{
	if (!MapIconSettingDataTable)
	{
		FString AssetPath = TEXT("DataTable'/WorldMapIconPlugin/Config/DT_MapIconSetting.DT_MapIconSetting'");
		UDataTable* TargetDataTable = LoadObject<UDataTable>(nullptr, *AssetPath, nullptr, LOAD_None, nullptr);
		if (!TargetDataTable)
		{
			UE_LOG(LogWorldMapIcon,
				Warning,
				TEXT("LoadDefaultMapIconSettings: Failed to load default map icon setting table --> %s !"),
				*AssetPath);
			return;
		}
		MapIconSettingDataTable = TargetDataTable;
	}
}

void UWorldMapIconSettings::LoadDefaultMapSettings()
{
	if (!MapSettingDataTable)
	{
		FString AssetPath = TEXT("DataTable'/WorldMapIconPlugin/Config/DT_MapSetting.DT_MapSetting'");
		UDataTable* TargetDataTable = LoadObject<UDataTable>(nullptr, *AssetPath, nullptr, LOAD_None, nullptr);
		if (!TargetDataTable)
		{
			UE_LOG(LogWorldMapIcon,
				Warning,
				TEXT("LoadDefaultMapSettings: Failed to load default map icon setting table --> %s !"),
				*AssetPath);
			return;
		}
		MapSettingDataTable = TargetDataTable;
	}
}
