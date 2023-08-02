// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Core/LayeredUI_Subsystem.h"

#include <Config/DS_LayeredUIGameSettings.h>

#include "Blueprint/UserWidget.h"

bool ULayeredUI_Subsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

TMap<FGameplayTag, int32> ULayeredUI_Subsystem::GetLayerMap()
{
	if(const UDS_LayeredUIGameSettings* UIData = GetDefault<UDS_LayeredUIGameSettings>())
	{
		return UIData->LayerMap;
	}

	return TMap<FGameplayTag, int32>();
}

TArray<UUserWidget*> ULayeredUI_Subsystem::GetCurrentWidgets()
{
	TArray<UUserWidget*> CurrentWidgets;
	
	for(auto& CurrentLayer : LayeredWidgets)
	{
		if(IsValid(CurrentLayer.Widget))
		{
			CurrentWidgets.Add(CurrentLayer.Widget);
		}
	}
	
	return CurrentWidgets;
}

TArray<FLayeredWidget> ULayeredUI_Subsystem::GetLayeredWidgets()
{
	return LayeredWidgets;
}

void ULayeredUI_Subsystem::AddWidgetToLayer(UUserWidget* Widget, FGameplayTag Layer, FLayeredWidget& LayeredWidget,
                                            int32 OrderOverride)
{
}

void ULayeredUI_Subsystem::RemoveWidgetFromLayer(FLayeredWidget Widget, FLayeredWidget& NewFocus)
{
}

void ULayeredUI_Subsystem::FindFirstWidgetOnLayer(FGameplayTag Layer, FLayeredWidget& Widget)
{
	FLayeredWidget HigherPriorityWidget;
	HigherPriorityWidget.LayerPriority = -1;
	for(auto& CurrentLayer : LayeredWidgets)
	{
		if(CurrentLayer.Layer == Layer)
		{
			if(CurrentLayer.LayerPriority > HigherPriorityWidget.LayerPriority)
			{
				HigherPriorityWidget = CurrentLayer;
				Widget = HigherPriorityWidget;
			}
		}
	}
}
