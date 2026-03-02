Overview
•	Purpose: A world-scoped subsystem to manage runtime map icons: registration, updates, removal, world↔map coordinate conversion, deferred/batched updates, and UI notifications.
•	Scope: Implemented as a UTickableWorldSubsystem to support per-frame processing (smoothing, batched broadcasts).
•	Note: The subsystem class name is UWroldMapIconSubsystem (preserves original spelling for compatibility).
Files (public API)
•	WorldMapIconSubsystem.h — subsystem API, events, and internal data structures.
•	WorldMapIconType.h — data structs, utility functions for coordinate conversion and async class loading.
•	WorldMapIconInterface.h — UI/Widget interface IWorldMapIconInterface (method OnMapIconDataChange).
Key Data Types
•	FMapSettingStruct
•	Fields: MapName, WorldMap_LeftTop (world origin for map), WorldMap_WorldScale (world size), WorldMap_MapScale (map/texture size), WorldMap_Texture (TSoftObjectPtr<UTexture2D>).
•	Usage: Global map configuration used for coordinate conversions and rendering.
•	FMapIconStruct
•	Fields: MapIcon_ID (int), MapIcon_Type (FName), MapIcon_ExtraPram (TMap<FName,FString>).
•	Method: UpdateMapIconData(const FMapIconStruct& Target, TArray<FName>& ChangedList) — merges Target into self and appends changed keys to ChangedList (for incremental updates).
•	FMapIconSettingStruct
•	Fields: MapIconType, MapIconWidget (TSoftClassPtr<UObject>), MapIcon_BaseTexture, MapIcon_BaseSize.
•	EWorldMapType — enum: None, WorldMap, MiniWorldMap.
•	UWorldMapIconType utilities:
•	static FVector2D WorldToMapCoordinates(const FVector& WorldPos, const FVector& WorldMap_WorldRoot, const FVector2D& WorldMap_WorldScale, const FVector2D& WorldMap_MapScale);
•	static FVector MapToWorldCoordinates(const FVector2D& MapPos, const FVector& WorldMap_WorldRoot, const FVector2D& WorldMap_WorldScale, const FVector2D& WorldMap_MapScale);
•	static void AsyncLoadClass(const TSoftClassPtr<UObject>& SoftClassPtr, FOnSoftClassLoad LoadedDelegate);
Subsystem: UWroldMapIconSubsystem — public API summary
•	Lifecycle
•	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
•	virtual void Deinitialize() override;
•	FDelegateHandle PostLoadMapWithWorldHandle;
•	Broadcast: FOnSystemActive OnSystemActive; (multicast)
•	Tick
•	virtual void Tick(float DeltaTime) override; — process UpdatedMapIconDataMap and dispatch batched updates.
•	virtual TStatId GetStatId() const override;
•	Settings and Helpers
•	UPROPERTY(BlueprintReadWrite) FMapSettingStruct WorldMapSetting;
•	UFUNCTION(BlueprintCallable) void SetWorldMapSetting(const FMapSettingStruct& NewSetting);
•	UFUNCTION(BlueprintPure) static UWroldMapIconSubsystem* GetWroldMapIconSubsystem(UObject* TargetObject);
•	UFUNCTION(BlueprintCallable) FVector2D WorldToMapCoordinates(const FVector& WorldPos);
•	UFUNCTION(BlueprintCallable) FVector MapToWorldCoordinates(const FVector2D& MapPos);
•	UFUNCTION(BlueprintCallable) bool IsActive();
•	Icon access / mutation
•	UFUNCTION(BlueprintPure) FMapIconStruct& GetMapIconDataByActor(AActor* TargetActor);
•	UFUNCTION(BlueprintPure) const int& GetMapIconIDByActor(AActor* TargetActor);
•	UFUNCTION(BlueprintPure) FMapIconStruct& GetMapIconDataByID(const int& TargetMapIconID);
•	UFUNCTION(BlueprintCallable) TArray<FMapIconStruct> GetAllMapIconDatas(); — returns copies for UI refresh.
•	UFUNCTION(BlueprintCallable) const int& TryUpdateMapIconByActor(const FMapIconStruct& UpdateData, AActor* Target = nullptr, const bool& bRequiresFraming = true);
•	UFUNCTION(BlueprintCallable) const int& TryUpdateMapIconByID(const FMapIconStruct& UpdateData, const int& TargetMapIconID = -1, const bool& bRequiresFraming = true);
•	UFUNCTION(BlueprintCallable) void TryRemoveMapIcon(int TargetMapIconID = -1);
•	UFUNCTION(BlueprintCallable) void TryRemoveMapByActor(AActor* Target = nullptr);
•	UFUNCTION() void OnActorDestroyEvent(AActor* Target); — bound to actor destruction to clean registration.
•	Events (Blueprint-assignable)
•	FOnMapIconUpdate — OnMapIconUpdate: (const FMapIconStruct& MapIconData, const TArray<FName>& UpdateList) — incremental update broadcast.
•	FOnMapIconRemove — OnMapIconRemove: (const FMapIconStruct& MapIconData) — removal broadcast.
•	FOnSystemActive — OnSystemActive: no params.
Internal Storage (important)
•	TMap<TWeakObjectPtr<AActor>, int> RegisteredMapIconActorMap; — actor → id (weak ptr).
•	TMap<int, FMapIconStruct> RegisteredMapIconDataMap; — id → data.
•	TMap<int, FMapIconStruct> UpdatedMapIconDataMap; — pending updates for batched dispatch.
•	TMultiMap<FName, int> RegisteredMapIconTypeMap; — type name → ids.
•	int MapIconID — auto-increment id generator.
•	bool bIsActive — subsystem ready flag.