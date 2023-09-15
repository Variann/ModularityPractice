// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_TabBarMenu.h"

#include "ImGuiBlueprintLibrary.h"

UK2Node_TabBarMenu* UK2Node_TabBarMenu::ImGui_TabBarMenu(TArray<FString> Tabs)
{
	UK2Node_TabBarMenu* NewAsyncObject = NewObject<UK2Node_TabBarMenu>();
	NewAsyncObject->TabNames = Tabs;
	return NewAsyncObject;
}

void UK2Node_TabBarMenu::Activate()
{
	Super::Activate();

	ImGui::PushID(UImGuiBlueprintLibrary::GetID());
	if (ImGui::BeginTabBar("TabBar"))
	{
		for (int n = 0; n < TabNames.Num(); n++)
		{
			if (ImGui::BeginTabItem(TCHAR_TO_UTF8(*TabNames[n])))
			{
				TabAdded.Broadcast(n);
				ImGui::EndTabItem();
			}
		}
			
		ImGui::EndTabBar();
		ImGui::PopID();
	}

	Finish.Broadcast();
	RemoveFromRoot();
}
