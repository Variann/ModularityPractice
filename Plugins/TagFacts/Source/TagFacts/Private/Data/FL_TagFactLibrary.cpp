// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/FL_TagFactLibrary.h"

#include "Kismet/GameplayStatics.h"

bool UFL_TagFactLibrary::CompareFact(FS_Fact Fact, int32 Value, TEnumAsByte<EFactComparator> Comparator)
{
	switch (Comparator)
	{
	case IsTrue:
		return Fact.Value > 0;

	case IsFactEqualTo:
		return Fact.Value == Value;

	case IsFactGreaterThan:
		return Fact.Value > Value;
	
	case IsFactLessThan:
		return Fact.Value < Value;

	default:
		return false;
	}
}
