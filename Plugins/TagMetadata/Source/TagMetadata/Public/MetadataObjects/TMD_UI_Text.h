// Copyright (C) Varian Daemon. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "TagMetadata/Public/O_TagMetadata.h"
#include "TMD_UI_Text.generated.h"

/**
 * Default example class for tag metadata.
 * Simply create a child of UO_TagMetadata, add the Edit
 * UPROPERTY specifier you want and set your UCLASS
 * display name for an easier-to-read name in the editor.
 */
UCLASS(DisplayName = "UI Text")
class TAGMETADATA_API UTMD_UI_Text : public UO_TagMetadata
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText UI_Text;
};
