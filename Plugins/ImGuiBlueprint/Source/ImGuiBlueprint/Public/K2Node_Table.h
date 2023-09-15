// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "K2Node_Table.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRowCreated, int32, ColumnIndex, int32, RowIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTableFinished);

UCLASS()
class IMGUIBLUEPRINT_API UK2Node_Table : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FRowCreated RowCreated;
	
	UPROPERTY(BlueprintAssignable)
	FTableFinished TableFinished;

	UPROPERTY()
	TArray<FString> ColumnNames;

	UPROPERTY()
	int32 RowAmounts;

	UPROPERTY()
	FVector2D TableSize;

	/**Tables seem to bug out if all of them have the same ID.
	 * The resizing will break, so each table needs a unique ID*/
	UPROPERTY()
	FString TableID;

public:

	UFUNCTION(Category="ImGui|Menus", BlueprintCallable, meta=(BlueprintInternalUseOnly="true"))
	static UK2Node_Table* ImGui_Table(TArray<FString> Columns, int32 RowAmount, FVector2D Size, FString TableName);

	virtual void Activate() override;
};
