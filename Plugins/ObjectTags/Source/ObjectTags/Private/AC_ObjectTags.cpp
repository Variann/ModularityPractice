// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "AC_ObjectTags.h"

#include "ObjectTags_Subsystem.h"


UAC_ObjectTags::UAC_ObjectTags()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAC_ObjectTags::BeginPlay()
{
	Super::BeginPlay();

	UObjectTags_Subsystem* ObjectTags_Subsystem = UObjectTags_Subsystem::Get();
	if(ObjectTags_Subsystem)
	{
		for(auto& CurrentTag : TagsToGrant)
		{
			ObjectTags_Subsystem->AddTagToObject(CurrentTag.Tag, GetOwner(), this, CurrentTag.Value, CurrentTag.Duration);
		}
	}

	/**Components being destroyed within the same callstack as BeginPlay can throw exceptions.
	 * Just delay until next tick to get around it.*/
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
	{
		DestroyComponent();
	});
}

