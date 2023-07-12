// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/DA_RelationData.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RelationsSubSystem.generated.h"



UCLASS()
class RELATIONS_API URelationsSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UPROPERTY(SaveGame, BlueprintReadOnly)
	TSet<FS_Relationship> Relationships;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	  //********************************************************//
	 // Most functions can be found in RelationsAsyncFunctions //
	//********************************************************//
};
