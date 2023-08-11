#pragma once


#include "Graph/Widgets/SFlowGraphNode.h"


class FLOWEXTENSIONEDITOR_API SFGN_Quest : public SFlowGraphNode
{
protected:

	virtual void UpdateGraphNode() override;
	virtual TSharedRef<SWidget> CreateNodeContentArea() override;

	TSharedPtr<SVerticalBox> CenterContentArea;
	
public:
	SLATE_BEGIN_ARGS(SFGN_Quest) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UFlowGraphNode* InNode);
};
