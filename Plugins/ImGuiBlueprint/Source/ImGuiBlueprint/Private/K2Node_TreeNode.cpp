// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_TreeNode.h"

UK2Node_TreeNode* UK2Node_TreeNode::ImGui_TreeNode(FString Label)
{
	UK2Node_TreeNode* NewAsyncObject = NewObject<UK2Node_TreeNode>();
	NewAsyncObject->TreeLabel = Label;
	return NewAsyncObject;
}

void UK2Node_TreeNode::Activate()
{
	Super::Activate();
	if(ImGui::TreeNode(TCHAR_TO_UTF8(*TreeLabel)))
	{
		TreeCreated.Broadcast();
		ImGui::TreePop();
	}
	Finish.Broadcast();
	RemoveFromRoot();
}

