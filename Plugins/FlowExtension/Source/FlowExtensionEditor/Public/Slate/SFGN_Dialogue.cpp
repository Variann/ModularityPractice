#include "SFGN_Dialogue.h"

#include "EditorStyleSet.h"
#include "FlowEditorStyle.h"
#include "GraphEditorSettings.h"
#include "SCommentBubble.h"
#include "SLevelOfDetailBranchNode.h"
#include "TutorialMetaData.h"
#include "Materials/MaterialInstance.h"

#include "Dialogue/FN_DialogueBase.h"
#include "Nodes/FGN_Dialogue.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

#define LOCTEXT_NAMESPACE "PGFlowEditor"

void SFGN_Dialogue::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	//	     ______________________
	//	    |      TITLE AREA      |
	//	    +-------+------+-------+
	//	    | (>) L |      | R (>) |
	//	    | (>) E |      | I (>) |
	//	    | (>) F |      | G (>) |
	//	    | (>) T |      | H (>) |
	//	    |       |      | T (>) |
	//	    |_______|______|_______|
	//
	TSharedPtr<SVerticalBox> MainVerticalBox;
	SetupErrorReporting();

	const TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);

	// Get node icon
	IconColor = FLinearColor::White;
	const FSlateBrush* IconBrush = nullptr;
	if (GraphNode && GraphNode->ShowPaletteIconOnNode())
	{
		IconBrush = GraphNode->GetIconAndTint(IconColor).GetOptionalIcon();
	}

	FText NodeTitleText = LOCTEXT("InvalidSpeaker", "Invalid Speaker!");
	FLinearColor SpeakerColor;
	FLinearColor PronounColor;
	if (UFlowNode* FlowNode = FlowGraphNode->GetFlowNode())
	{
		UFN_DialogueBase* DialogueNode = Cast<UFN_DialogueBase>(FlowNode);
		{
			NodeTitleText = DialogueNode->GetSpeakerName();
			SpeakerColor = DialogueNode->GetSpeakerColor();
		}
	}

	const TSharedRef<SOverlay> DefaultTitleAreaWidget = SNew(SOverlay)
		+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Center)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
					.HAlign(HAlign_Fill)
					[
						SNew(SBorder)
							.BorderImage(FFlowEditorStyle::GetBrush("Flow.Node.Title"))
							// The extra margin on the right is for making the color spill stretch well past the node title
							.Padding(FMargin(10, 5, 30, 3))
							.BorderBackgroundColor(FLinearColor(SpeakerColor))
							[
								SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
									.VAlign(VAlign_Top)
									.Padding(FMargin(0.f, 0.f, 4.f, 0.f))
									.AutoWidth()
									[
										SNew(SImage)
											.Image(IconBrush)
											.ColorAndOpacity(FLinearColor(SpeakerColor))
									]
								+ SHorizontalBox::Slot()
									[
										SNew(SVerticalBox)
										+ SVerticalBox::Slot()
											.AutoHeight()
											[
												// CreateTitleWidget(NodeTitle)
											SAssignNew(InlineEditableText, SInlineEditableTextBlock)
											.Style(FAppStyle::Get(), "Graph.Node.NodeTitleInlineEditableText")
											.Text(NodeTitleText)
											.ColorAndOpacity(SpeakerTextColor)
											.IsReadOnly(true)
											]
										+ SVerticalBox::Slot()
											.AutoHeight()
											[
												NodeTitle.ToSharedRef()
											]
									]
							]
					]
			];

	SetDefaultTitleAreaWidget(DefaultTitleAreaWidget);

	const TSharedRef<SWidget> TitleAreaWidget = 
		SNew(SLevelOfDetailBranchNode)
			.UseLowDetailSlot(false)
			.LowDetail()
			[
				SNew(SBorder)
					.BorderImage(FFlowEditorStyle::GetBrush("Flow.Node.Title"))
					.Padding(FMargin(75.0f, 22.0f)) // Saving enough space for a 'typical' title so the transition isn't quite so abrupt
					.BorderBackgroundColor(FLinearColor(1, 1, 1, 0.5))
			]
			.HighDetail()
			[
				DefaultTitleAreaWidget
			];

	// Setup a meta tag for this node
	FGraphNodeMetaData TagMeta(TEXT("FlowGraphNode"));
	PopulateMetaTag(&TagMeta);

	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);

	const TSharedPtr<SVerticalBox> InnerVerticalBox = SNew(SVerticalBox)
	+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Top)
				.Padding(Settings->GetNonPinNodeBodyPadding())
				[
					TitleAreaWidget
				]
	+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			[
				CreateNodeContentArea()
			];

	const TSharedPtr<SWidget> EnabledStateWidget = GetEnabledStateWidget();
	if (EnabledStateWidget.IsValid())
	{
		InnerVerticalBox->AddSlot()
			.AutoHeight()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			.Padding(FMargin(2, 0))
			[
				EnabledStateWidget.ToSharedRef()
			];
	}

	InnerVerticalBox->AddSlot()
		.AutoHeight()
		.Padding(Settings->GetNonPinNodeBodyPadding())
		[
			ErrorReporting->AsWidget()
		];

	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
		SAssignNew(MainVerticalBox, SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SOverlay)
					.AddMetaData<FGraphNodeMetaData>(TagMeta)
					+ SOverlay::Slot()
						.Padding(Settings->GetNonPinNodeBodyPadding())
						[
							SNew(SImage)
							.Image(GetNodeBodyBrush())
							.ColorAndOpacity(FLinearColor(1, 1, 1, 1))
						]
					+ SOverlay::Slot()
						[
							InnerVerticalBox.ToSharedRef()
						]
			]
		];

	if (GraphNode && GraphNode->SupportsCommentBubble())
	{
		// Create comment bubble
		TSharedPtr<SCommentBubble> CommentBubble;
		const FSlateColor CommentColor = GetDefault<UGraphEditorSettings>()->DefaultCommentNodeTitleColor;

		SAssignNew(CommentBubble, SCommentBubble)
			.GraphNode(GraphNode)
			.Text(this, &SGraphNode::GetNodeComment)
			.OnTextCommitted(this, &SGraphNode::OnCommentTextCommitted)
			.OnToggled(this, &SGraphNode::OnCommentBubbleToggled)
			.ColorAndOpacity(CommentColor)
			.AllowPinning(true)
			.EnableTitleBarBubble(true)
			.EnableBubbleCtrls(true)
			.GraphLOD(this, &SGraphNode::GetCurrentLOD)
			.IsGraphNodeHovered(this, &SGraphNode::IsHovered);

		GetOrAddSlot(ENodeZone::TopCenter)
			.SlotOffset(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetOffset))
			.SlotSize(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetSize))
			.AllowScaling(TAttribute<bool>(CommentBubble.Get(), &SCommentBubble::IsScalingAllowed))
			.VAlign(VAlign_Top)
			[
				CommentBubble.ToSharedRef()
			];
	}

	CreateBelowWidgetControls(MainVerticalBox);
	CreatePinWidgets();
	CreateInputSideAddButton(LeftNodeBox);
	CreateOutputSideAddButton(RightNodeBox);
	// CreateBelowPinControls(InnerVerticalBox);
	CreateAdvancedViewArrow(InnerVerticalBox);
}

TSharedRef<SWidget> SFGN_Dialogue::CreateDialogueContentArea()
{
	if (UFlowNode* FlowNode = FlowGraphNode->GetFlowNode())
	{
		UFN_DialogueBase* DialogueNode = Cast<UFN_DialogueBase>(FlowNode);
		if(DialogueNode)
		{
			bool UseImage = false;
			Brush.ImageSize = FVector2D(64,64);
	
			if(UTexture2D* Portrait = DialogueNode->GetSpeakerPortrait())
			{
				UseImage = true;
				Brush.SetResourceObject(Portrait);	
			}
	
			FText DialogueText = DialogueNode->GetReadableDialogueString();
			if(UseImage)
			{
				return SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.HAlign(HAlign_Left)
						.VAlign(VAlign_Top)
						.AutoWidth()
						.Padding(0.f)
							[
								SNew(SImage)
								.Image(&Brush)
							]
						
						+ SHorizontalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Center)
						.FillWidth(1.0f)
						.Padding(10.f)
							[
								SNew(STextBlock)
								.Text(DialogueText)
								.WrapTextAt(250.f)
							];
			}
			else
			{
				return SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.VAlign(VAlign_Center)
						.Padding(10.f)
							[
								SNew(STextBlock)
								.Text(DialogueText)
								.WrapTextAt(250.f)
							];
			}
		}
	}
	
	return SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.Padding(10.f)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("BrokenNode", "This node seems to be broken. Please recreate it!"))
						.WrapTextAt(370.f)
					];
}

TSharedRef<SWidget> SFGN_Dialogue::CreateNodeContentArea()
{
	return SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("NoBorder"))
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			.AutoWidth()
			[
				SAssignNew(LeftNodeBox, SVerticalBox)
			]
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Center)
			.FillWidth(1.0f)
			[
				CreateDialogueContentArea()
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Center)
			[
				SAssignNew(RightNodeBox, SVerticalBox)
			]
		];
}

void SFGN_Dialogue::CreateBelowPinControls(TSharedPtr<SVerticalBox> MainBox)
{
	if (UFlowNode* FlowNode = FlowGraphNode->GetFlowNode())
	{
		UFN_DialogueBase* DialogueNode = Cast<UFN_DialogueBase>(FlowNode);
		if(DialogueNode)
		{
			FText DialogueText = DialogueNode->GetReadableDialogueString();
			
			Brush.ImageSize = FVector2D(64,64);
	
			if(UTexture2D* Portrait = DialogueNode->GetSpeakerPortrait())
			{
				Brush.SetResourceObject(Portrait);
			}
	
			MainBox->AddSlot()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Bottom)
				.AutoWidth()
				.Padding(0.f)
					[
						SNew(SImage)
						.Image(&Brush)
					]
					
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				.FillWidth(1.0f)
				.Padding(10.f)
					[
						SNew(STextBlock)
						.Text(DialogueText)
						.WrapTextAt(250.f)
					]
			];
		}
	}
}

void SFGN_Dialogue::Construct(const FArguments& InArgs, UFlowGraphNode* InNode)
{
	SpeakerTextColor = FLinearColor(1.f, 1.f, 1.f, 1.f);
	PronounTextColor = FLinearColor(1.f, 1.f, 1.f, 1.f);
	
	GraphNode = InNode;
	FlowGraphNode = InNode;

	SetCursor(EMouseCursor::CardinalCross);
	UpdateGraphNode();
}

#undef LOCTEXT_NAMESPACE