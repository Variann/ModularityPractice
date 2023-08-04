// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Core/LayeredUI_Subsystem.h"

#include <Config/DS_LayeredUIGameSettings.h>

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Data/I_WidgetCommunication.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

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
	LayeredWidget = FLayeredWidget();
	
	for(auto& CurrentLayer : LayeredWidgets)
	{
		if(CurrentLayer.Widget->GetClass() == Widget->GetClass())
		{
			if(UKismetSystemLibrary::DoesImplementInterface(Widget, UI_WidgetCommunication::StaticClass()))
			{
				if(!II_WidgetCommunication::Execute_AllowMultipleInstances(Widget))
				{
					UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%ls does not allow multiple instances"), *Widget->GetClass()->GetName()));
					return;
				}
			}
		}
	
		if(CurrentLayer.Widget == Widget)
		{
			UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%p Widget instance is already on the screen."), Widget->GetClass()));
			return;
		}
	}
	
	//Widget has passed verification, start adding it to the screen.
	TMap<FGameplayTag, int32> LayerMap = GetLayerMap();
	const int32* ZOrder = LayerMap.Find(Layer);
	if(ZOrder)
	{
		bool HideCursor = false;
		if(UKismetSystemLibrary::DoesImplementInterface(Widget, UI_WidgetCommunication::StaticClass()))
		{
			HideCursor = II_WidgetCommunication::Execute_HideCursor(Widget);
		}
		FLayeredWidget NewLayeredWidget;
		NewLayeredWidget.Widget = Widget;
		NewLayeredWidget.Layer = Layer;
		NewLayeredWidget.ZOrder = *ZOrder;
		NewLayeredWidget.HideCursor = HideCursor;

		if(UKismetSystemLibrary::DoesImplementInterface(Widget, UI_WidgetCommunication::StaticClass()))
		{
			II_WidgetCommunication::Execute_SetWidgetLayerData(Widget, NewLayeredWidget);
		}
		
		LayeredWidgets.Add(NewLayeredWidget);
	
		Widget->AddToViewport(*ZOrder);
		UGameplayStatics::GetPlayerController(this , 0)->SetShowMouseCursor(!HideCursor);
		LayeredWidget = NewLayeredWidget;
		return;
	}
	else
	{
		UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Tag %ls has not been added to the project settings layer map."), *Layer.ToString()));
	}
}

void ULayeredUI_Subsystem::RemoveWidgetFromLayer(FLayeredWidget& Widget, FLayeredWidget& NewFocus)
{
	NewFocus = FLayeredWidget();
	
	if(!IsValid(Widget.Widget))
	{
		return;
	}
	
	if(!LayeredWidgets.RemoveSingle(Widget))
	{
		return;
	}
	
	Widget.Widget->RemoveFromParent();
	Widget.Widget->SetIsEnabled(false);
	
	FLayeredWidget NextWidget;
	/**Find the next widget to focus.
	 * A reverse loop is used because we want the most recent widget added to the screen.
	 * The latest widget is always added to the end of the array.
	 * This will also help with managing the focus and input priority on the correct
	 * widget, as Unreal always adds the latest widget on top of other widgets, even
	 * if they are on the same ZOrder to avoid Z-fighting*/
	for(int32 CurrentWidget = LayeredWidgets.Num(); CurrentWidget >= 0; CurrentWidget--)
	{
		if(IsValid(LayeredWidgets[CurrentWidget].Widget) && LayeredWidgets[CurrentWidget].ZOrder > NextWidget.ZOrder)
		{
			NextWidget = LayeredWidgets[CurrentWidget];
		}
	}
	
	if(IsValid(NextWidget.Widget))
	{
		if(NextWidget.Widget->IsFocusable())
		{
			NextWidget.Widget->SetFocus();
		}
		else
		{
			UWidgetBlueprintLibrary::SetFocusToGameViewport();
		}

		if(UKismetSystemLibrary::DoesImplementInterface(NextWidget.Widget, UI_WidgetCommunication::StaticClass()))
		{
			UGameplayStatics::GetPlayerController(this , 0)->SetShowMouseCursor(!II_WidgetCommunication::Execute_HideCursor(NextWidget.Widget));
		}
	
		NewFocus = NextWidget;
		return;
	}
	
	Widget = FLayeredWidget();
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
