// Copyright 2026 CJ. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WorldMapIconType.h"
#include "WorldMapIconInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UWorldMapIconInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WORLDMAPICONPLUGIN_API IWorldMapIconInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable,Category = "WorldMapIcon")
	void OnMapIconDataChange(const FMapIconStruct& NewMapIconData, const TArray<FName>& ChangePramKey);
};
