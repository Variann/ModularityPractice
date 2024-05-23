// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/QuestSubSystem.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Quest/FL_QuestHelpers.h"
#include "Quest/I_QuestUpdates.h"
#include "VisualLogger/VisualLoggerKismetLibrary.h"
#include "Math/Color.h"
#include "Quest/DataAssets/DA_QuestChain.h"


UQuestSubSystem::UQuestSubSystem()
{
#if WITH_EDITOR
	if(GEngine)
	{
		IConsoleManager::Get().RegisterConsoleCommand(
			TEXT("SetQuestState"),
			TEXT("Set the state of a quest. Requires the name of the quest asset (for example; QA_Tutorial_BasicMovement), "
		"then a state (Inactive, InProgress, Completed, Failed"),
			FConsoleCommandWithArgsDelegate::CreateStatic(&SetQuestState),
			ECVF_Default);
	}
#endif
}

UQuestSubSystem* UQuestSubSystem::Get()
{
	if(!GEngine->GameViewport) { return nullptr; }
	const UWorld* World = GEngine->GameViewport->GetWorld();
	if(!World) { return nullptr; }
	if(!UGameplayStatics::GetGameInstance(World)){ return nullptr; }
	if(UQuestSubSystem* SubSystem = UGameplayStatics::GetGameInstance(World)->GetSubsystem<UQuestSubSystem>())
	{
		return SubSystem;
	}

	return nullptr;
}

void UQuestSubSystem::AddListenerToQuest(const TSoftObjectPtr<UDA_Quest> Quest, UObject* Listener)
{
	UQuestSubSystem* QuestSubSystem = UQuestSubSystem::Get();
	if(!QuestSubSystem)
	{
		return;
	}
	
	FQuestWrapper* QuestWrapper = QuestSubSystem->Quests.Find(Quest);
	if(QuestWrapper)
	{
		QuestWrapper->Listeners.Add(Listener);
	}
}

bool UQuestSubSystem::AcceptQuest(TSoftObjectPtr<UDA_Quest> Quest, bool ForceAccept)
{
	if(Quest.IsNull())
	{
		return false;
	}
	
	UQuestSubSystem* QuestSubSystem = UQuestSubSystem::Get();
	if(!QuestSubSystem)
	{
		return false;
	}
	
	if(!CanAcceptQuest(Quest) && !ForceAccept)
	{
		return false;
	}

	//Player can accept the quest, start accepting it.

	UDA_Quest* LoadedQuest = Quest.LoadSynchronous();

	//Wrap the quest into a struct that is more easily
	//serialized and manageable.
	FQuestWrapper QuestWrapper = UFL_QuestHelpers::WrapQuest(LoadedQuest);

	//Wrap the tasks into a struct that is more easily
	//serialized and manageable.
	for(const auto& CurrentTask : Quest->Tasks)
	{
		FTaskWrapper TaskWrapper = UFL_QuestHelpers::WrapTask(LoadedQuest, CurrentTask);
		
		QuestWrapper.Tasks.Add(TaskWrapper);
	}

	QuestSubSystem->Quests.Add(Quest, QuestWrapper);
	for(auto& CurrentChain : Quest->QuestChains)
	{
		if(!QuestSubSystem->QuestChains.Contains(CurrentChain))
		{
			QuestSubSystem->QuestChains.Add(CurrentChain);
			QuestSubSystem->QuestChainStarted.Broadcast(CurrentChain);
		}
	}
	
	QuestSubSystem->QuestStateUpdated.Broadcast(QuestWrapper, InProgress);

	#if ENABLE_VISUAL_LOG
	
	UE_VLOG_LOCATION(QuestSubSystem, TEXT("Quest System %s"), Verbose, UGameplayStatics::GetPlayerPawn(QuestSubSystem, 0)->GetActorLocation(),
		10, FColor::White, TEXT("Accepted Quest: %s"), *Quest.GetAssetName());

	#endif
	
	return true;
}

bool UQuestSubSystem::CanAcceptQuest(TSoftObjectPtr<UDA_Quest> Quest)
{
	if(GetQuestState(Quest) != Inactive)
	{
		return false;
	}

	if(!HasCompletedRequiredQuests(Quest))
	{
		return false;
	}

	return true;
}

void UQuestSubSystem::CompleteQuest(TSoftObjectPtr<UDA_Quest> Quest, bool SkipCompletionCheck, bool AutoAcceptQuest)
{
	if(Quest.IsNull())
	{
		return;
	}
	
	UQuestSubSystem* QuestSubSystem = UQuestSubSystem::Get();
	if(!QuestSubSystem)
	{
		return;
	}
	
	FQuestWrapper* QuestWrapper = QuestSubSystem->Quests.Find(Quest);
	if(!QuestWrapper)
	{
		if(AutoAcceptQuest)
		{
			AcceptQuest(Quest, true);
			QuestWrapper = QuestSubSystem->Quests.Find(Quest);
		}
		else
		{
			return;
		}
	}
	
	if(!SkipCompletionCheck)
	{
		if(!CanCompleteQuest(*QuestWrapper))
		{
			return;
		}
	}

	QuestWrapper->State = Completed;

	//Safety check, mostly happens when a quest is force completed through a dev tool.
	for(auto& CurrentQuest : GetRequiredQuestsForQuest(Quest))
	{
		if(GetQuestState(CurrentQuest) != Completed && CurrentQuest != Quest)
		{
			CompleteQuest(CurrentQuest, true);
		}
	}

	/**If we are forcing this quest completion through the editor/dev tools,
	 * then we need to forcibly complete non-optional tasks as well.*/
	for(auto& CurrentTask : QuestWrapper->Tasks)
	{
		if(!QuestWrapper->QuestAsset->IsTaskOptional(CurrentTask.TaskID))
		{
			CompleteTask(CurrentTask.TaskID, nullptr);
		}
		else
		{
			FailTask(CurrentTask.TaskID, false);
		}
	}
	
	QuestSubSystem->QuestStateUpdated.Broadcast(*QuestWrapper, Completed);
		
#if ENABLE_VISUAL_LOG
		
	UE_VLOG_LOCATION(QuestSubSystem, TEXT("Quest System %s"), Verbose, UGameplayStatics::GetPlayerPawn(QuestSubSystem, 0)->GetActorLocation(),
	10, FColor::White, TEXT("Completed quest: %s"), *Quest->GetName());
		
#endif

	//Notify listeners
	for(const auto& CurrentListener : QuestWrapper->Listeners)
	{
		if(IsValid(CurrentListener))
		{
			if(UKismetSystemLibrary::DoesImplementInterface(CurrentListener, UI_QuestUpdates::StaticClass()))
			{
				II_QuestUpdates::Execute_QuestStateUpdated(CurrentListener, *QuestWrapper, Completed);
			}
		}
	}

	//Quest is completed, no need to keep the listeners array active
	QuestWrapper->Listeners.Empty();
}

bool UQuestSubSystem::CanCompleteQuest(FQuestWrapper Quest)
{
	UQuestSubSystem* QuestSubSystem = UQuestSubSystem::Get();
	if(!QuestSubSystem)
	{
		return false;
	}
	
	if(!Quest.QuestAsset.IsValid())
	{
		UKismetSystemLibrary::PrintString(QuestSubSystem, TEXT("Invalid Quest Asset for quest"));
		return false;
	}
	
	if(Quest.State != InProgress)
	{
		return false;
	}

	if(Quest.Tasks.IsEmpty())
	{
		UKismetSystemLibrary::PrintString(QuestSubSystem, TEXT("Quest has no tasks, can't complete"));
		return false;
	}

	for(auto& CurrentTask : Quest.Tasks)
	{
		if(Quest.QuestAsset->IsTaskOptional(CurrentTask.TaskID))
		{
			continue;
		}
		
		if(CurrentTask.State == InProgress || CurrentTask.State == Inactive)
		{
			return false;
		}
	}

	return true;
}

bool UQuestSubSystem::HasCompletedQuest(TSoftObjectPtr<UDA_Quest> Quest)
{
	UQuestSubSystem* QuestSubSystem = UQuestSubSystem::Get();
	if(!QuestSubSystem)
	{
		return false;
	}
	
	if(FQuestWrapper* QuestWrapper = QuestSubSystem->Quests.Find(Quest))
	{
		return QuestWrapper->State == Completed;
	}

	return false;
}

bool UQuestSubSystem::HasFailedQuest(TSoftObjectPtr<UDA_Quest> Quest)
{
	UQuestSubSystem* QuestSubSystem = UQuestSubSystem::Get();
	if(!QuestSubSystem)
	{
		return false;
	}
	
	if(FQuestWrapper* QuestWrapper = QuestSubSystem->Quests.Find(Quest))
	{
		return QuestWrapper->State == Failed;
	}

	return false;
}

TEnumAsByte<EQuestState> UQuestSubSystem::GetQuestState(TSoftObjectPtr<UDA_Quest> Quest)
{
	UQuestSubSystem* QuestSubSystem = UQuestSubSystem::Get();
	if(!QuestSubSystem)
	{
		return Inactive;
	}
	
	if(FQuestWrapper* QuestWrapper = QuestSubSystem->Quests.Find(Quest))
	{
		return QuestWrapper->State;
	}

	return Inactive;
}

bool UQuestSubSystem::DropQuest(TSoftObjectPtr<UDA_Quest> Quest)
{
	UQuestSubSystem* QuestSubSystem = UQuestSubSystem::Get();
	if(!QuestSubSystem)
	{
		return false;
	}
	
	FQuestWrapper* QuestWrapper = QuestSubSystem->Quests.Find(Quest);
	if(!QuestWrapper)
	{
		return false;
	}
	
	//Drop the tasks
	for(const auto& CurrentTask : QuestWrapper->Tasks)
	{
		QuestSubSystem->TaskDropped.Broadcast(CurrentTask);
			
		for(auto& CurrentListener : CurrentTask.Listeners)
		{
			if(IsValid(CurrentListener))
			{
				if(UKismetSystemLibrary::DoesImplementInterface(CurrentListener, UI_QuestUpdates::StaticClass()))
				{
					II_QuestUpdates::Execute_TaskDropped(CurrentListener, CurrentTask);
				}
			}
		}
	}

	QuestSubSystem->QuestDropped.Broadcast(*QuestWrapper);

	//Announce the quest being dropped
	for(const auto& CurrentListener : QuestWrapper->Listeners)
	{
		if(IsValid(CurrentListener))
		{
			if(UKismetSystemLibrary::DoesImplementInterface(CurrentListener, UI_QuestUpdates::StaticClass()))
			{
				II_QuestUpdates::Execute_QuestDropped(CurrentListener, *QuestWrapper);
			}
		}
	}

	QuestSubSystem->Quests.Remove(Quest);

	#if ENABLE_VISUAL_LOG
	UE_VLOG_LOCATION(QuestSubSystem, TEXT("Quest System %s"), Verbose, UGameplayStatics::GetPlayerPawn(QuestSubSystem, 0)->GetActorLocation(),
	10, FColor::White, TEXT("Dropped quest: %s"), *Quest->GetName());
	#endif

	return true;
}

bool UQuestSubSystem::FailQuest(TSoftObjectPtr<UDA_Quest> Quest, const bool FailTasks)
{
	UQuestSubSystem* QuestSubSystem = UQuestSubSystem::Get();
	if(!QuestSubSystem)
	{
		return false;
	}
	
	FQuestWrapper* QuestWrapper = QuestSubSystem->Quests.Find(Quest);
	if(!QuestWrapper)
	{
		return false;
	}
	
	if(QuestWrapper->State != InProgress)
	{
		return false;
	}

	QuestWrapper->State = Failed;

	#if ENABLE_VISUAL_LOG
	UE_VLOG_LOCATION(QuestSubSystem, TEXT("Quest System %s"), Verbose, UGameplayStatics::GetPlayerPawn(QuestSubSystem, 0)->GetActorLocation(),
	10, FColor::White, TEXT("Failed quest: %s"),
	*Quest->GetName());
	#endif
		
	if(FailTasks)
	{
		//Fail the tasks
		for(auto& CurrentTask : QuestWrapper->Tasks)
		{
			CurrentTask.State = Failed;
			QuestSubSystem->TaskFailed.Broadcast(CurrentTask);
			
			for(auto& CurrentListener : CurrentTask.Listeners)
			{
				if(IsValid(CurrentListener))
				{
					if(UKismetSystemLibrary::DoesImplementInterface(CurrentListener, UI_QuestUpdates::StaticClass()))
					{
						II_QuestUpdates::Execute_TaskFailed(CurrentListener, CurrentTask);
					}
				}
			}
		}
	}

	QuestSubSystem->QuestFailed.Broadcast(*QuestWrapper);

	//Announce the quest failure
	for(const auto& CurrentListener : QuestWrapper->Listeners)
	{
		if(IsValid(CurrentListener))
		{
			if(UKismetSystemLibrary::DoesImplementInterface(CurrentListener, UI_QuestUpdates::StaticClass()))
			{
				II_QuestUpdates::Execute_QuestFailed(CurrentListener, *QuestWrapper);
			}
		}
	}

	return true;
}

TArray<FQuestWrapper> UQuestSubSystem::GetQuestsWithState(TEnumAsByte<EQuestState> State)
{
	TArray<FQuestWrapper> FoundQuests;

	UQuestSubSystem* QuestSubSystem = UQuestSubSystem::Get();
	if(!QuestSubSystem)
	{
		return FoundQuests;
	}

	for(auto& CurrentQuest : QuestSubSystem->Quests)
	{
		if(CurrentQuest.Value.State == State)
		{
			FoundQuests.Add(CurrentQuest.Value);
		}
	}

	return FoundQuests;
}

TArray<TSoftObjectPtr<UDA_Quest>> UQuestSubSystem::GetRequiredQuestsForQuest(TSoftObjectPtr<UDA_Quest> Quest)
{
	TArray<TSoftObjectPtr<UDA_Quest>> Quests;
	UDA_Quest* LoadedQuest = Quest.LoadSynchronous();

	if(!LoadedQuest->QuestChains.IsValidIndex(0))
	{
		return Quests;
	}
	
	//Go through all chains and their stages
	for(auto& CurrentChain : LoadedQuest->QuestChains)
	{
		CurrentChain.LoadSynchronous();
		//Find the stage containing this quest
		for(int32 FoundStage = 0; FoundStage < CurrentChain->Stages.Num(); FoundStage++)
		{
			if(CurrentChain->Stages[FoundStage].Quests.Contains(Quest))
			{
				//Quest is part of the root stage required to start this chain.
				//It has no required quests in this chain.
				if(FoundStage == 0)
				{
					break;
				}
				
				//Reverse loop to the previous stages.
				for(int32 ScannedStages = FoundStage - 1; ScannedStages >= 0; ScannedStages--)
				{
					for(auto& CurrentQuest : CurrentChain->Stages[ScannedStages].Quests)
					{
						Quests.AddUnique(CurrentQuest);
					}
				}
			}
		}
	}
	
	return Quests;
}

bool UQuestSubSystem::HasCompletedRequiredQuests(TSoftObjectPtr<UDA_Quest> Quest)
{
	UDA_Quest* LoadedQuest = Quest.LoadSynchronous();

	if(!LoadedQuest->QuestChains.IsValidIndex(0))
	{
		return true;
	}
	
	//Go through all chains and their stages
	for(auto& CurrentChain : LoadedQuest->QuestChains)
	{
		CurrentChain.LoadSynchronous();
		//Find the stage containing this quest
		for(int32 FoundStage = 0; FoundStage < CurrentChain->Stages.Num(); FoundStage++)
		{
			if(CurrentChain->Stages[FoundStage].Quests.Contains(Quest))
			{
				//Quest is part of the root stage required to start this chain.
				//It has no required quests.
				if(FoundStage == 0)
				{
					break;
				}
				
				//Reverse loop to the previous stages.
				//If any aren't completed, the required quests haven't been completed.
				for(int32 ScannedStages = FoundStage - 1; ScannedStages >= 0; ScannedStages--)
				{
					for(auto& CurrentQuest : CurrentChain->Stages[ScannedStages].Quests)
					{
						if(!HasCompletedQuest(CurrentQuest))
						{
							return false;
						}
					}
				}
			}
		}
	}
	
	return true;
}

int32 UQuestSubSystem::GetCurrentStageOnQuestChain(TSoftObjectPtr<UDA_QuestChain> QuestChain)
{
	UDA_QuestChain* LoadedQuestChain = QuestChain.LoadSynchronous();
	int32 Stage = 0;

	for(auto& CurrentStage : LoadedQuestChain->Stages)
	{
		for(auto& CurrentQuest : CurrentStage.Quests)
		{
			if(GetQuestState(CurrentQuest) == Inactive)
			{
				return Stage;
			}
		}

		Stage++;
	}

	return Stage;
}

float UQuestSubSystem::GetQuestChainProgress(TSoftObjectPtr<UDA_QuestChain> QuestChain)
{
	UQuestSubSystem* QuestSubSystem = UQuestSubSystem::Get();
	if(!QuestSubSystem)
	{
		return 0;
	}

	if(!QuestSubSystem->QuestChains.Contains(QuestChain))
	{
		return 0;
	}
	
	UDA_QuestChain* LoadedQuestChain = QuestChain.LoadSynchronous();
	int32 TotalQuests = 0;
	int32 CompletedQuests = 0;
	
	for(auto& CurrentStage : LoadedQuestChain->Stages)
	{
		for(auto& CurrentQuest : CurrentStage.Quests)
		{
			TotalQuests++;
			if(HasCompletedQuest(CurrentQuest))
			{
				CompletedQuests++;
			}
		}
	}
	
	return CompletedQuests / TotalQuests * 100.0f;
}

FQuestWrapper UQuestSubSystem::GetQuestForTask(const FGameplayTag Task)
{
	UQuestSubSystem* QuestSubSystem = UQuestSubSystem::Get();
	if(!QuestSubSystem)
	{
		return FQuestWrapper();
	}
	
	for(auto& CurrentQuest : QuestSubSystem->Quests)
	{
		for(auto& CurrentTask : CurrentQuest.Value.Tasks)
		{
			if(CurrentTask.TaskID == Task)
			{
				return CurrentQuest.Value;
			}
		}
	}

	return FQuestWrapper();
}

FTaskWrapper UQuestSubSystem::GetTaskWrapper(FGameplayTag Task)
{
	UQuestSubSystem* QuestSubSystem = UQuestSubSystem::Get();
	if(!QuestSubSystem)
	{
		return FTaskWrapper();
	}

	FQuestWrapper FoundQuest = GetQuestForTask(Task);
	if(!FoundQuest.QuestAsset)
	{
		return FTaskWrapper();
	}

	for(auto& CurrentTask : FoundQuest.Tasks)
	{
		if(CurrentTask.TaskID != Task)
		{
			continue;
		}

		return CurrentTask;
	}

	return FTaskWrapper();
}

TArray<FTaskWrapper> UQuestSubSystem::GetTasksForQuest(TSoftObjectPtr<UDA_Quest> Quest)
{
	UQuestSubSystem* QuestSubSystem = UQuestSubSystem::Get();
	if(!QuestSubSystem)
	{
		return TArray<FTaskWrapper>();
	}
	
	if(FQuestWrapper* QuestWrapper = QuestSubSystem->Quests.Find(Quest))
	{
		return QuestWrapper->Tasks;
	}
	
	return TArray<FTaskWrapper>();
}

void UQuestSubSystem::AddListenerToTask(FGameplayTag Task, UObject* Listener)
{
	UQuestSubSystem* QuestSubSystem = UQuestSubSystem::Get();
	if(!QuestSubSystem)
	{
		return;
	}
	
	FQuestWrapper FoundQuest = GetQuestForTask(Task);
	if(!FoundQuest.QuestAsset)
	{
		return;
	}

	//Get a pointer to the real quest wrapper
	FQuestWrapper* QuestWrapper = QuestSubSystem->Quests.Find(FoundQuest.QuestAsset);
	if(!QuestWrapper)
	{
		//This should NEVER happen
		return;
	}

	for(auto& CurrentTask : QuestWrapper->Tasks)
	{
		if(CurrentTask.TaskID == Task)
		{
			CurrentTask.Listeners.Add(Listener);
			return;
		}
	}
}

bool UQuestSubSystem::ProgressTask(const FGameplayTag Task, float ProgressToAdd, UObject* Instigator)
{
	UQuestSubSystem* QuestSubSystem = UQuestSubSystem::Get();
	if(!QuestSubSystem)
	{
		return false;
	}
	
	FQuestWrapper FoundQuest = GetQuestForTask(Task);
	if(!FoundQuest.QuestAsset)
	{
		return false;
	}

	//Get a pointer to the real quest wrapper
	FQuestWrapper* QuestWrapper = QuestSubSystem->Quests.Find(FoundQuest.QuestAsset);
	if(!QuestWrapper)
	{
		//This should NEVER happen
		return false;
	}

	bool TaskCompleted = false;
	bool QuestCompleted = true;
	
	for(auto& CurrentTask : QuestWrapper->Tasks)
	{
		if(CurrentTask.TaskID == Task)
		{
			//Only tasks that are in progress can be progressed.
			if(CurrentTask.State != InProgress)
			{
				return false;
			}

			//Progress is less than 0, don't bother with math and delegate.
			if(CurrentTask.CurrentProgress < 0)
			{
				return false;
			}

			if(!CanTaskBeProgressed(CurrentTask.TaskID))
			{
				return false;
			}

			const float ProgressRequired = CurrentTask.RootQuest->GetRequiredTaskProgression(CurrentTask.TaskID);
			const float ProgressDelta = (UKismetMathLibrary::Clamp(CurrentTask.CurrentProgress + ProgressToAdd, 0,ProgressRequired) - CurrentTask.CurrentProgress);
			
			CurrentTask.CurrentProgress = UKismetMathLibrary::Clamp(CurrentTask.CurrentProgress + ProgressToAdd,0,ProgressRequired);
			
			if(CurrentTask.CurrentProgress == ProgressRequired)
			{
				CurrentTask.State = Completed;
				TaskCompleted = true;
			}
			QuestSubSystem->TaskProgressed.Broadcast(CurrentTask, ProgressDelta, Instigator);
			
			//Notify listeners
			for(const auto& CurrentListener : CurrentTask.Listeners)
			{
				if(IsValid(CurrentListener))
				{
					if(UKismetSystemLibrary::DoesImplementInterface(CurrentListener, UI_QuestUpdates::StaticClass()))
					{
						II_QuestUpdates::Execute_TaskProgressed(CurrentListener, CurrentTask, ProgressDelta, Instigator);
					}
				}
			}
			
			#if ENABLE_VISUAL_LOG
			UE_VLOG_LOCATION(QuestSubSystem, TEXT("Quest System %s"), Verbose, UGameplayStatics::GetPlayerPawn(QuestSubSystem, 0)->GetActorLocation(),
			10, FColor::White, TEXT("Progressed task %s - %s / %s"),
			*Task.ToString(),
			*FString::SanitizeFloat(CurrentTask.CurrentProgress),
			*FString::SanitizeFloat(ProgressRequired));
			#endif
		}

		if(CurrentTask.State == InProgress && CurrentTask.RootQuest->IsTaskOptional(CurrentTask.TaskID) != true)
		{
			QuestCompleted = false;
		}
	}

	if(QuestCompleted)
	{
		CompleteQuest(QuestWrapper->QuestAsset, false);
	}

	if(TaskCompleted)
	{
		return true;
	}
	
	return false;
}

float UQuestSubSystem::GetTaskProgress(FGameplayTag Task)
{
	FTaskWrapper TaskWrapper = GetTaskWrapper(Task);
	if(TaskWrapper.IsValid())
	{
		return TaskWrapper.CurrentProgress;
	}

	return 0;
}

TEnumAsByte<EQuestState> UQuestSubSystem::GetTaskState(FGameplayTag Task)
{
	FTaskWrapper TaskWrapper = GetTaskWrapper(Task);
	if(TaskWrapper.IsValid())
	{
		return TaskWrapper.State;
	}

	return Inactive;
}

bool UQuestSubSystem::CompleteTask(FGameplayTag Task, UObject* Instigator)
{
	UQuestSubSystem* QuestSubSystem = UQuestSubSystem::Get();
	if(!QuestSubSystem)
	{
		return false;
	}

	FQuestWrapper FoundQuest = GetQuestForTask(Task);
	if(!FoundQuest.QuestAsset)
	{
		return false;
	}

	if(GetTaskState(Task) == InProgress)
	{
		ProgressTask(Task, FoundQuest.QuestAsset->GetRequiredTaskProgression(Task), Instigator);
		return true;
	}
	
	return false;
}

bool UQuestSubSystem::CanTaskBeProgressed(FGameplayTag Task)
{
	UQuestSubSystem* QuestSubSystem = UQuestSubSystem::Get();
	if(!QuestSubSystem)
	{
		return false;
	}
	
	FQuestWrapper FoundQuest = GetQuestForTask(Task);
	if(!FoundQuest.QuestAsset)
	{
		return false;
	}

	//Get a pointer to the real quest wrapper
	FQuestWrapper* QuestWrapper = QuestSubSystem->Quests.Find(FoundQuest.QuestAsset);
	if(!QuestWrapper)
	{
		//This should NEVER happen
		return false;
	}

	for(auto& CurrentTask : QuestWrapper->Tasks)
	{
		if(CurrentTask.TaskID == Task)
		{
			if(CurrentTask.State != InProgress)
			{
				return false;
			}
			
			if(CurrentTask.CurrentProgress >= CurrentTask.RootQuest->GetRequiredTaskProgression(CurrentTask.TaskID))
			{
				return false;
			}

			if(UFL_QuestHelpers::IsTaskRequirementsMet(CurrentTask.RootQuest->GetTasksRequirements(CurrentTask.TaskID)))
			{
				
			}
			
			if(Execute_PreventTaskProgress(QuestSubSystem, CurrentTask))
			{
				return false;
			}

			for(const auto& CurrentListener : CurrentTask.Listeners)
			{
				if(IsValid(CurrentListener))
				{
					if(UKismetSystemLibrary::DoesImplementInterface(CurrentListener, UI_QuestUpdates::StaticClass()))
					{
						if(Execute_PreventTaskProgress(CurrentListener, CurrentTask))
						{
							return false;
						}
					}
				}
			}
		}
	}
	
	return true;
}

bool UQuestSubSystem::FailTask(const FGameplayTag Task, const bool bFailQuest)
{
	UQuestSubSystem* QuestSubSystem = UQuestSubSystem::Get();
	if(!QuestSubSystem)
	{
		return false;
	}
	
	FQuestWrapper FoundQuest = GetQuestForTask(Task);
	if(!FoundQuest.QuestAsset)
	{
		return false;
	}

	//Get a pointer to the real quest wrapper
	FQuestWrapper* QuestWrapper = QuestSubSystem->Quests.Find(FoundQuest.QuestAsset);
	if(!QuestWrapper)
	{
		//This should NEVER happen
		return false;
	}

	for(auto& CurrentTask : QuestWrapper->Tasks)
	{
		if(CurrentTask.TaskID == Task)
		{
			CurrentTask.State = Failed;

			#if ENABLE_VISUAL_LOG
			UE_VLOG_LOCATION(QuestSubSystem, TEXT("Quest System %s"), Verbose, UGameplayStatics::GetPlayerPawn(QuestSubSystem, 0)->GetActorLocation(),
			10, FColor::White, TEXT("Failed task: %s"),
			*Task.ToString());
			#endif

			QuestSubSystem->TaskFailed.Broadcast(CurrentTask);
				
			for(auto& CurrentListener : CurrentTask.Listeners)
			{
				if(IsValid(CurrentListener))
				{
					if(UKismetSystemLibrary::DoesImplementInterface(CurrentListener, UI_QuestUpdates::StaticClass()))
					{
						II_QuestUpdates::Execute_TaskFailed(CurrentListener, CurrentTask);
					}
				}
			}
			break;
		}
	}
		
	if(bFailQuest)
	{
		//Since we are failing a specific task, don't go ahead and fail the others.
		FailQuest(QuestWrapper->QuestAsset, false);
	}
	else
	{
		CompleteQuest(QuestWrapper->QuestAsset, false, false);
	}

	return true;
}

bool UQuestSubSystem::AddTaskToQuest(FQuestTask Task, TSoftObjectPtr<UDA_Quest> Quest)
{
	// const int32 QuestIndex = GetQuestIndex_Active(Quest);
	//
	// if(ActiveQuests.IsValidIndex(QuestIndex))
	// {
	// 	const FTaskWrapper TaskWrapper = UFL_QuestHelpers::WrapTask(Quest.LoadSynchronous(), Task);
	//
	// 	ActiveQuests[QuestIndex].Tasks.Add(TaskWrapper);
	//
	// 	TaskAddedToQuest.Broadcast(TaskWrapper, ActiveQuests[QuestIndex]);
	//
	// 	for(const auto& CurrentListener : ActiveQuests[QuestIndex].Listeners)
	// 	{
	// 		if(IsValid(CurrentListener))
	// 		{
	// 			if(UKismetSystemLibrary::DoesImplementInterface(CurrentListener, UI_QuestUpdates::StaticClass()))
	// 			{
	// 				II_QuestUpdates::Execute_TaskAddedToQuest(CurrentListener, TaskWrapper, ActiveQuests[QuestIndex]);
	// 			}
	// 		}
	// 	}
	// 	
	// 	return true;
	// }

	return false;
}

bool UQuestSubSystem::RemoveTaskFromQuest(FGameplayTag Task, TSoftObjectPtr<UDA_Quest> Quest)
{
	// const int32 QuestIndex = GetQuestIndex_Active(Quest);
	//
	// if(ActiveQuests.IsValidIndex(QuestIndex))
	// {
	// 	FQuestWrapper& QuestWrapper = ActiveQuests[QuestIndex];
	// 	for(int32 CurrentIndex = 0; CurrentIndex < QuestWrapper.Tasks.Num(); CurrentIndex++)
	// 	{
	// 		if(QuestWrapper.Tasks[CurrentIndex].TaskID == Task)
	// 		{
	// 			const FTaskWrapper TaskWrapper = QuestWrapper.Tasks[CurrentIndex];
	// 			QuestWrapper.Tasks.RemoveAt(CurrentIndex);
	//
	// 			TaskRemovedFromQuest.Broadcast(TaskWrapper, ActiveQuests[QuestIndex]);
	//
	// 			for(const auto& CurrentListener : ActiveQuests[QuestIndex].Listeners)
	// 			{
	// 				if(IsValid(CurrentListener))
	// 				{
	// 					if(UKismetSystemLibrary::DoesImplementInterface(CurrentListener, UI_QuestUpdates::StaticClass()))
	// 					{
	// 						II_QuestUpdates::Execute_TaskRemovedFromQuest(CurrentListener, TaskWrapper, ActiveQuests[QuestIndex]);
	// 					}
	// 				}
	// 			}
	//
	// 			for(const auto& CurrentListener : TaskWrapper.Listeners)
	// 			{
	// 				if(IsValid(CurrentListener))
	// 				{
	// 					if(UKismetSystemLibrary::DoesImplementInterface(CurrentListener, UI_QuestUpdates::StaticClass()))
	// 					{
	// 						II_QuestUpdates::Execute_TaskRemovedFromQuest(CurrentListener, TaskWrapper, ActiveQuests[QuestIndex]);
	// 					}
	// 				}
	// 			}
	// 			break;
	// 		}
	// 	}
	//
	// 	#if ENABLE_VISUAL_LOG
	// 	UE_VLOG_LOCATION(this, TEXT("Quest System %s"), Verbose, UGameplayStatics::GetPlayerPawn(this, 0)->GetActorLocation(),
	// 	10, FColor::White, TEXT("Removed task (%s) from quest (%s)"),
	// 	*Task.ToString(),
	// 	*Quest->GetName());
	// 	#endif
	//
	// 	//If all the remaining tasks are optional, drop the quest.
	// 	bool RemainingTasksAreOptional = true;
	// 	for(auto& CurrentTask : QuestWrapper.Tasks)
	// 	{
	// 		if(!CurrentTask.RootQuest->IsTaskOptional(CurrentTask.TaskID))
	// 		{
	// 			RemainingTasksAreOptional = false;
	// 			break;
	// 		}
	// 	}
	//
	// 	if(QuestWrapper.Tasks.Num() == 0 || RemainingTasksAreOptional)
	// 	{
	// 		DropQuest(QuestWrapper);
	// 	}
	// 	
	// 	return true;
	// }

	return false;
}

void UQuestSubSystem::SetQuestState(const TArray<FString>& Args)
{
	UQuestSubSystem* QuestSubSystem = UQuestSubSystem::Get();
	if(!QuestSubSystem)
	{
		return;
	}

	if(Args.Num() != 2)
	{
		UKismetSystemLibrary::PrintString(QuestSubSystem, "SetQuestState only accepts 2 arguments");
		return;
	}

	FString QuestName = Args[0];
	TSoftObjectPtr<UDA_Quest> QuestAsset = nullptr;
	//Use the AssetRegistry to search for assets
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	//Search for all assets in the project
	//There might be a way of handling this, but as a dev tool; if it works, it works.
	TArray<FAssetData> AssetDataArray;
	AssetRegistry.GetAssetsByPath(FName(TEXT("/Game")), AssetDataArray, true);

	//Iterate over the assets to find the one matching the QuestName
	for (const FAssetData& AssetData : AssetDataArray)
	{
		if (AssetData.AssetName.ToString() == QuestName)
		{
			// Construct the full path to the asset
			FString AssetPath = AssetData.GetSoftObjectPath().ToString();

			// Create a TSoftObjectPtr using the asset path
			QuestAsset = AssetPath;

			if(!QuestAsset)
			{
				UKismetSystemLibrary::PrintString(QuestSubSystem, "Could not find quest asset");
				return;
			}

			break;
		}
	}

	/**Start attempting to set the quest state appropriately.
	 * This console command is quite literal. It will try its best to set the state
	 * of the quest to whatever the tester wants. This includes possibly dropping,
	 * completing, or accepting the quest, sometimes forcibly
	 */
	TEnumAsByte<EQuestState> QuestState = QuestSubSystem->GetQuestState(QuestAsset);
	if(Args[1] == "Inactive")
	{
		if(QuestState != Inactive)
		{
			QuestSubSystem->DropQuest(QuestAsset);
		}
	}
	else if (Args[1] == "InProgress")
	{
		if(QuestState == Completed || QuestState == Failed)
		{
			//In case the player has already completed or failed the quest
			QuestSubSystem->DropQuest(QuestAsset);
		}
		if(QuestState != InProgress)
		{
			QuestSubSystem->AcceptQuest(QuestAsset, true);
		}
	}
	else if (Args[1] == "Completed")
	{
		if(QuestState != Completed)
		{
			QuestSubSystem->CompleteQuest(QuestAsset, true, true);
		}
	}
	else if(Args[1] == "Failed")
	{
		if(QuestState == Inactive)
		{
			QuestSubSystem->AcceptQuest(QuestAsset);
		}
		if(QuestState != Failed)
		{
			QuestSubSystem->FailQuest(QuestAsset, true);
		}
	}
	else
	{
		UKismetSystemLibrary::PrintString(QuestSubSystem, "Invalid state passed into SetQuestState");
	}
}
