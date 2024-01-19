// Copyright (C) Varian Daemon. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Core/DA_CoreCharacter.h"
#include "TagMetadata/Public/O_TagMetadata.h"
#include "TMD_CharacterAssetReference.generated.h"


/**
 * 
 */
UCLASS(DisplayName = "Character Asset Reference")
class TAGMETADATA_API UTMD_CharacterAssetReference : public UO_TagMetadata
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UDA_CoreCharacter* CharacterAsset;
};
