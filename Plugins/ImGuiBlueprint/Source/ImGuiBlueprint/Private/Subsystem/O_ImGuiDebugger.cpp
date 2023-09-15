// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Subsystem/O_ImGuiDebugger.h"

#include "Kismet/GameplayStatics.h"
#include "Subsystem/ImGui_Subsystem.h"

void UO_ImGuiDebugger::PostInitProperties()
{
	UObject::PostInitProperties();

	if(GetOuter() && GetOuter()->GetWorld())
	{
		UImGui_Subsystem* GuiSubSystem = UGameplayStatics::GetPlayerController(GetOuter(), 0)->GetLocalPlayer()->GetSubsystem<UImGui_Subsystem>();
		if(GuiSubSystem)
		{
			GuiSubSystem->AddDebugger(this);
		}
	}
}

void UO_ImGuiDebugger::Tick(float DeltaTime)
{
	if(GetOuter() && GetOuter()->GetWorld() && TickEnabled)
	{
		EventTick(DeltaTime);
	}
}

bool UO_ImGuiDebugger::IsTickable() const
{
	return true;
}

TStatId UO_ImGuiDebugger::GetStatId() const
{
	return TStatId();
}
