// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "imgui.h"
#include "K2Node_TreeNode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTreeCreated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTreeFinished);

UCLASS()
class IMGUIBLUEPRINT_API UK2Node_TreeNode : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FTreeCreated TreeCreated;
	
	UPROPERTY(BlueprintAssignable)
	FTreeFinished Finish;

	UPROPERTY()
	FString TreeLabel;

public:

	UFUNCTION(Category="ImGui|Menus", BlueprintCallable, meta=(BlueprintInternalUseOnly="true"))
	static UK2Node_TreeNode* ImGui_TreeNode(FString Label);

	virtual void Activate() override;
};
