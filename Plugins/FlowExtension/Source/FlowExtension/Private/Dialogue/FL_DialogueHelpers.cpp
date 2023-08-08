// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Dialogue/FL_DialogueHelpers.h"

bool UFL_DialogueHelpers::DoesDialogueMeetCondition(FDialogueOption DialogueOption)
{
	if(DialogueOption.Conditions.IsEmpty())
	{
		//Dialogue has no conditions, just return true.
		return true;
	}

	//Figure out how many conditions we need
	const int32 ConditionsNeeded = DialogueOption.ConditionHandling == AllConditions ? DialogueOption.Conditions.Num() : 1;
	
	int32 ConditionsMet = 0;
	for(auto& CurrentCondition : DialogueOption.Conditions)
	{
		if(CurrentCondition->IsConditionMet())
		{
			ConditionsMet++;
			if(ConditionsMet == ConditionsNeeded)
			{
				return true;
			}
		}
	}

	return false;
}
