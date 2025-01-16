// Copyright (C) Varian Daemon. All Rights Reserved


#include "Quest/Objects/Parents/O_TaskFailConditionBase.h"

bool UO_TaskFailConditionBase::IsTaskFailed_Implementation()
{
	return true;
}

class UWorld* UO_TaskFailConditionBase::GetWorld() const
{
	if(!GEngine->GameViewport)
	{
		return nullptr;
	}
	
	return GEngine->GameViewport->GetWorld();
}
