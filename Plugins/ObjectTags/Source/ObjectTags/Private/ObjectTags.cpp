// Copyright Epic Games, Inc. All Rights Reserved.

#include "ObjectTags.h"
#include "ObjectTags_Subsystem.h"
#include "Kismet/KismetSystemLibrary.h"

#define LOCTEXT_NAMESPACE "FObjectTagsModule"

void FObjectTagsModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	IConsoleManager::Get().RegisterConsoleCommand(
			TEXT("ToggleViewActorTags"),
			TEXT("Toggle on or off the ObjectTag's in-world debugger. This will show text in the level for any actor that has any tags associated with it."),
			FConsoleCommandDelegate::CreateLambda([this]()
			{
				if(TickDelegateHandle.IsValid())
				{
					FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
				}
				else
				{
					TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FObjectTagsModule::Tick));
				}
			}));
}

void FObjectTagsModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

bool FObjectTagsModule::Tick(float DeltaTime)
{
	UObjectTags_Subsystem* Tags_Subsystem = UObjectTags_Subsystem::Get();
	if(!Tags_Subsystem)
	{
		return false;
	}

	for(auto& CurrentObject : Tags_Subsystem->ObjectTags)
	{
		if(AActor* Actor = Cast<AActor>(CurrentObject.Key))
		{
			if(APawn* Pawn = Cast<APawn>(Actor))
			{
				//This will skip the players pawn
				if(Pawn->IsPawnControlled())
				{
					if(Pawn->Controller->IsLocalPlayerController())
					{
						continue;
					}
				}
			}
			
			if(APlayerController* Controller = Cast<APlayerController>(Actor))
			{
				//We are never interested in the controller
				continue;
			}
			
			FVector ActorCenter;
			FVector ActorBounds;
			Actor->GetActorBounds(true, ActorCenter, ActorBounds);
			FString Text;

			for(auto& CurrentTag : CurrentObject.Value.TagsAndValues)
			{
				if(!Text.IsEmpty())
				{
					Text += "\n";
				}
				
				Text += CurrentTag.Key.ToString() + " - " + FString::SanitizeFloat(CurrentTag.Value);
				if(CurrentObject.Value.TagTimers.Contains(CurrentTag.Key))
				{
					Text += " : " + FString::SanitizeFloat(UKismetSystemLibrary::K2_GetTimerRemainingTimeHandle(Actor, *CurrentObject.Value.TagTimers.Find(CurrentTag.Key)));
				}
			}
			
			DrawDebugString(Actor->GetWorld(), ActorCenter - Actor->GetActorLocation() + FVector(0, 20, 40), Text, Actor, FColor::White, 0, true);
		}
	}
	
	return true;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FObjectTagsModule, ObjectTags)