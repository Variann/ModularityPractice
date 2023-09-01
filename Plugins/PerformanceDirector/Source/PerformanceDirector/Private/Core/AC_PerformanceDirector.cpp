// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Core/AC_PerformanceDirector.h"


// Sets default values for this component's properties
UAC_PerformanceDirector::UAC_PerformanceDirector()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAC_PerformanceDirector::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAC_PerformanceDirector::TickComponent(float DeltaTime, ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

