// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Core/RelationsSubSystem.h"

#include "Core/RelationsAsyncFunctions.h"
#include "Kismet/KismetSystemLibrary.h"

bool URelationsSubSystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}