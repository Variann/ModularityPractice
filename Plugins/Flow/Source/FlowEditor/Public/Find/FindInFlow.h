// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "Containers/Array.h"
#include "Containers/BitArray.h"
#include "Containers/Set.h"
#include "Containers/SparseArray.h"
#include "Containers/UnrealString.h"
#include "Delegates/Delegate.h"
#include "HAL/PlatformCrt.h"
#include "Input/Reply.h"
#include "Internationalization/Text.h"
#include "Misc/Optional.h"
#include "Templates/SharedPointer.h"
#include "Templates/TypeHash.h"
#include "Templates/UnrealTemplate.h"
#include "Types/SlateEnums.h"
#include "UObject/WeakObjectPtr.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/STreeView.h"

class ITableRow;
class SWidget;
class UFlowGraphNode;
class UEdGraphNode;

/** Item that matched the search results */
class FFindInFlowResult
{
public: 
	/** Create a root (or only text) result */
	FFindInFlowResult(const FString& InValue);
	
	/** Create a flow node result */
	FFindInFlowResult(const FString& InValue, TSharedPtr<FFindInFlowResult>& InParent, UEdGraphNode* InNode, bool bInIsSubGraphNode = false);

	/** Called when user clicks on the search item */
	FReply OnClick(TWeakPtr<class FFlowAssetEditor> FlowAssetEditor,  TSharedPtr<FFindInFlowResult> Root);
	
	/** Called when user double clicks on the search item */
	FReply OnDoubleClick(TSharedPtr<FFindInFlowResult> Root) const;

	/** Create an icon to represent the result */
	TSharedRef<SWidget>	CreateIcon() const;

	/** Gets the description on flow node if any */
	FString GetDescriptionText() const;

	/** Gets the comment on this node if any */
	FString GetCommentText() const;

	/** Gets the node type */
	FString GetNodeTypeText() const;

	/** Gets the node tool tip */
	FText GetToolTipText() const;

	/** Any children listed under this flow node (decorators and services) */
	TArray< TSharedPtr<FFindInFlowResult> > Children;

	/** The string value for this result */
	FString Value;

	/** The graph node that this search result refers to */
	TWeakObjectPtr<UEdGraphNode> GraphNode;

	/** Search result parent */
	TWeakPtr<FFindInFlowResult> Parent;

	/** Whether this item is a subgraph node */
	bool bIsSubGraphNode = false;
};

/** Widget for searching for (Flow nodes) across focused FlowNodes */
class SFindInFlow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SFindInFlow){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TSharedPtr<class FFlowAssetEditor> InFlowAssetEditor);

	/** Focuses this widget's search box */
	void FocusForUse() const;

private:
	typedef TSharedPtr<FFindInFlowResult> FSearchResult;
	typedef STreeView<FSearchResult> STreeViewType;

	/** Called when user changes the text they are searching for */
	void OnSearchTextChanged(const FText& Text);

	/** Called when user commits text */
	void OnSearchTextCommitted(const FText& Text, ETextCommit::Type CommitType);

	/** Get the children of a row */
	void OnGetChildren(FSearchResult InItem, TArray<FSearchResult>& OutChildren);

	/** Called when user clicks on a new result */
	void OnTreeSelectionChanged(FSearchResult Item, ESelectInfo::Type SelectInfo);
	
	/* Called when user double clicks on a new result */
	void OnTreeSelectionDoubleClicked( FSearchResult Item );

	/** Called when whether find in sub graph changed */
	void OnFindInSubGraphStateChanged(ECheckBoxState CheckBoxState);

	/** Called when a new row is being generated */
	TSharedRef<ITableRow> OnGenerateRow(FSearchResult InItem, const TSharedRef<STableViewBase>& OwnerTable);

	/** Begins the search based on the SearchValue */
	void InitiateSearch();
	
	/** Find any results that contain all of the tokens */
	void MatchTokens(const TArray<FString>& Tokens);

	/** Find if child contains all of the tokens and add a result accordingly */
	static void MatchTokensInChild(const TArray<FString>& Tokens, UEdGraphNode* Child, FSearchResult ParentNode);
	
	/** Determines if a string matches the search tokens */
	static bool StringMatchesSearchTokens(const TArray<FString>& Tokens, const FString& ComparisonString);

private:
	/** Pointer back to the flow editor that owns us */
	TWeakPtr<class FFlowAssetEditor> FlowAssetEditorPtr;
	
	/** The tree view displays the results */
	TSharedPtr<STreeViewType> TreeView;

	/** The search text box */
	TSharedPtr<class SSearchBox> SearchTextField;
	
	/** This buffer stores the currently displayed results */
	TArray<FSearchResult> ItemsFound;

	/** we need to keep a handle on the root result, because it won't show up in the tree */
	FSearchResult RootSearchResult;

	/** The string to highlight in the results */
	FText HighlightText;

	/** The string to search for */
	FString	SearchValue;

	/** Using to control whether search in sub graph */
	bool bFindInSubGraph = false;
};
