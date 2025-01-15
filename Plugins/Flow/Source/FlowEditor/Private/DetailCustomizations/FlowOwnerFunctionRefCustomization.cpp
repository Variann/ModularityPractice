// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "DetailCustomizations/FlowOwnerFunctionRefCustomization.h"

#include "FlowAsset.h"
#include "Interfaces/FlowOwnerInterface.h"
#include "Nodes/FlowNode.h"
#include "Nodes/World/FlowNode_CallOwnerFunction.h"

#include "UObject/UnrealType.h"
#include "Types/FlowOwnerFunctionParams.h"

void FFlowOwnerFunctionRefCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	// Do not include children properties (the header is all we need to show for this struct)
}

TSharedPtr<IPropertyHandle> FFlowOwnerFunctionRefCustomization::GetCuratedNamePropertyHandle() const
{
	check(StructPropertyHandle->IsValidHandle());

	TSharedPtr<IPropertyHandle> FoundHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFlowOwnerFunctionRef, FunctionName));
	check(FoundHandle);

	return FoundHandle;
}

TArray<FName> FFlowOwnerFunctionRefCustomization::GetCuratedNameOptions() const
{
	TArray<FName> Results;

	const UClass* ExpectedOwnerClass = TryGetExpectedOwnerClass();
	if (!IsValid(ExpectedOwnerClass))
	{
		return Results;
	}

	const UFlowNode_CallOwnerFunction* FlowNodeOwner = Cast<UFlowNode_CallOwnerFunction>(TryGetFlowNodeOuter());
	if (!IsValid(FlowNodeOwner))
	{
		return Results;
	}

	Results = GetFlowOwnerFunctionRefs(*FlowNodeOwner, *ExpectedOwnerClass);

	return Results;
}

const UClass* FFlowOwnerFunctionRefCustomization::TryGetExpectedOwnerClass() const
{
	const UFlowNode* NodeOwner = TryGetFlowNodeOuter();
	if (!IsValid(NodeOwner))
	{
		return nullptr;
	}

	const UFlowAsset* FlowAsset = NodeOwner->GetFlowAsset();
	if (!IsValid(FlowAsset))
	{
		return nullptr;
	}

	UClass* ExpectedOwnerClass = FlowAsset->GetExpectedOwnerClass();
	return ExpectedOwnerClass;
}

TArray<FName> FFlowOwnerFunctionRefCustomization::GetFlowOwnerFunctionRefs(
	const UFlowNode_CallOwnerFunction& FlowNodeOwner,
	const UClass& ExpectedOwnerClass)
{
	TArray<FName> ValidFunctionNames;

	// Gather a list of potential functions
	TSet<FName> PotentialFunctionNames;

	const UClass* CurClass = &ExpectedOwnerClass;
	while (IsValid(CurClass))
	{
		TArray<FName> CurClassFunctionNames;
		CurClass->GenerateFunctionList(CurClassFunctionNames);

		PotentialFunctionNames.Append(CurClassFunctionNames);

		// Recurse to include all of the Super(s) names
		CurClass = CurClass->GetSuperClass();
	}

	if (PotentialFunctionNames.Num() == 0)
	{
		return ValidFunctionNames;
	}

	ValidFunctionNames.Reserve(PotentialFunctionNames.Num());

	// Filter out any unusable functions (that do not match the expected signature)
	for (const FName& PotentialFunctionName : PotentialFunctionNames)
	{
		const UFunction* PotentialFunction = ExpectedOwnerClass.FindFunctionByName(PotentialFunctionName);
		check(IsValid(PotentialFunction));

		if (IsFunctionUsable(*PotentialFunction, FlowNodeOwner))
		{
			ValidFunctionNames.Add(PotentialFunctionName);
		}
	}

	return ValidFunctionNames;
}

bool FFlowOwnerFunctionRefCustomization::IsFunctionUsable(const UFunction& Function, const UFlowNode_CallOwnerFunction& FlowNodeOwner)
{
	if (!UFlowNode_CallOwnerFunction::DoesFunctionHaveValidFlowOwnerFunctionSignature(Function))
	{
		return false;
	}

	return true;
}

void FFlowOwnerFunctionRefCustomization::SetCuratedName(const FName& NewFunctionName)
{
	TSharedPtr<IPropertyHandle> FunctionNameHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFlowOwnerFunctionRef, FunctionName));

	check(FunctionNameHandle);

	FunctionNameHandle->SetPerObjectValue(0, NewFunctionName.ToString());
}

bool FFlowOwnerFunctionRefCustomization::TryGetCuratedName(FName& OutName) const
{
	const FFlowOwnerFunctionRef* FlowOwnerFunction = GetFlowOwnerFunctionRef();
	if (FlowOwnerFunction)
	{
		OutName = FlowOwnerFunction->FunctionName;

		return true;
	}
	else
	{
		return false;
	}
}

UFlowNode* FFlowOwnerFunctionRefCustomization::TryGetFlowNodeOuter() const
{
	check(StructPropertyHandle->IsValidHandle());

	TArray<UObject*> OuterObjects;
	StructPropertyHandle->GetOuterObjects(OuterObjects);

	for (UObject* OuterObject : OuterObjects)
	{
		UFlowNode* FlowNodeOuter = Cast<UFlowNode>(OuterObject);
		if (IsValid(FlowNodeOuter))
		{
			return FlowNodeOuter;
		}
	}

	return nullptr;
}
