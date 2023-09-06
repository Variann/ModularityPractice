// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "FL_ColorTags.h"

#include "Config/DS_ColorTagSettings.h"

FLinearColor UFL_ColorTags::GetColorAttribute(FGameplayTag Tag, bool& ColorFound)
{
	if(const UDS_ColorTagSettings* ColorAttributeSettings = GetDefault<UDS_ColorTagSettings>())
	{
		if(const FLinearColor* FoundColor = ColorAttributeSettings->ColorAttributes.Find(Tag))
		{
			ColorFound = true;
			return *FoundColor;
		}
	}

	ColorFound = false;
	return FLinearColor();
}
