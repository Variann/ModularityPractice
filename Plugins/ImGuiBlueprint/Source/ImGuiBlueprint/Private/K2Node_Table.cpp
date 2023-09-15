// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "K2Node_Table.h"

#include "imgui.h"
#include "ImGuiBlueprintLibrary.h"

UK2Node_Table* UK2Node_Table::ImGui_Table(TArray<FString> Columns, int32 RowAmount, FVector2D Size, FString TableName)
{
	UK2Node_Table* NewAsyncObject = NewObject<UK2Node_Table>();
	NewAsyncObject->ColumnNames = Columns;
	NewAsyncObject->RowAmounts = RowAmount;
	NewAsyncObject->TableSize = Size;
	NewAsyncObject->TableID = TableName;
	return NewAsyncObject;
}

void UK2Node_Table::Activate()
{
	Super::Activate();
	ImGuiTableFlags flags =
		   ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_SortMulti
		   | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_NoBordersInBody
	       | ImGuiTableFlags_SizingFixedFit | ImGuiTreeNodeFlags_DefaultOpen;

	//Tables greedily take up all space without this flag, but will behave
	//oddly when it has this flag, but has a set size.
	//Only add the flag if sizing is set to 0.
	if(TableSize != FVector2D(0))
	{
		flags += ImGuiTableFlags_ScrollY;
	}
	ImGui::PushID(UImGuiBlueprintLibrary::GetID());
	if(ImGui::BeginTable( TCHAR_TO_UTF8(*TableID), ColumnNames.Num(), flags, ImVec2(TableSize.X, TableSize.Y)))
	{
		//Create the columns
		for(auto& CurrentColumn : ColumnNames)
		{
			ImGui::TableSetupColumn(TCHAR_TO_UTF8(*CurrentColumn), ImGuiTableColumnFlags_WidthStretch);
		}

		//You have to create columns first, then call this
		ImGui::TableHeadersRow();

		//Start creating the rows
		for(int32 RowIndex = 0; RowIndex < RowAmounts; RowIndex++)
		{
			ImGui::TableNextRow();
			//Iterate over all the columns and start filling them.
			for(int32 ColumnIndex = 0; ColumnIndex < ColumnNames.Num(); ColumnIndex++)
			{
				ImGui::TableSetColumnIndex(ColumnIndex);
				RowCreated.Broadcast(ColumnIndex, RowIndex);
			}
		}
		
		ImGui::EndTable();
		ImGui::PopID();
	}

	TableFinished.Broadcast();

	
}
