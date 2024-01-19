// Copyright (C) Varian Daemon. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "O_TagMetadata.h"
#include "TMD_WorldLocation.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "World Location")
class TAGMETADATA_API UTMD_WorldLocation : public UO_TagMetadata
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UWorld> World;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector Location;
};
