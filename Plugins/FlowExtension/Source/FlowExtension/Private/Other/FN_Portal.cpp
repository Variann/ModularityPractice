// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Other/FN_Portal.h"

#include "FlowAsset.h"

UFN_Portal::UFN_Portal(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("Route");
#endif

	OutputPins.Empty();
	InputPins.Empty();
	RefreshPins();
}

#if WITH_EDITOR

void UFN_Portal::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	RefreshPins();
	PortalGUID = GetGuid();
	
	OnReconstructionRequested.ExecuteIfBound();
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UFN_Portal::RefreshPins()
{
	OutputPins.Empty();
	InputPins.Empty();
	
	if(PortalDirection == Entrance)
	{
		InputPins.Add(TEXT("In"));
	}
	else
	{
		OutputPins.Add(TEXT("Out"));
	}
}

EDataValidationResult UFN_Portal::ValidateNode()
{
	if(PortalDirection == Entrance && !PortalGUIDToTrigger.IsValid())
	{
		ValidationLog.Error<UFlowNode>(TEXT("Portal has no exit portal to trigger."), this);
		return EDataValidationResult::Invalid;
	}
	
	return EDataValidationResult::Valid;
}

#endif

void UFN_Portal::ExecuteInput(const FName& PinName)
{
	UFlowAsset* FlowAsset = GetFlowAsset();

	if(!FlowAsset)
	{
		return;
	}

	if(UFN_Portal* OtherPortal = Cast<UFN_Portal>(FlowAsset->GetNode(PortalGUIDToTrigger)))
	{
		OtherPortal->TriggerOutput(OtherPortal->OutputPins[0].PinName);
	}
}
