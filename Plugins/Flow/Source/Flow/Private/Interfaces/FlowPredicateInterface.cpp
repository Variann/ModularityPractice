// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Interfaces/FlowPredicateInterface.h"
#include "AddOns/FlowNodeAddOn.h"

bool IFlowPredicateInterface::ImplementsInterfaceSafe(const UFlowNodeAddOn* AddOnTemplate)
{
	if (!IsValid(AddOnTemplate))
	{
		return false;
	}

	UClass* AddOnClass = AddOnTemplate->GetClass();
	if (AddOnClass->ImplementsInterface(UFlowPredicateInterface::StaticClass()))
	{
		return true;
	}

	return false;
}
