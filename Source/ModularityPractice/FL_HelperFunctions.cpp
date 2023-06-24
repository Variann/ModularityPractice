// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "FL_HelperFunctions.h"

UObject* UFL_HelperFunctions::GetObjectsDefaultObject(UObject* Object)
{
	if(IsValid(Object))
	{
		UObject* TempObject = Object->GetClass()->GetDefaultObject();
		FString ObjectName = TempObject->GetName();
		return TempObject;
	}
	return nullptr;
}

TArray<UFlowNode*> UFL_HelperFunctions::GetFlowNodes(UFlowAsset* FlowAsset)
{
	// UFlowGraph* NewAsset = NewObject<UFlowGraph>();
	TArray<UFlowNode*> FoundNodes;

	if(FlowAsset)
	{
		FlowAsset->PreloadNodes();
		const TMap<FGuid, UFlowNode*>& AssetNodes = FlowAsset->GetNodes();
		AssetNodes.GenerateValueArray(FoundNodes);
		
		return FoundNodes;
	}

	return FoundNodes;
}
