// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AC_PerformanceDirector.generated.h"

/**This enum is designed to be expanded
 * or modified depending on what your team
 * deems as a "importance level", though
 * there should always be a "Unknown" in
 * the case that an importance can't be
 * calculated. No gameplay logic or updates
 * should follow an Unknown importance update.*/
UENUM(Blueprintable)
enum EPerformanceImportance
{
	/**Multithreading can have moments where an actor is destroyed
	 * or the component is destroyed while the other thread is still
	 * active. This can lead to moments where no importance can be
	 * retrieved, but we do not want to risk sending a false update.
	 *
	 * This should never be used for DefaultImportance. This is only
	 * used as a failsafe and should have no logic connected to it.*/
	Unknown,
	/**Hero objects typically should have no limits applied to them.
	 * This could be the players vehicle, weapon, etc.*/
	Hero,
	/**Importance is critical, very few limits, if any, should be
	 * applied. This actor is more important than most other actors.*/
	Critical,
	/**General importance is high, start removing some limitations.*/
	High,
	/**Actor should not try to modify its code or general behavior.
	* Actor should behave as by default.
	* In general, your actor should try to be optimized by default.*/
	Normal,
	/**Actor might be visible, but is a moderate distance away.
	 * This would usually mean disabling collision and lowering
	 * tick rate.*/
	ImportantBackground,
	/**Actor might be behind a nearby wall or has line of sight
	 * of the player, but can have some minor limitations applied,
	 * such as disabling skeletal mesh updates.*/
	Background,
	/**Actor is somewhat relevant, but is far away enough that
	 * most limitations can be applied.*/
	LowBackground,
	/**Actor has nearly no importance anymore. This could be moments
	 * where you might want to consider completely removing meshes,
	 * disabling shadows, disabling tick and generally lowering
	 * the importance update interval.*/
	Low,
	/**Completely disable everything, actor should run no gameplay
	 * logic and be virtually non-existent on the profiler.*/
	Irrelevant
};

UENUM(BlueprintType)
enum EUpdateThread
{
	//Fastest and most responsive, but impacts
	//game performance the most.
	GameThread,
	//Slowest and least responsive, but has
	//extremely little impact on performance.
	BackgroundThread
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FImportanceUpdated, TEnumAsByte<EPerformanceImportance>, OldImportance, TEnumAsByte<EPerformanceImportance>, NewImportance);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), DisplayName = "Performance Director")
class PERFORMANCEDIRECTOR_API UAC_PerformanceDirector : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAC_PerformanceDirector();

	/**Typically this component is automatically destroyed on
	 * dedicated servers. This lets you disable that behavior.*/
	UPROPERTY(Category = "Settings", BlueprintReadOnly, EditAnywhere)
	bool RunOnDedicatedServer = false;

	UPROPERTY(Category = "Settings", BlueprintReadOnly, EditAnywhere)
	bool StartTrackingOnBeginPlay = false;

	/**What importance should this actor have on BeginPlay or what
	 * importance should be set when ResetImportance is called?*/
	UPROPERTY(Category = "Settings", BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<EPerformanceImportance> DefaultImportance = Normal;

	/**Generally not recommended to change during runtime, will
	 * only go into affect after tracking is reset.
	 *
	 * Majority of the time you will want @BackgroundThread,
	 * @GameThread should rarely be used.*/
	UPROPERTY(Category = "Settings", BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<EUpdateThread> Thread = BackgroundThread;

	/**How often should this actor be checked for updates?
	 * If 0, it will check every frame.
	 * TODO: If 0, use tick instead.*/
	UPROPERTY(Category = "Settings", BlueprintReadOnly, EditAnywhere)
	float UpdateInterval = 0.1;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FImportanceUpdated ImportanceUpdated;

	TQueue<EPerformanceImportance> ThreadTaskQueue;

protected:

	EPerformanceImportance CurrentImportance = Normal;

	bool bIsTracking = false;

	FTimerHandle TimerHandle;
	
	// Called when the game starts
	virtual void BeginPlay() override;

	//Need to make sure to stop the thread when this component is destroyed.
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	UFUNCTION(Category = "Performance Director", BlueprintCallable)
	void StartTracking();

	UFUNCTION(Category = "Performance Director", BlueprintCallable)
	void StopTracking(bool bResetImportance = true);

	UFUNCTION(Category = "Performance Director", BlueprintCallable)
	TEnumAsByte<EPerformanceImportance> GetCurrentImportance(bool EvaluateImportance);

	UFUNCTION(Category = "Performance Director", BlueprintCallable)
	void SetImportance(TEnumAsByte<EPerformanceImportance> NewImportance);

	UFUNCTION(Category = "Performance Director", BlueprintCallable)
	void ResetImportance();
	
	UFUNCTION(Category = "Performance Director", BlueprintCallable)
	void SetUpdateInterval(float NewUpdateInterval);

	void StartTrackingTimer();
};