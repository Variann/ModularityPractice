// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Dialogue/Objects/O_DialogueConditionBase.h"

bool UO_DialogueConditionBase::IsConditionMet_Implementation()
{
	return true;
}

UWorld* UO_DialogueConditionBase::GetWorld() const
{
	return GEngine->GetCurrentPlayWorld(nullptr);
}
