// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/FL_TagFactLibrary.h"

bool UFL_TagFactLibrary::CompareFact(FS_Fact Fact, int32 Value, TEnumAsByte<EFactComparator> Comparator)
{
	switch (Comparator)
	{
	case IsTrue:
		if(Fact.Value > 0) { return true; } else { return false; }

	case IsFactEqualTo:
		if(Fact.Value == Value) { return true; } else { return false; }

	case IsFactGreaterThan:
		if(Fact.Value > Value) { return true; } else { return false; }
	
	case IsFactLessThan: if(Fact.Value < Value) { return true; } else { return false; }

	default:
		return false;
	}
}
