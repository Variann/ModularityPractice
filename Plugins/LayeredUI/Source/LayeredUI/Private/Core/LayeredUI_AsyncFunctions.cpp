// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Core/LayeredUI_AsyncFunctions.h"

#include "Blueprint/UserWidget.h"
#include "Core/LayeredUI_Subsystem.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

ULayeredUI_AddWidgetToLayer* ULayeredUI_AddWidgetToLayer::AddWidgetToLayer_Async(TSoftClassPtr<UUserWidget> Widget,
                                                                                 FGameplayTag Layer, UObject* Context, int32 OrderOverride)
{
	ULayeredUI_AddWidgetToLayer* NewAsyncObject = NewObject<ULayeredUI_AddWidgetToLayer>(Context);
	NewAsyncObject->WidgetToLoad = Widget;
	NewAsyncObject->LayerDestination = Layer;
	NewAsyncObject->DesiredOrderOverride = OrderOverride;
	NewAsyncObject->WidgetOwner = Context;
	return NewAsyncObject;
}

void ULayeredUI_AddWidgetToLayer::Activate()
{
	Super::Activate();

	Handle = StreamableManager.RequestAsyncLoad(WidgetToLoad.ToSoftObjectPath(), [this]()
	{
		FLayeredWidget NewLayeredWidget;
		LoadedWidget = Cast<UUserWidget>(Handle->GetLoadedAsset());
		if(LoadedWidget)
		{
			UUserWidget* CreatedWidget = Cast<UUserWidget>(CreateWidget(WidgetOwner, LoadedWidget->GetClass()));

			ULayeredUI_Subsystem* LayeredUI_Subsystem = UGameplayStatics::GetPlayerController(WidgetOwner, 0)->GetLocalPlayer()->GetSubsystem<ULayeredUI_Subsystem>();
			if(!LayeredUI_Subsystem)
			{
				Fail.Broadcast(NewLayeredWidget);
				RemoveFromRoot();
				return;
			}
			
			LayeredUI_Subsystem->AddWidgetToLayer(CreatedWidget ,LayerDestination, NewLayeredWidget, DesiredOrderOverride);

			if(!IsValid(NewLayeredWidget.Widget))
			{
				Fail.Broadcast(NewLayeredWidget);
				RemoveFromRoot();
				return;
			}

			Success.Broadcast(NewLayeredWidget);
			RemoveFromRoot();
		}
	});
}
