// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Core/LayeredUI_Subsystem.h"

#include <Config/DS_LayeredUIGameSettings.h>

#include "CommonActivatableWidget.h"
#include "Blueprint/UserWidget.h"
#include "Data/I_LayeringCommunication.h"
#include "Components/PanelWidget.h"
#include "Core/W_UI_Manager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

void ULayeredUI_Subsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if(const UDS_LayeredUIGameSettings* UIData = GetDefault<UDS_LayeredUIGameSettings>())
	{
		if(IsValid(UIData->UIManagerClass))
		{
			UW_UI_Manager* CreatedWidget = Cast<UW_UI_Manager>(CreateWidget(UGameplayStatics::GetPlayerController(this, 0), UIData->UIManagerClass));
			if(CreatedWidget)
			{
				CreatedWidget->AddToViewport(UIData->UIManagerZOrder);
				UIManager = CreatedWidget;
			}
		}
	}
}

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

bool ULayeredUI_Subsystem::IsWidgetValid(const UObject* WorldContextObject, FLayeredWidget Widget)
{
	if(!IsValid(Widget.Widget))
	{
		return false;
	}
	
	if(!UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		return false;
	}

	ULayeredUI_Subsystem* LayeredUI_Subsystem = UGameplayStatics::GetPlayerController(WorldContextObject, 0)->GetLocalPlayer()->GetSubsystem<ULayeredUI_Subsystem>();
	if(!LayeredUI_Subsystem)
	{
		return false;
	}
	
	for(const auto& CurrentWidget : LayeredUI_Subsystem->GetLayeredWidgets())
	{
		if(CurrentWidget.Widget == Widget.Widget)
		{
			return true;
		}
	}

	return false;
}

void ULayeredUI_Subsystem::AddWidgetToLayer(const UObject* WorldContextObject, UUserWidget* Widget, FGameplayTag Layer, FLayeredWidget& LayeredWidget)
{
	LayeredWidget = FLayeredWidget();
	if(!UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		return;
	}

	ULayeredUI_Subsystem* LayeredUI_Subsystem = UGameplayStatics::GetPlayerController(WorldContextObject, 0)->GetLocalPlayer()->GetSubsystem<ULayeredUI_Subsystem>();
	if(!LayeredUI_Subsystem)
	{
		return;
	}

	if(!IsValid(Widget))
	{
		return;
	}
	
	LayeredUI_Subsystem->AddWidgetToLayer_Internal(Widget, Layer, LayeredWidget);
}

void ULayeredUI_Subsystem::AddWidgetToLayer_Internal(UUserWidget* Widget, FGameplayTag Layer, FLayeredWidget& LayeredWidget)
{
	LayeredWidget = FLayeredWidget();
	
	for(auto& CurrentLayer : LayeredWidgets)
	{
		if(CurrentLayer.Widget->GetClass() == Widget->GetClass())
		{
			if(UKismetSystemLibrary::DoesImplementInterface(Widget, UI_LayeringCommunication::StaticClass()))
			{
				if(!II_LayeringCommunication::Execute_AllowMultipleInstances(Widget))
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

	if(UWidget* Slot = GetSlotForLayer(Layer))
	{
		//If the slot and widget we're adding are compatible with CommonUI,
		//use the CommonUI workflow
		if(UCommonActivatableWidgetContainerBase* Stack = Cast<UCommonActivatableWidgetContainerBase>(Slot); IsValid(Stack)
			&& Widget->GetClass()->IsChildOf(UCommonActivatableWidget::StaticClass()))
		{
			Stack->AddWidget(Widget->GetClass());
			LayeredWidget.Widget = Widget;
			return;
		}

		if(UPanelWidget* PanelSlot = Cast<UPanelWidget>(Slot))
		{
			PanelSlot->AddChild(Widget);
			LayeredWidget.Widget = Widget;
			return;
		}
	}
	
	//Widget has passed verification, start adding it to the screen.
	TMap<FGameplayTag, int32> LayerMap = GetLayerMap();
	const int32* ZOrder = LayerMap.Find(Layer);
	if(ZOrder)
	{
		FLayeredWidget NewLayeredWidget;
		NewLayeredWidget.Widget = Widget;
		NewLayeredWidget.Layer = Layer;
		NewLayeredWidget.ZOrder = *ZOrder;
		LayeredWidget = NewLayeredWidget;

		//Might be called on different thread, engine crashes if we try to add the widget
		//to viewport outside of the game thread.
		if(UKismetSystemLibrary::DoesImplementInterface(Widget, UI_LayeringCommunication::StaticClass()))
		{
			II_LayeringCommunication::Execute_SetWidgetLayerData(Widget, NewLayeredWidget);
		}
		
		LayeredWidgets.Add(NewLayeredWidget);
		Widget->AddToViewport(*ZOrder);
		WidgetAdded.Broadcast(NewLayeredWidget);
	}
	else
	{
		UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Tag %ls has not been added to the project settings layer map."), *Layer.ToString()));
	}
}

void ULayeredUI_Subsystem::RemoveWidgetFromLayer(const UObject* WorldContextObject, UUserWidget* Widget)
{
	if(!UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		return;
	}

	ULayeredUI_Subsystem* LayeredUI_Subsystem = UGameplayStatics::GetPlayerController(WorldContextObject, 0)->GetLocalPlayer()->GetSubsystem<ULayeredUI_Subsystem>();
	if(!LayeredUI_Subsystem)
	{
		return;
	}
	
	LayeredUI_Subsystem->RemoveWidgetFromLayer_Internal(Widget);
}

void ULayeredUI_Subsystem::RemoveWidgetFromLayer_Internal(UUserWidget* Widget)
{
	if(!IsValid(Widget))
	{
		return;
	}

	int32 WidgetIndex = LayeredWidgets.Find(FLayeredWidget(Widget));
	if(WidgetIndex == INDEX_NONE)
	{
		for(auto& CurrentSlot: ActiveSlots)
		{
			if(IsValid(CurrentSlot.Value.Get()))
			{
				if(UPanelWidget* PanelSlot = Cast<UPanelWidget>(CurrentSlot.Value.Get()))
				{
					if(PanelSlot->RemoveChild(Widget))
					{
						return;
					}
				}
			}
		}
		
		return;
	}
	
	FLayeredWidget LayeredWidget = LayeredWidgets[WidgetIndex];
	if(!LayeredWidgets.RemoveSingle(LayeredWidget))
	{
		return;
	}
	
	Widget->RemoveFromParent();
	Widget->SetIsEnabled(false);

	WidgetRemoved.Broadcast(LayeredWidget);
}

void ULayeredUI_Subsystem::FindFirstWidgetOnLayer(const UObject* WorldContextObject, FGameplayTag Layer,
	FLayeredWidget& Widget)
{
	Widget = FLayeredWidget();
	if(!UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		return;
	}

	ULayeredUI_Subsystem* LayeredUI_Subsystem = UGameplayStatics::GetPlayerController(WorldContextObject, 0)->GetLocalPlayer()->GetSubsystem<ULayeredUI_Subsystem>();
	if(!LayeredUI_Subsystem)
	{
		return;
	}
	
	LayeredUI_Subsystem->FindFirstWidgetOnLayer_Internal(Layer, Widget);
}

void ULayeredUI_Subsystem::FindFirstWidgetOnLayer_Internal(FGameplayTag Layer, FLayeredWidget& Widget)
{
	Widget = FLayeredWidget();
	
	for(auto& CurrentLayer : LayeredWidgets)
	{
		if(CurrentLayer.Layer == Layer)
		{
			Widget = CurrentLayer;
		}
	}

	if(!ActiveSlots.Find(Layer))
	{
		return;
	}
	
	if(UWidget* Slot = ActiveSlots.Find(Layer)->Get())
	{
		if(UPanelWidget* PanelWidget = Cast<UPanelWidget>(Slot))
		{
			if(UUserWidget* SlotWidget = Cast<UUserWidget>(PanelWidget->GetChildAt(PanelWidget->GetChildrenCount() - 1)))
			{
				Widget.Layer = Layer;
				Widget.Widget = SlotWidget;
				return;
			}
			
		}
		
		if(UUserWidget* SlotWidget = Cast<UUserWidget>(Slot))
		{
			Widget.Layer = Layer;
			Widget.Widget = SlotWidget;
			return;
		}
	}
}

UWidget* ULayeredUI_Subsystem::GetSlotForLayer(FGameplayTag Layer)
{
	return ActiveSlots.FindRef(Layer);
}

void ULayeredUI_Subsystem::RegisterSlot(const UObject* WorldContextObject, UWidget* Slot, FGameplayTag Layer)
{
	if(!UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		return;
	}

	ULayeredUI_Subsystem* LayeredUI_Subsystem = UGameplayStatics::GetPlayerController(WorldContextObject, 0)->GetLocalPlayer()->GetSubsystem<ULayeredUI_Subsystem>();
	if(!LayeredUI_Subsystem)
	{
		return;
	}
	UCommonActivatableWidgetContainerBase* Stack = Cast<UCommonActivatableWidgetContainerBase>(Slot);
	UPanelWidget* PanelSlot = Cast<UPanelWidget>(Slot);
	if(Stack || PanelSlot)
	{
		LayeredUI_Subsystem->ActiveSlots.Add(Layer, Slot);
	}
	else
	{
		UKismetSystemLibrary::PrintString(WorldContextObject, "Slot is not a child of UCommonActivatableWidgetContainerBase or UPanelWidget, can't register slot");
	}
}
