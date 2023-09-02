// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Core/FL_PerformanceDirector.h"

#include "Core/I_PerformanceDirector.h"
#include "Kismet/KismetSystemLibrary.h"

TEnumAsByte<EPerformanceImportance> UFL_PerformanceDirector::GetActorsImportance(AActor* Actor, bool EvaluateImportance)
{
	if(!IsValid(Actor))
	{
		return Unknown;
	}
	
	UAC_PerformanceDirector* PerformanceDirector;

	//Interface retrieval is faster. If that fails, go through the actors components.
	if(Actor->GetClass()->ImplementsInterface(UI_PerformanceDirector::StaticClass()))
	{
		PerformanceDirector = II_PerformanceDirector::Execute_GetPerformanceDirector(Actor);
		if(!IsValid(PerformanceDirector))
		{
			UKismetSystemLibrary::PrintString(Actor, "Actor implements interface, but did not return GetPerformanceDirector");
			return Normal;
		}
	}
	else
	{
		PerformanceDirector = Cast<UAC_PerformanceDirector>(Actor->GetComponentByClass(UAC_PerformanceDirector::StaticClass()));
		if(!IsValid(PerformanceDirector))
		{
			UKismetSystemLibrary::PrintString(Actor, "No PerformanceDirector component on actor, can't return importance.");
			return Normal;
		}
	}

	if(PerformanceDirector)
	{
		return PerformanceDirector->GetCurrentImportance(EvaluateImportance);
	}
	
	return Unknown;
}
