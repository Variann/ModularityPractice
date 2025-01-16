// Copyright (C) Varian Daemon. All Rights Reserved


#include <Quest/Objects/Parents/O_TaskRequirementBase.h>

bool UO_TaskRequirementBase::IsConditionMet_Implementation()
{
	return true;
}

class UWorld* UO_TaskRequirementBase::GetWorld() const
{
	if(!GEngine->GameViewport)
	{
		return nullptr;
	}
	
	return GEngine->GameViewport->GetWorld();
}
