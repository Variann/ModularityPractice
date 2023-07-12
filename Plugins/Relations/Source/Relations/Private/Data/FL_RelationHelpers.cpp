// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Data/FL_RelationHelpers.h"

float UFL_RelationHelpers::GetValueAtLevel(FScalableFloat ScalableFloat, float Level, FString ContextString)
{
	return ScalableFloat.GetValueAtLevel(Level);
}

TMap<float, float> UFL_RelationHelpers::GetRuntimeCurveKeys(FRuntimeFloatCurve RuntimeCurveFloat)
{
	TMap<float, float> FoundLevels;
	FRuntimeFloatCurve Levels = RuntimeCurveFloat;

	if(!RuntimeCurveFloat.GetRichCurve())
	{
		return FoundLevels;
	}
	
	for(auto& CurrentKey : Levels.GetRichCurve()->Keys)
	{
		FoundLevels.Add(CurrentKey.Time, CurrentKey.Value);
	}

	return FoundLevels;
}
