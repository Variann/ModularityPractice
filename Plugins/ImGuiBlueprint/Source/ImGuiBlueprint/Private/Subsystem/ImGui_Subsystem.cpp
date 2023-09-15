// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Subsystem/ImGui_Subsystem.h"

bool UImGui_Subsystem::AddDebugger(UO_ImGuiDebugger* Debugger)
{
	if(!Debuggers.Contains(Debugger))
	{
		Debugger->TickEnabled = true;
		Debuggers.Add(Debugger);
		return true;
	}

	return false;
}

bool UImGui_Subsystem::RemoveDebugger(UO_ImGuiDebugger* Debugger)
{
	if(Debuggers.Contains(Debugger))
	{
		Debugger->TickEnabled = false;
		Debuggers.RemoveSingle(Debugger);
		return true;
	}

	return false;
}

bool UImGui_Subsystem::RemoveDebuggerByIdentifier(FName Identifier)
{
	for(auto& CurrentDebugger : Debuggers)
	{
		if(CurrentDebugger->Identifier == Identifier)
		{
			CurrentDebugger->TickEnabled = false;
			Debuggers.RemoveSingle(CurrentDebugger);
			return true;
		}
	}

	return false;
}

char* UImGui_Subsystem::GetID()
{
	IDCounter++;
	return TCHAR_TO_UTF8(*FString::FromInt(IDCounter));
}
