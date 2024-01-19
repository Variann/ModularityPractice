// Copyright (C) Varian Daemon. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "TagMetadata/Public/O_TagMetadata.h"
#include "TMD_ReportText.generated.h"

/**Text displayed in the report.
 * This is different from UI Text as in this provides
 * a longer contextual text.
 * For example: Report.CauseOfDeath.Stabbed.Knife will
 * have a UI text as "Knife" but in the report we want it
 * displayed as "Stabbed with knife"*/
UCLASS(DisplayName = "Report UI Text")
class TAGMETADATA_API UTMD_ReportText : public UO_TagMetadata
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText ReportText;

	/**Should we show the murderer section inside the report screen?*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool ShowMurderer = false;
};
