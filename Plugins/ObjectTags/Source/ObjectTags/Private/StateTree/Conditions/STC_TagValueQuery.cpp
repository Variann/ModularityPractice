// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "StateTree/Conditions/STC_TagValueQuery.h"

#include "ObjectTags_Subsystem.h"
#include "Kismet/KismetSystemLibrary.h"

bool USTC_TagValueQuery::TestCondition(FStateTreeExecutionContext& Context) const
{
	if(TagValueQuery.IsEmpty())
	{
		UKismetSystemLibrary::PrintString(this, "ObjectTagQuery condition had no object or no tag query");
		return false;
	}

	FObjectTag ObjectTag = UObjectTags_Subsystem::GetObjectTags(Object);

	int32 MatchedQueries = 0;
	for(auto& CurrentEntry : TagValueQuery)
	{
		if(!AllMustMatch && MatchedQueries > 1)
		{
			return true;
		}
		
		float* Value = ObjectTag.TagsAndValues.Find(CurrentEntry.Tag);
		if(Value)
		{
			switch (CurrentEntry.Comparison)
			{
			case Equals:
				{
					if(*Value == CurrentEntry.Value)
					{
						MatchedQueries++;
					}
					break;
				}
			case LessThan:
				{
					if(*Value < CurrentEntry.Value)
					{
						MatchedQueries++;
					}
					break;
				}
			case GreaterThan:
				{
					if(*Value > CurrentEntry.Value)
					{
						MatchedQueries++;
					}
					break;
				}
			default:
				{
					break;
				}
			}
		}
	}

	return AllMustMatch ? MatchedQueries == TagValueQuery.Num() : MatchedQueries > 0;
}
