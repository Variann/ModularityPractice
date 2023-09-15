// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_ImGuiBeginBase.h"

#include "Utilities/CoreTechK2Utilities.h"

// BlueprintGraph
#include "K2Node_AssignmentStatement.h"
#include "K2Node_CallFunction.h"
#include "K2Node_ExecutionSequence.h"
#include "K2Node_IfThenElse.h"
#include "K2Node_TemporaryVariable.h"

// Kismet
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// KismetCompiler
#include "ImGuiBlueprintLibrary.h"
#include "KismetCompiler.h"
#include "Utilities/CoreTechK2Utilities.h"

#define LOCTEXT_NAMESPACE "UK2Node_ImGuiBeginBase"

const FName UK2Node_ImGuiBeginBase::OnActiveExecutionPinName(TEXT("OnActivePin"));
const FName UK2Node_ImGuiBeginBase::NamePinName(TEXT("NamePin"));

void UK2Node_ImGuiBeginBase::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	CoreTechK2Utilities::DefaultGetMenuActions(this, ActionRegistrar);
}

FText UK2Node_ImGuiBeginBase::GetMenuCategory() const
{
	return LOCTEXT("MenuCategory", "ImGui|Basic");
}

void UK2Node_ImGuiBeginBase::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	// Execution pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);

	// Label pin

		const auto LabelPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Text, GetLabelPinName());
		LabelPin->PinType.bIsConst = true;
		LabelPin->DefaultTextValue = FText::AsCultureInvariant("");
		LabelPin->bHidden = !IsLabelRequired();
	
	const auto OnClickPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, OnActiveExecutionPinName);
	OnClickPin->PinFriendlyName = LOCTEXT("OnActive_FriendlyName", "On Active");
	
	// Then Pin
	const auto ThenPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
}

void UK2Node_ImGuiBeginBase::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);


	const auto K2Schema = GetDefault<UEdGraphSchema_K2>();

	// Cache pins
	const auto ExecPin = GetExecPin();

	const auto OnActivePin = FindPinChecked(OnActiveExecutionPinName);
	const auto ThenPin = GetThenPin();

	// Create a Begin() ImGui function call
	const auto Begin = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	Begin->FunctionReference.SetExternalMember(GetBeginFunction(), UImGuiBlueprintLibrary::StaticClass());
	Begin->AllocateDefaultPins();
	
	const auto FuncReturnValuePin = Begin->GetReturnValuePin();

	CompilerContext.MovePinLinksToIntermediate(*ExecPin, *Begin->GetExecPin());
	if(IsLabelRequired())
	{
		const auto NamePin = FindPinChecked(GetLabelPinName());
		const auto FuncNamePin = Begin->FindPinChecked(GetLabelPinName());
		CompilerContext.MovePinLinksToIntermediate(*NamePin, *FuncNamePin);
	}

	// Create a sequence node
	const auto Sequence = CompilerContext.SpawnIntermediateNode<UK2Node_ExecutionSequence>(this, SourceGraph);
	Sequence->AllocateDefaultPins();

	const auto SequenceExec = Sequence->GetExecPin();
	const auto SequenceOne = Sequence->GetThenPinGivenIndex(0);
	const auto SequenceTwo = Sequence->GetThenPinGivenIndex(1);

	// Connect Begin() ImGui function call output pin to sequence
	Begin->GetThenPin()->MakeLinkTo(SequenceExec);

	// Create branch
	const auto Branch = CompilerContext.SpawnIntermediateNode<UK2Node_IfThenElse>(this, SourceGraph);
	Branch->AllocateDefaultPins();

	SequenceOne->MakeLinkTo(Branch->GetExecPin());
	Branch->GetConditionPin()->MakeLinkTo(FuncReturnValuePin);


	if(AlwaysRequiresEndCall())
	{
		// Connect OnActive Pin directly to branch result
		CompilerContext.MovePinLinksToIntermediate(*OnActivePin, *Branch->GetThenPin());
		
		// Create an End() ImGui function call
		const auto End = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
		End->FunctionReference.SetExternalMember(GetEndFunction(), UImGuiBlueprintLibrary::StaticClass());
		End->AllocateDefaultPins();

		// Connect Sequence Two output to End()
		SequenceTwo->MakeLinkTo(End->GetExecPin());
		
		CompilerContext.MovePinLinksToIntermediate(*ThenPin, *End->GetThenPin());
	}
	else
	{
		// Create a sequence node for active so we can call End when done
		const auto ActiveSequence = CompilerContext.SpawnIntermediateNode<UK2Node_ExecutionSequence>(this, SourceGraph);
		ActiveSequence->AllocateDefaultPins();

		const auto ActiveSequenceExec = ActiveSequence->GetExecPin();
		const auto ActiveSequenceOne = ActiveSequence->GetThenPinGivenIndex(0);
		const auto ActiveSequenceTwo = ActiveSequence->GetThenPinGivenIndex(1);

		Branch->GetThenPin()->MakeLinkTo(ActiveSequenceExec);
		
		// Connect OnActive Pin directly to branch result
		CompilerContext.MovePinLinksToIntermediate(*OnActivePin, *ActiveSequenceOne);

		// Create an End() ImGui function call
		const auto End = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
		End->FunctionReference.SetExternalMember(GetEndFunction(), UImGuiBlueprintLibrary::StaticClass());
		End->AllocateDefaultPins();

		// Connect Sequence Two output to End()
		ActiveSequenceTwo->MakeLinkTo(End->GetExecPin());
		
		CompilerContext.MovePinLinksToIntermediate(*ThenPin, *SequenceTwo);
	}

	BreakAllNodeLinks();
}

FText UK2Node_ImGuiBeginBase::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("NodeTitle", "ImGui Window");
}

FText UK2Node_ImGuiBeginBase::GetTooltipText() const
{
	return LOCTEXT("ToolTipText", "calls Begin() and End()");
}

#undef LOCTEXT_NAMESPACE
