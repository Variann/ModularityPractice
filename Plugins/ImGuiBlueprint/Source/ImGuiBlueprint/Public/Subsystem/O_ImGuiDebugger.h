// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "O_ImGuiDebugger.generated.h"

/**
 * This object is created by ImGui_SubSystem and is meant to only create
 * ImGui widgets. This lets you debug common things among your game within one place,
 * without putting that tick code into actors.
 *
 * Use ConstructObjectFromClass so any exposed variables can be filled in easily.
 */
UCLASS(Abstract, Blueprintable, meta=(ShowWorldContextPin),
	meta=(ShowWorldContextPin, ContextMenuCategory = "Varian's Plugins", ContextMenuEntryName = "ImGui Debugger", ContextMenuPrefix = "IGD_"))
class IMGUIBLUEPRINT_API UO_ImGuiDebugger : public UObject, public FTickableGameObject
{
	GENERATED_BODY()

public:

	virtual void PostInitProperties() override;

	bool TickEnabled = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn))
	FName Identifier;

	//Tick and timer support
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void EventTick(float DeltaTime);
};
