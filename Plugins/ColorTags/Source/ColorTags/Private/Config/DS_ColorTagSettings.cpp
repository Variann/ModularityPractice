// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Config/DS_ColorTagSettings.h"

UDS_ColorTagSettings::UDS_ColorTagSettings()
{
	SectionName = "Color Tag Settings";
}

FLinearColor UDS_ColorTagSettings::GetColorByTag(FGameplayTag Tag, bool& ColorFound)
{
	if(const UDS_ColorTagSettings* ColorAttributeSettings = GetDefault<UDS_ColorTagSettings>())
	{
		if(const FLinearColor* FoundColor = ColorAttributeSettings->ColorTags.Find(Tag))
		{
			ColorFound = true;
			return *FoundColor;
		}
	}

	ColorFound = false;
	return FLinearColor();
}
