// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "O_TagMetadata.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, meta=(ShowWorldContextPin), EditInlineNew, DefaultToInstanced, CollapseCategories,
	meta=(ShowWorldContextPin, ContextMenuCategory = "Varian's Plugins", ContextMenuEntryName = "Metadata", ContextMenuPrefix = "TMD_"))
class TAGMETADATA_API UO_TagMetadata : public UObject
{
	GENERATED_BODY()
};
