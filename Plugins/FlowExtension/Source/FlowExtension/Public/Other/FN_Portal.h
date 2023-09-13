// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "FN_Portal.generated.h"

UENUM()
enum EPortalDirection
{
	Entrance,
	Exit
};

UCLASS(DisplayName = "Portal")
class FLOWEXTENSION_API UFN_Portal : public UFlowNode
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(Category = "Portal Settings", EditAnywhere)
	TEnumAsByte<EPortalDirection> PortalDirection = Entrance;

	//General description of what this portal is meant for.
	UPROPERTY(Category = "Portal Settings", EditAnywhere, meta = (EditCondition = "PortalDirection == EPortalDirection::Exit", EditConditionHides))
	FName PortalID;

	//GUID of the exit node
	UPROPERTY()
	FGuid PortalGUID;

	UPROPERTY(Category = "Portal Settings", EditAnywhere, meta = (EditCondition = "PortalDirection == EPortalDirection::Entrance", EditConditionHides, GetOptions = "GetAvailablePortals"))
	FName PortalToTrigger;

	//What exit node should this entrance node trigger?
	// UPROPERTY(Category = "Portal Settings", EditAnywhere, DisplayName = "Portal GUID to Trigger", meta = (EditCondition = "PortalDirection == EPortalDirection::Entrance", EditConditionHides))
	UPROPERTY()
	FGuid PortalGUIDToTrigger;

	void RefreshPins();
	
#if WITH_EDITOR
	
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	void RefreshConnectedPortals();

	virtual FString GetNodeDescription() const override { return PortalID.ToString(); }

	virtual EDataValidationResult ValidateNode() override;

	UFUNCTION(Category = "Portal Settings", CallInEditor)
	void FocusExitPortal();

#endif
	
	virtual void ExecuteInput(const FName& PinName) override;

	UFUNCTION()
	TArray<FName> GetAvailablePortals();
};
