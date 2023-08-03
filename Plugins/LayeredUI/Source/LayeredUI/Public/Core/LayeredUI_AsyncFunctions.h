// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "Data/CoreLayeredUIData.h"
#include "LayeredUI_AsyncFunctions.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAddWidgetSuccess, FLayeredWidget, NewWidget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAddWidgetFailed, FLayeredWidget, NewWidget);


UCLASS()
class LAYEREDUI_API ULayeredUI_AddWidgetToLayer : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FAddWidgetSuccess Success;

	UPROPERTY(BlueprintAssignable)
	FAddWidgetFailed Fail;

	UPROPERTY()
	TSoftClassPtr<UUserWidget> WidgetToLoad;
	UPROPERTY()
	UUserWidget* LoadedWidget;
	
	//Needed for loading soft reference.
	FStreamableManager StreamableManager;
	TSharedPtr<FStreamableHandle> Handle;

	UPROPERTY()
	FGameplayTag LayerDestination;

	UPROPERTY()
	int32 DesiredOrderOverride;

	UPROPERTY()
	UObject* WidgetOwner;

public:

	UFUNCTION(Category="LayeredUI", BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="Context"), DisplayName = "Add widget to Layer (Async)")
	static ULayeredUI_AddWidgetToLayer* AddWidgetToLayer_Async(TSoftClassPtr<UUserWidget> Widget, FGameplayTag Layer, UObject* Context, int32 OrderOverride = -1);

	virtual void Activate() override;
};
