// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Data/DA_RelationData.h"

#include "Data/FL_RelationHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

FS_RelationStatus UDA_RelationData::GetCurrentStatus(const int32 Level)
{
	FS_RelationStatus FoundStatus;
	
	for(auto& CurrentStatus : RelationStatuses)
	{
		if(UKismetMathLibrary::InRange_IntInt(Level, CurrentStatus.MinLevel, CurrentStatus.MaxLevel, true, false))
		{
			return CurrentStatus;
		}
	}
	
	return FoundStatus;
}

int32 UDA_RelationData::GetLevelFromExperience(const float Experience)
{
	TArray<float> Keys;
	TArray<float> Values;
	UFL_RelationHelpers::GetRuntimeCurveKeys(ExperienceAndLevelCurve).GenerateKeyArray(Keys);
	UFL_RelationHelpers::GetRuntimeCurveKeys(ExperienceAndLevelCurve).GenerateValueArray(Values);

	if(!Values.IsValidIndex(0) && !Values.IsValidIndex(1))
	{
		UKismetSystemLibrary::PrintString(this, TEXT("Level curve must have at least 2 keys"));
		return 0;
	}
	
	for(int32 CurrentLevel = 0; CurrentLevel < Keys.Num(); CurrentLevel++)
	{
		if(!Values.IsValidIndex(CurrentLevel + 1))
		{
			if(Experience >= Values[CurrentLevel])
			{
				return Keys[CurrentLevel];
			}

			return 0;
		}
		
		const float MinRange = Values[CurrentLevel];
		const float MaxRange = Values[CurrentLevel + 1];
		
		if(UKismetMathLibrary::InRange_FloatFloat(Experience, MinRange, MaxRange, true, false))
		{
			return Keys[CurrentLevel];
		}
		
	}

	return 0;
}
