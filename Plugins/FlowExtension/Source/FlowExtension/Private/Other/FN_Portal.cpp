// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Other/FN_Portal.h"

#include "FlowAsset.h"
#include "Framework/Notifications/NotificationManager.h"

//Need this for the focus button
#if WITH_EDITOR
#include "Graph/FlowGraphEditor.h"
#include "Graph/FlowGraphUtils.h"
#endif

#include "Widgets/Notifications/SNotificationList.h"

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

#if WITH_EDITOR

void UFN_Portal::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	RefreshPins();
	PortalGUID = GetGuid();

	//Update the PortalID so the node description is correct.
	if(PortalDirection == Exit)
	{
		//Make sure no other Exit portals have the same ID
		if(PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UFN_Portal, PortalID) && !PortalID.IsNone())
		{
			UFlowAsset* FlowAsset = GetFlowAsset();

			for(auto& CurrentNode : FlowAsset->GetNodes())
			{
				if(UFN_Portal* OtherPortal = Cast<UFN_Portal>(CurrentNode.Value))
				{
					if(OtherPortal->PortalID == PortalID && OtherPortal != this)
					{
						//reset the name
						PortalID = "";

						//Spit out a warning on the bottom right
						FText InfoText = FText::FromString("PortalID already in use.");
						FNotificationInfo Info(InfoText);
						Info.bUseThrobber = false;
						Info.FadeOutDuration = 0.5f;
						Info.ExpireDuration = 5.0f;
						if (TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info))
						{
							Notification->SetCompletionState(SNotificationItem::CS_Fail);
						}
					}
				}
			}
		}
		
		RefreshConnectedPortals();
	}
	else
	{
		//This is a entrance, get the exit portal and use its PortalID
		UFlowAsset* FlowAsset = GetFlowAsset();

		if(FlowAsset)
		{
			if(UFN_Portal* OtherPortal = Cast<UFN_Portal>(FlowAsset->GetNode(PortalGUIDToTrigger)))
			{
				PortalID = OtherPortal->PortalID;
			}
		}
	}
	
	OnReconstructionRequested.ExecuteIfBound();
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UFN_Portal::RefreshConnectedPortals()
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
			if(OtherPortal->PortalDirection == Entrance && OtherPortal->PortalGUIDToTrigger == PortalGUID)
			{
				OtherPortal->PortalID = PortalID;
			}
		}
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

void UFN_Portal::FocusExitPortal()
{
	const UFlowAsset* FlowAsset = GetFlowAsset();
	if(PortalDirection == Entrance)
	{
		if(FlowAsset)
		{
			if(const UFN_Portal* ExitPortal = Cast<UFN_Portal>(FlowAsset->GetNode(PortalGUIDToTrigger)))
			{
				if(const TSharedPtr<SFlowGraphEditor> FlowGraphEditor = FFlowGraphUtils::GetFlowGraphEditor(GetGraphNode()->GetGraph()))
				{
					FlowGraphEditor->JumpToNode(ExitPortal->GetGraphNode());
				}
			}
		}
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

	if(UFN_Portal* OtherPortal = Cast<UFN_Portal>(FlowAsset->GetNode(PortalGUIDToTrigger)))
	{
		OtherPortal->TriggerOutput(OtherPortal->OutputPins[0].PinName);
	}
}
