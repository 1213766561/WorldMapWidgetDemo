概览
•	插件职责：在 World 作用域管理“地图图标”（注册、更新、移除、世界/地图坐标转换、批量/延迟更新与 UI 通知）。
•	运行时范围：继承自 UTickableWorldSubsystem，支持每帧处理（批量更新/平滑等）。
•	设计要点：使用弱指针避免 Actor 挂起导致悬空引用；使用软引用支持延迟/异步加载图集或 widget class；采用增量变更（ChangedList）以减少 UI 更新开销。
•	注意：类名 UWroldMapIconSubsystem 保留了代码中原有拼写（注释提及为兼容性保留）。
文件清单（主要公开定义）
•	WorldMapIconSubsystem.h — 子系统核心 API、事件、内部数据结构声明。
•	WorldMapIconType.h — 数据结构（FMapSettingStruct、FMapIconStruct、FMapIconSettingStruct）、工具函数（坐标转换、异步加载）。
•	WorldMapIconInterface.h — UI/Widget 交互接口 IWorldMapIconInterface（OnMapIconDataChange）。
主要类型（摘要）
•	FMapSettingStruct
•	地图配置：MapName、WorldMap_LeftTop（世界左上角）、WorldMap_WorldScale、WorldMap_MapScale、WorldMap_Texture（TSoftObjectPtr<UTexture2D>）。
•	FMapIconStruct
•	图标实例：MapIcon_ID（int）、MapIcon_Type（FName）、MapIcon_ExtraPram（TMap<FName,FString>，用于 Location/Rotation/Scale/OwnID 等）。
•	方法：UpdateMapIconData(const FMapIconStruct& Target, TArray<FName>& ChangedList) — 合并数据并把发生变化的键追加到 ChangedList（用于增量广播）。
•	FMapIconSettingStruct
•	类型配置：MapIconType、MapIconWidget（TSoftClassPtr<UObject> Conv_SoftClassPathToSoftClassRef 推荐实现 UWorldMapIconInterface）、MapIcon_BaseTexture、MapIcon_BaseSize。
•	EWorldMapType：枚举（None / WorldMap / MiniWorldMap）。
•	UWorldMapIconType（工具类）
•	静态函数：WorldToMapCoordinates、MapToWorldCoordinates、AsyncLoadClass（异步加载软类并回调）。
子系统（UWroldMapIconSubsystem）公开 API（概要）
•	生命周期
•	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
•	virtual void Deinitialize() override;
•	成员：FDelegateHandle PostLoadMapWithWorldHandle（用于地图加载后回调绑定）。
•	Tick
•	virtual void Tick(float DeltaTime) override; — 处理 UpdatedMapIconDataMap 中的批量/延迟更新逻辑。
•	virtual TStatId GetStatId() const override;
•	获取/设置
•	static UWroldMapIconSubsystem* GetWroldMapIconSubsystem(UObject* TargetObject); — 静态帮助方法（Blueprint 可用）。
•	void SetWorldMapSetting(const FMapSettingStruct& NewSetting);
•	属性：UPROPERTY(BlueprintReadWrite) FMapSettingStruct WorldMapSetting;
•	坐标转换（封装工具类）
•	FVector2D WorldToMapCoordinates(const FVector& WorldPos)
•	FVector MapToWorldCoordinates(const FVector2D& MapPos)
•	注册/更新/查询/移除
•	FMapIconStruct& GetMapIconDataByActor(AActor* TargetActor);
•	const int& GetMapIconIDByActor(AActor* TargetActor);
•	FMapIconStruct& GetMapIconDataByID(const int& TargetMapIconID);
•	TArray<FMapIconStruct> GetAllMapIconDatas(); — 返回副本供 UI 刷新使用。
•	const int& TryUpdateMapIconByActor(const FMapIconStruct& UpdateData, AActor* Target = nullptr, const bool& bRequiresFraming = true);
•	const int& TryUpdateMapIconByID(const FMapIconStruct& UpdateData, const int& TargetMapIconID = -1, const bool& bRequiresFraming = true);
•	void TryRemoveMapIcon(int TargetMapIconID = -1);
•	void TryRemoveMapByActor(AActor* Target = nullptr);
•	Actor 生命周期绑定
•	UFUNCTION() void OnActorDestroyEvent(AActor* Target); — 用于在 Actor 销毁时清理其注册数据。
•	事件 / 委托（Blueprint 可绑定）
•	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMapIconUpdate, const FMapIconStruct&, MapIconData, const TArray<FName>&, UpdateList);
•	广播图标更新：提供最新 FMapIconStruct 与变更字段名列表（用于增量刷新）。
•	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMapIconRemove, const FMapIconStruct&, MapIconData);
•	广播图标移除：提供被移除的 FMapIconStruct（副本）。
•	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSystemActive); — 子系统变为激活时广播。
•	对应 UPROPERTY(BlueprintAssignable)：
•	OnMapIconUpdate, OnMapIconRemove, OnSystemActive。
内部数据结构（重要字段）
•	TMap<TWeakObjectPtr<AActor>, int> RegisteredMapIconActorMap; — Actor -> MapIcon ID（弱引用避免悬挂）。
•	TMap<int, FMapIconStruct> RegisteredMapIconDataMap; — ID -> 数据。
•	TMap<int, FMapIconStruct> UpdatedMapIconDataMap; — 已更新但待广播/处理的数据（用于批量/延迟）。
•	TMultiMap<FName, int> RegisteredMapIconTypeMap; — MapIcon 类型名 -> 多个 ID（按类型查询）。
•	int MapIconID = 0; — 自增生成 ID（新建时分配）。
•	bool bIsActive = false; — 子系统活跃标志（IsActive() 可从蓝图调用）。
事件与行为要点
•	更新路径：外部调用 TryUpdateMapIconByActor/ID -> 内部合并/写入 UpdatedMapIconDataMap -> 在 Tick 中批量处理 -> 触发 OnMapIconUpdate 并附带 ChangedList。
•	删除：调用 TryRemoveMapIcon/ByActor -> 从注册表移除 -> 触发 OnMapIconRemove。
•	Actor 销毁：子系统绑定 OnActorDestroyEvent 清理相关注册项，避免悬空。
•	异步加载资源：TSoftObjectPtr<UObject> Conv_SoftObjPathToSoftObjRef/TSoftClassPtr<UObject> Conv_SoftClassPathToSoftClassRef + AsyncLoadClass，适用于运行时或编辑时延迟加载 Widget/贴图。