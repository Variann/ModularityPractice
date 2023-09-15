// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_ImGuiButton.h"

#include "EdGraphSchema_K2.h"


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

#define LOCTEXT_NAMESPACE "UK2Node_ImGuiButton"

const FName UK2Node_ImGuiButton::OnClickExecutionPinName(TEXT("OnClickPin"));
const FName UK2Node_ImGuiButton::NamePinName(TEXT("Text"));
const FName UK2Node_ImGuiButton::SizePinName(TEXT("SizePin"));

void UK2Node_ImGuiButton::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	CoreTechK2Utilities::DefaultGetMenuActions(this, ActionRegistrar);
}

FText UK2Node_ImGuiButton::GetMenuCategory() const
{
	return LOCTEXT("MenuCategory", "ImGui|Basic");
}

void UK2Node_ImGuiButton::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	// Execution pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);

	// Button Name pin
	const auto NamePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_String, NamePinName);
	NamePin->PinType.bIsConst = true;
	NamePin->DefaultTextValue = FText::AsCultureInvariant("");

	
	// Button Size Pin
	const auto SizePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, TBaseStructure<FVector2D>::Get(), SizePinName);
	SizePin->bAdvancedView = true;

	const auto OnClickPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, OnClickExecutionPinName);
	OnClickPin->PinFriendlyName = LOCTEXT("OnClick_FriendlyName", "On Click");


	// Then Pin
	const auto ThenPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

	if(AdvancedPinDisplay == ENodeAdvancedPins::NoPins)
		AdvancedPinDisplay = ENodeAdvancedPins::Hidden;
}

void UK2Node_ImGuiButton::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	const auto K2Schema = GetDefault<UEdGraphSchema_K2>();

	// Cache pins
	const auto ExecPin = GetExecPin();
	const auto NamePin = FindPinChecked(NamePinName);
	const auto SizePin = FindPinChecked(SizePinName);

	const auto OnClickPin = FindPinChecked(OnClickExecutionPinName);
	const auto ThenPin = GetThenPin();

	// Create a function call to ImGui::Button
	const auto Button = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	Button->FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UImGuiBlueprintLibrary, ImGui_Button), UImGuiBlueprintLibrary::StaticClass());
	Button->AllocateDefaultPins();

	const auto FuncNamePin = Button->FindPinChecked(TEXT("Text"));
	const auto FuncSizePin = Button->FindPinChecked(TEXT("Size"));
	const auto FuncReturnValuePin = Button->GetReturnValuePin();

	CompilerContext.MovePinLinksToIntermediate(*ExecPin, *Button->GetExecPin());
	CompilerContext.MovePinLinksToIntermediate(*NamePin, *FuncNamePin);
	CompilerContext.MovePinLinksToIntermediate(*SizePin, *FuncSizePin);

	// Create a sequence node
	const auto Sequence = CompilerContext.SpawnIntermediateNode<UK2Node_ExecutionSequence>(this, SourceGraph);
	Sequence->AllocateDefaultPins();

	const auto SequenceExec = Sequence->GetExecPin();
	const auto SequenceOne = Sequence->GetThenPinGivenIndex(0);
	const auto SequenceTwo = Sequence->GetThenPinGivenIndex(1);

	// Connect ImGui::Button output pin to sequence
	Button->GetThenPin()->MakeLinkTo(SequenceExec);

	// Create branch
	const auto Branch = CompilerContext.SpawnIntermediateNode<UK2Node_IfThenElse>(this, SourceGraph);
	Branch->AllocateDefaultPins();

	SequenceOne->MakeLinkTo(Branch->GetExecPin());
	Branch->GetConditionPin()->MakeLinkTo(FuncReturnValuePin);

	CompilerContext.MovePinLinksToIntermediate(*OnClickPin, *Branch->GetThenPin());

	// Connect second output Sequence Pin to out
	CompilerContext.MovePinLinksToIntermediate(*ThenPin, *SequenceTwo);

	BreakAllNodeLinks();
}

FText UK2Node_ImGuiButton::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("NodeTitle", "ImGui Button");
}

FText UK2Node_ImGuiButton::GetTooltipText() const
{
	return LOCTEXT("ToolTipText", "Button");
}

#undef LOCTEXT_NAMESPACE
