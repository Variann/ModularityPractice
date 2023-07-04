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

#endif

void UFN_Portal::ExecuteInput(const FName& PinName)
{
	UFlowAsset* FlowAsset = GetFlowAsset();

	if(!FlowAsset)
	{
		return;
	}

	TArray<UFlowNode*> FoundNodes;
	FlowAsset->PreloadNodes();
	const TMap<FGuid, UFlowNode*>& AssetNodes = FlowAsset->GetNodes();
	AssetNodes.GenerateValueArray(FoundNodes);

	for(auto& CurrentNode : FoundNodes)
	{
		UFN_Portal* OtherPortal = Cast<UFN_Portal>(CurrentNode);
		if(OtherPortal)
		{
			if(OtherPortal->PortalDirection == Exit && OtherPortal->PortalID == PortalID && OtherPortal->OutputPins.IsValidIndex(0))
			{
				OtherPortal->TriggerOutput(OtherPortal->OutputPins[0].PinName);
			}
		}
	}
}
