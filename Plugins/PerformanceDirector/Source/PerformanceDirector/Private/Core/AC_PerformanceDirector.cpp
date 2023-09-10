// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Core/AC_PerformanceDirector.h"

#include "Core/FL_PerformanceDirector.h"
#include "Core/I_PerformanceDirector.h"
#include "HAL/ThreadManager.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UAC_PerformanceDirector::UAC_PerformanceDirector()
{
	PrimaryComponentTick.bCanEverTick = false;
	bAutoActivate = false;
	
	//This component can be run on server or client,
	//but it's not meant to be replicated.
	SetIsReplicated(false);
}


// Called when the game starts
void UAC_PerformanceDirector::BeginPlay()
{
	Super::BeginPlay();
	
	if(UKismetSystemLibrary::IsDedicatedServer(this))
	{
		if(!RunOnDedicatedServer)
		{
			DestroyComponent();
			return;
		}
	}

	if(StartTrackingOnBeginPlay)
	{
		StartTracking();
	}
}

void UAC_PerformanceDirector::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	StopTracking(false);
}

void UAC_PerformanceDirector::StartTracking()
{
	if(!TimerHandle.IsValid())
	{
		GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle, this, &UAC_PerformanceDirector::StartTrackingTimer, UpdateInterval, true, 0);
	}
	else
	{
		UKismetSystemLibrary::PrintString(this, "Actor is already being tracked.");
	}
}

void UAC_PerformanceDirector::StopTracking(bool bResetImportance)
{
	if(bResetImportance)
	{
		ResetImportance();
	}
	
	GetOwner()->GetWorldTimerManager().ClearTimer(TimerHandle);
}

TEnumAsByte<EPerformanceImportance> UAC_PerformanceDirector::GetCurrentImportance(bool EvaluateImportance)
{	
	if(EvaluateImportance)
	{
		if(GetOwner()->GetClass()->ImplementsInterface(UI_PerformanceDirector::StaticClass()))
		{
			return II_PerformanceDirector::Execute_EvaluateImportance(GetOwner());
		}
		else
		{
			UKismetSystemLibrary::PrintString(this, "Tried to evaluate importance, but actor does not implement interface");
		}
	}

	return CurrentImportance;
}

void UAC_PerformanceDirector::SetImportance(TEnumAsByte<EPerformanceImportance> NewImportance)
{
	if(CurrentImportance != NewImportance)
	{
		const EPerformanceImportance OldImportance = CurrentImportance;
		CurrentImportance = NewImportance;

		II_PerformanceDirector::Execute_ImportanceUpdated(GetOwner(), OldImportance, NewImportance);
		ImportanceUpdated.Broadcast(OldImportance, NewImportance);
	}
}

void UAC_PerformanceDirector::ResetImportance()
{
	SetImportance(DefaultImportance);
}

void UAC_PerformanceDirector::SetUpdateInterval(float NewUpdateInterval, bool UpdateTracker)
{
	UpdateInterval = NewUpdateInterval;

	if(UpdateTracker)
	{
		if(UGameplayStatics::GetGameInstance(this))
		{
			StopTracking(false);
			StartTracking();
		}
	}
}

void UAC_PerformanceDirector::StartTrackingTimer()
{
	/**Dirty hack to have a "multithreaded timer". I'm sure some programmers
	 * with multithreading experience are screaming inside looking at this,
	 * but hey... It works!*/
	if(!bIsTracking)
	{
		bIsTracking = true;

		/**Evaluate if we want background or game thread.
		 * From my testing, all other ENamedThreads weren't worth
		 * using, some didn't seem to work and some put the task
		 * on the RHI thread. AnyThread was the only one who moved
		 * the task to an unused thread.*/
		ENamedThreads::Type NamedThread = ENamedThreads::AnyThread;
		if(Thread == GameThread)
		{
			NamedThread = ENamedThreads::GameThread;
		}
		else if(Thread == BackgroundThread)
		{
			NamedThread = ENamedThreads::AnyThread;
		}

		//Used for debugging tools, can be removed in packaged games.
		float CurrentTime = GetOwner()->GetGameTimeSinceCreation();

		//Async tasks seem to crash less if you use TWeakObjectPtr.
		TWeakObjectPtr<UAC_PerformanceDirector> Director = this;
		AsyncTask(NamedThread, [Director, CurrentTime]()
		{
			if(Director.Get())
			{
				TEnumAsByte<EPerformanceImportance> LatestImportance = UFL_PerformanceDirector::GetActorsImportance(Director.Get()->GetOwner(), true);
				
				uint32 ThreadId = FPlatformTLS::GetCurrentThreadId();
				Director->LastEvaluationThread = FThreadManager::Get().GetThreadName(ThreadId);
				AsyncTask(ENamedThreads::GameThread, [Director, LatestImportance, CurrentTime]()
				{
					if(Director.Get())
					{
						if(Director.Get()->CurrentImportance != LatestImportance)
						{
							Director.Get()->SetImportance(LatestImportance);
							
							//Used for debugging tools, can be removed in packaged games.
							Director->LastEvaluationTime = Director->GetOwner()->GetGameTimeSinceCreation() - CurrentTime;
						}
						Director.Get()->bIsTracking = false;
					}
				});
			}
		});
	}
}
