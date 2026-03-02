// Copyright 2026 CJ. All Rights Reserved.


#include "WorldMapIconPlugin.h"

#define LOCTEXT_NAMESPACE "FWorldMapIconPluginModule"

DEFINE_LOG_CATEGORY(LogWorldMapIcon);

void FWorldMapIconPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FWorldMapIconPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FWorldMapIconPluginModule, WorldMapIconPlugin)