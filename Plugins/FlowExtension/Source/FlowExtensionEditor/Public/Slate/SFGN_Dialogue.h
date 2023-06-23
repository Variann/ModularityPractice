#pragma once


#include "Graph/Widgets/SFlowGraphNode.h"


class FLOWEXTENSIONEDITOR_API SFGN_Dialogue : public SFlowGraphNode
{
protected:
	
	virtual void UpdateGraphNode() override;
	
	virtual TSharedRef<SWidget> CreateDialogueContentArea();
	virtual TSharedRef<SWidget> CreateNodeContentArea() override;
	virtual void CreateBelowPinControls(TSharedPtr<SVerticalBox> MainBox) override;

public:
	SLATE_BEGIN_ARGS(SFGN_Dialogue) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UFlowGraphNode* InNode);


	/** Used to display the name of the node and allow renaming of the node */
	TSharedPtr<SVerticalBox> TitleBox;
	TSharedPtr<SHorizontalBox> TitleBoxH;
	
	FSlateBrush Brush;

	FSlateColor SpeakerTextColor;
	FSlateColor PronounTextColor;

	FSlateColor SpeakerBgColor;
	FSlateBrush SpeakerBgBrush;
	FSlateBrush SpeakerBgCornerBrush;
	
	FSlateColor PronounBgColor;
	FSlateBrush PronounBgBrush;
	FSlateBrush PronounBgCornerBrush;
};
