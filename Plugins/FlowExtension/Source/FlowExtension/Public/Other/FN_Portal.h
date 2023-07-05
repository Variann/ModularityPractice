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

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EPortalDirection> PortalDirection = Entrance;

	UPROPERTY(EditAnywhere)
	FName PortalID;

	//GUID of the exit node
	UPROPERTY(VisibleAnywhere, meta = (EditCondition = "PortalDirection == EPortalDirection::Exit", EditConditionHides))
	FGuid PortalGUID;

	//What exit node should this entrance node trigger?
	UPROPERTY(EditAnywhere, meta = (EditCondition = "PortalDirection == EPortalDirection::Entrance", EditConditionHides))
	FGuid PortalGUIDToTrigger;

#if WITH_EDITOR

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	void RefreshPins();

	virtual FString GetNodeDescription() const override { return PortalID.ToString(); }

	virtual EDataValidationResult ValidateNode() override;

#endif
	
	virtual void ExecuteInput(const FName& PinName) override;
};
