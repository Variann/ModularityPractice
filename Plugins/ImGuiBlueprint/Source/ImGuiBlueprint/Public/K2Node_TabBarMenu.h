// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "imgui.h"
#include "K2Node_TabBarMenu.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTabAdded, int32, TabIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFinish);

UCLASS()
class IMGUIBLUEPRINT_API UK2Node_TabBarMenu : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FTabAdded TabAdded;

	UPROPERTY(BlueprintAssignable)
	FFinish Finish;

	UPROPERTY()
	TArray<FString> TabNames;

public:

	UFUNCTION(Category="ImGui|Menus", BlueprintCallable, meta=(BlueprintInternalUseOnly="true"))
	static UK2Node_TabBarMenu* ImGui_TabBarMenu(TArray<FString> Tabs);

	virtual void Activate() override;
};
