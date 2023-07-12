// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CoreTagFactData.generated.h"

UENUM(BlueprintType)
enum EFactComparator
{
	IsTrue, //Is the value greater than 0
	IsFactGreaterThan,
	IsFactLessThan,
	IsFactEqualTo,
};

UENUM()
enum EFactType
{
	Value,
	Fact
};

USTRUCT(BlueprintType)
struct FS_Fact
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fact")
	FGameplayTag Tag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fact")
	int32 Value = 0;

	bool operator==(const FS_Fact& Argument) const
	{
		return Argument.Tag == Tag;
	}
};
FORCEINLINE uint32 GetTypeHash(const FS_Fact& Thing)
{
	uint32 Hash = FCrc::MemCrc32(&Thing, sizeof(FS_Fact));
	return Hash;
}


USTRUCT(BlueprintType)
struct FS_FactCheck
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fact")
	FS_Fact Fact;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fact")
	TEnumAsByte<EFactComparator> CheckToPerform = IsTrue;
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fact")
	TEnumAsByte<EFactType> Type;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fact", meta = (EditCondition="Type == EFactType::Fact", EditConditionHides))
	FS_Fact FactToCompare;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fact", meta = (EditCondition="Type == EFactType::Value", EditConditionHides))
	int32 ValueToCompare = 0;
};
