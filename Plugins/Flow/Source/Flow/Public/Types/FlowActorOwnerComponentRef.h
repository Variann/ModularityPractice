// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "UObject/ObjectPtr.h"

#include "FlowActorOwnerComponentRef.generated.h"

// Forward Declarations
class UActorComponent;

// Similar to FAnimNodeFunctionRef, providing a FName-based Component binding
//  that is resolved at runtime
USTRUCT(BlueprintType)
struct FFlowActorOwnerComponentRef
{
	GENERATED_BODY()

public:

	// Tries to find the component by name on the given actor
	UActorComponent* TryResolveComponent(const AActor& InActor, bool bWarnIfFailed = true);

	// In some cases, the component can be resolved directly
	void SetResolvedComponentDirect(UActorComponent& Component);

	// Returns a the resolved component
	//  (assumes TryResolveComponent() was called previously)
	UActorComponent* GetResolvedComponent() const { return ResolvedComponent; }

	// Accessors
	bool IsConfigured() const { return !ComponentName.IsNone(); }
	bool IsResolved() const;

	static UActorComponent* TryResolveComponentByName(const AActor& InActor, const FName& InComponentName);

public:

	// The name of the component
	UPROPERTY(VisibleAnywhere, Category = "Flow Actor Owner Component")
	FName ComponentName = NAME_None;

protected:

	// Cached resolved component (resolved at runtime by calling TryResolveComponent)
	UPROPERTY(Transient)
	TObjectPtr<UActorComponent> ResolvedComponent = nullptr;
};
