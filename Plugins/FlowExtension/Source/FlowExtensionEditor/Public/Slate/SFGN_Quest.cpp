﻿#include "SFGN_Quest.h"
#include "Materials/MaterialInstance.h"
#include "Dialogue/Nodes/FN_DialogueBase.h"
#include "Nodes/FGN_Dialogue.h"
#include "Quest/Nodes/FN_QuestBase.h"

#define LOCTEXT_NAMESPACE "PGFlowEditor"

void SFGN_Quest::UpdateGraphNode()
{
	SFlowGraphNode::UpdateGraphNode();

	UDA_Quest* QuestInformation = nullptr;
	
	if (UFlowNode* FlowNode = FlowGraphNode->GetFlowNode())
	{
		UFN_QuestBase* QuestNode = Cast<UFN_QuestBase>(FlowNode);
		{
			if(!QuestNode->QuestAsset.IsValid())
			{
				QuestNode->QuestAsset = QuestNode->QuestAsset.LoadSynchronous();
			}
			
			QuestInformation = QuestNode->QuestAsset.Get();
		}
	}

	//In case we haven't filled in QuestAsset yet,
	//just get the default object so we don't crash.
	if(!QuestInformation)
	{
		QuestInformation = Cast<UDA_Quest>(UDA_Quest::StaticClass()->GetDefaultObject());
	}

	TSharedPtr<STextBlock> Title;

	CenterContentArea->AddSlot()
	.VAlign(VAlign_Top)
	.Padding(0, 10, 0, 0)
	[
	SAssignNew(Title, STextBlock)
		.Font(FCoreStyle::GetDefaultFontStyle("Bold", 18))
		.Text(QuestInformation->QuestName)
	];

	CenterContentArea->AddSlot()
	.AutoHeight()
	.Padding(0, 10)
	.VAlign(VAlign_Top)
	[
		SNew(SBox)
		.VAlign(VAlign_Top)
		.MaxDesiredWidth(Title->GetDesiredSize().X)
		[
			SNew(STextBlock)
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
			.AutoWrapText(true)
			.Text(QuestInformation->QuestText)
		]
	];
	
	for(auto& CurrentTask : QuestInformation->Tasks)
	{
		FText TaskDisplayName;
		if(CurrentTask.TaskName.IsEmpty())
		{
			TaskDisplayName = FText::FromString("No task name!");
		}
		else
		{
			if(CurrentTask.IsOptional)
			{
				TaskDisplayName = FText::Format(LOCTEXT("QuestNodeCustomization", "{TaskName} / {AmountRequired} (Optional)"), CurrentTask.TaskName, CurrentTask.ProgressRequired);
			}
			else
			{
				TaskDisplayName = FText::Format(LOCTEXT("QuestNodeCustomization", "{TaskName} / {AmountRequired}"), CurrentTask.TaskName, CurrentTask.ProgressRequired);
			}
		}
		
		
		CenterContentArea->AddSlot()
		.AutoHeight()
		.Padding(0, 5)
		.VAlign(VAlign_Top)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			.Padding(0, 0, 7, 0)
			[
				SNew(SImage)
				.Image(FAppStyle::GetBrush(TEXT("Icons.BulletPoint")))
				.DesiredSizeOverride(FVector2D(12.f, 12.f))
			]
			+ SHorizontalBox::Slot()
			[
				SNew(SBox)
				.MaxDesiredWidth(Title->GetDesiredSize().X)
				[
					SNew(STextBlock)
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
					.AutoWrapText(true)
					.Text(TaskDisplayName)
				]
			]
		];
	}
}

TSharedRef<SWidget> SFGN_Quest::CreateNodeContentArea()
{
	return SNew(SBorder)
	.BorderImage(FAppStyle::GetBrush("NoBorder"))
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Fill)
	[
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.FillWidth(1.0f)
		[
			SAssignNew(LeftNodeBox, SVerticalBox)
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Top)
		[
			SAssignNew(CenterContentArea, SVerticalBox)
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Right)
		[
			SAssignNew(RightNodeBox, SVerticalBox)
		]
	];
}

void SFGN_Quest::Construct(const FArguments& InArgs, UFlowGraphNode* InNode)
{
	GraphNode = InNode;
	FlowGraphNode = InNode;

	SetCursor(EMouseCursor::CardinalCross);
	UpdateGraphNode();
}

#undef LOCTEXT_NAMESPACE

