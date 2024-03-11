// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TagMetadata/Public/O_TagMetadata.h"
#include "TMD_UI_Text.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Tag UI Text")
class TAGMETADATA_API UTMD_UI_Text : public UO_TagMetadata
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText UI_Text;
};
