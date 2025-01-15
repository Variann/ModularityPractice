// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Pins/SFlowOutputPinHandle.h"
#include "Nodes/FlowNode.h"
#include "Pins/SFlowPinHandle.h"

#include "EdGraphSchema_K2.h"
#include "Engine/Blueprint.h"

void SFlowOutputPinHandle::RefreshNameList()
{
	PinNames.Empty();

	if (Blueprint && Blueprint->GeneratedClass && Blueprint->GeneratedClass->IsChildOf<UFlowNode>())
	{
		if (UFlowNode* FlowNode = Blueprint->GeneratedClass->GetDefaultObject<UFlowNode>())
		{
			for (const FFlowPin& OutputPin : FlowNode->OutputPins)
			{
				PinNames.Add(MakeShareable(new FName(OutputPin.PinName)));
			}
		}
	}
}

TSharedPtr<SGraphPin> FFlowOutputPinHandleFactory::CreatePin(UEdGraphPin* InPin) const
{
	if (InPin->PinType.PinCategory == GetDefault<UEdGraphSchema_K2>()->PC_Struct && InPin->PinType.PinSubCategoryObject == FFlowOutputPinHandle::StaticStruct() && InPin->LinkedTo.Num() == 0)
	{
		if (const UEdGraphNode* GraphNode = InPin->GetOuter())
		{
			if (const UBlueprint* Blueprint = GraphNode->GetGraph()->GetTypedOuter<UBlueprint>())
			{
				return SNew(SFlowOutputPinHandle, InPin, Blueprint);
			}
		}
	}

	return nullptr;
}
