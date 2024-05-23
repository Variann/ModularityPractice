#include "SFGN_Quest.h"
#include "Materials/MaterialInstance.h"
#include "Dialogue/Nodes/FN_DialogueBase.h"
#include "Nodes/FGN_Dialogue.h"
#include "Quest/DataAssets/DA_QuestChain.h"
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
	
	//Title
	TSharedPtr<STextBlock> Title;
	CenterContentArea->AddSlot()
	.VAlign(VAlign_Top)
	.AutoHeight()
	.Padding(0, 10, 0, 0)
	[
	SAssignNew(Title, STextBlock)
		.Font(FCoreStyle::GetDefaultFontStyle("Bold", 18))
		.Text(QuestInformation->QuestName)
	];

	//Quest text. Make sure the text wraps and doesn't expand the node too much.
	CenterContentArea->AddSlot()
	.Padding(0, 5)
	.VAlign(VAlign_Top)
	.AutoHeight()
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

	TSharedPtr<SVerticalBox> TasksVerticalBox;
	CenterContentArea->AddSlot()
	// .AutoHeight()
	.VAlign(VAlign_Top)
	.AutoHeight()
	.Padding(0, 0, 0, 10)
	[
		SAssignNew(TasksVerticalBox, SVerticalBox)
	];
	//Create the task bullet points
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
		
		
		TasksVerticalBox->AddSlot()
		.Padding(0, 3)
		.AutoHeight()
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
				.MaxDesiredWidth(300)
				[
					SNew(STextBlock)
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
					.AutoWrapText(true)
					.Text(TaskDisplayName)
				]
			]
		];
	}

	
	if(QuestInformation->QuestChains.IsValidIndex(0))
	{
		CenterContentArea->AddSlot()
			.FillHeight(1)
			.VAlign(VAlign_Bottom)
			.Padding(0, 3, 0, 0)
			[
			SNew(STextBlock)
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 8))
				.Text(FText::FromString("Chains:"))
			];
		for(auto& CurrentChain : QuestInformation->QuestChains)
		{
			UDA_QuestChain* LoadedChain = CurrentChain.LoadSynchronous();
			// CenterContentArea->AddSlot()
			// .FillHeight(1)
			// .VAlign(VAlign_Bottom)
			// .Padding(0, 3, 0, 0)
			// [
			// SNew(STextBlock)
			// 	.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
			// 	.Text(LoadedChain->ChainName)
			// ];
			CenterContentArea->AddSlot()
			.FillHeight(1)
			.VAlign(VAlign_Bottom)
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
					.MaxDesiredWidth(300)
					[
						SNew(STextBlock)
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
						.AutoWrapText(true)
						.Text(LoadedChain->ChainName)
					]
				]
			];
		}
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
		.VAlign(VAlign_Fill)
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

