// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Core/LayeredUI_Subsystem.h"

#include <Config/DS_LayeredUIGameSettings.h>

bool ULayeredUI_Subsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

TMap<FGameplayTag, int32> ULayeredUI_Subsystem::GetLayerMap()
{
	const UDS_LayeredUIGameSettings* UIData = GetDefault<UDS_LayeredUIGameSettings>();

	if(UIData)
	{
		return UIData->LayerMap;
	}

	return TMap<FGameplayTag, int32>();
}
