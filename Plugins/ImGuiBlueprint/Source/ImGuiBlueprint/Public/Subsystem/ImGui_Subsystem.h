// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "O_ImGuiDebugger.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "ImGui_Subsystem.generated.h"


UCLASS()
class IMGUIBLUEPRINT_API UImGui_Subsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	
	UPROPERTY()
	TArray<UO_ImGuiDebugger*> Debuggers;

	/**Adds a debugger and enable its tick event.
	 * Returns true if it was successful.*/
	UFUNCTION(Category="ImGui Subsystem", BlueprintCallable)
	bool AddDebugger(UO_ImGuiDebugger* Debugger);

	UFUNCTION(Category="ImGui Subsystem", BlueprintCallable)
	bool RemoveDebugger(UO_ImGuiDebugger* Debugger);

	UFUNCTION(Category="ImGui Subsystem", BlueprintCallable)
	bool RemoveDebuggerByIdentifier(FName Identifier);

	int32 IDCounter = 0;
	
	char* GetID();

	/**Many ImGui widgets need an ID and this is mostly automated.
	 * Although, at some point you need to reset it.
	 * This should be at the beginning of your tick event, so
	 * after the UI has been made, all the widgets can get the
	 * same ID again.*/
	UFUNCTION(Category="ImGui Subsystem", BlueprintCallable)
	void ResetID() { IDCounter = 0; }
};
