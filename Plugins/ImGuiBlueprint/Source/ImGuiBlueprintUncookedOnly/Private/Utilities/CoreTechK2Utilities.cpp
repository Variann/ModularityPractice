
#include "CoreTechK2Utilities.h"

// Credit to https://github.com/MagForceSeven/UE-K2-Nodes

// KismetCompiler
#include "KismetCompiler.h"

// BlueprintGraph
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_AddDelegate.h"

// Engine
#include "EdGraph/EdGraphPin.h"
#include "Kismet2/BlueprintEditorUtils.h"

// GraphEditor
#include "GraphEditorSettings.h"

void CoreTechK2Utilities::MovePinLinksOrCopyDefaults( FKismetCompilerContext &CompilerContext, UEdGraphPin *Source, UEdGraphPin *Dest )
{
	if (Source->LinkedTo.Num( ) > 0) // Move the pink links
	{
		CompilerContext.MovePinLinksToIntermediate( *Source, *Dest );
	}
	else // Copy the blueprint literal
	{
		Dest->DefaultObject = Source->DefaultObject;
		Dest->DefaultValue = Source->DefaultValue;
		Dest->DefaultTextValue = Source->DefaultTextValue;
	}
}

void CoreTechK2Utilities::RefreshAllowedConnections( const UK2Node *K2Node, UEdGraphPin *Pin )
{
	auto PinConnectionList = Pin->LinkedTo;
	Pin->BreakAllPinLinks( true );

	const auto K2Schema = GetDefault<UEdGraphSchema_K2>( );
	for (const auto Connection : PinConnectionList)
	{
		K2Schema->TryCreateConnection( Pin, Connection );
	}

	K2Node->GetGraph( )->NotifyGraphChanged( );

	FBlueprintEditorUtils::MarkBlueprintAsModified( K2Node->GetBlueprint( ) );
}

void CoreTechK2Utilities::SetPinToolTip( UEdGraphPin *Pin, const FText &PinDescription )
{
	Pin->PinToolTip.Empty( );

	if (const auto K2Schema = Cast< UEdGraphSchema_K2 >( Pin->GetOwningNode( )->GetSchema( ) ))
		Pin->PinToolTip += K2Schema->GetPinDisplayName( Pin ).ToString( );

	if (!PinDescription.IsEmpty( ))
	{
		Pin->PinToolTip += TEXT( "\n" );
		Pin->PinToolTip += PinDescription.ToString( );
	}

	Pin->PinToolTip += FString( TEXT( "\n\n" ) ) + UEdGraphSchema_K2::TypeToText( Pin->PinType ).ToString( );
}

void CoreTechK2Utilities::DefaultGetMenuActions( const UK2Node *Node, FBlueprintActionDatabaseRegistrar& ActionRegistrar )
{
	// actions get registered under specific object-keys; the idea is that 
	// actions might have to be updated (or deleted) if their object-key is  
	// mutated (or removed)... here we use the node's class (so if the node 
	// type disappears, then the action should go with it)
	const auto ActionKey = Node->GetClass( );
	// to keep from needlessly instantiating a UBlueprintNodeSpawner, first   
	// check to make sure that the registrar is looking for actions of this type
	// (could be regenerating actions for a specific asset, and therefore the 
	// registrar would only accept actions corresponding to that asset)
	if (ActionRegistrar.IsOpenForRegistration( ActionKey ))
	{
		const auto NodeSpawner = UBlueprintNodeSpawner::Create( ActionKey );
		check( NodeSpawner != nullptr );

		ActionRegistrar.AddBlueprintAction( ActionKey, NodeSpawner );
	}
}

FSlateIcon CoreTechK2Utilities::GetFunctionIconAndTint( FLinearColor& OutColor )
{
	OutColor = GetDefault< UGraphEditorSettings >( )->FunctionCallNodeTitleColor;
	static FSlateIcon Icon( "EditorStyle", "Kismet.AllClasses.FunctionIcon" );
	return Icon;
}

FSlateIcon CoreTechK2Utilities::GetPureFunctionIconAndTint( FLinearColor& OutColor )
{
	OutColor = GetDefault< UGraphEditorSettings >( )->PureFunctionCallNodeTitleColor;
	static FSlateIcon Icon( "EditorStyle", "Kismet.AllClasses.FunctionIcon" );
	return Icon;
}