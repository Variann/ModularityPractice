// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "EditorAssistantThumbnailRenderer.h"

#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "Config/DS_EditorAssistantSettings.h"

void UEditorAssistantThumbnailRenderer::Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height,
                                             FRenderTarget* Viewport, FCanvas* Canvas, bool bAdditionalViewFamily)
{
	UBlueprint* Blueprint = Cast<UBlueprint>(Object);
	UDataAsset* DataAsset = Cast<UDataAsset>(Object);
	if(!Blueprint && !DataAsset)
	{
		Super::Draw(Object, X, Y, Width, Height, Viewport, Canvas, bAdditionalViewFamily);
		return;
	}

	//Background by default is pitch black. Set it to the normal gray background.
	//V: There must be a better way of getting the values.
	Canvas->Clear(FLinearColor(0.09803922f,0.10196078f,0.10980392f));

	UTexture2D* ThumbnailToUse = FindThumbnailOverrideForObject(Object);
	
	if(ThumbnailToUse)
	{
		FCanvasTileItem CanvasTile(FVector2D(X, Y), ThumbnailToUse->GetResource(), FVector2D(Width, Height), FLinearColor::White);
		CanvasTile.BlendMode = SE_BLEND_Translucent;
		CanvasTile.Draw(Canvas);
		return;
	}
	
	Super::Draw(Object, X, Y, Width, Height, Viewport, Canvas, bAdditionalViewFamily);
}

bool UEditorAssistantThumbnailRenderer::CanVisualizeAsset(UObject* Object)
{
	if(Super::CanVisualizeAsset(Object))
	{
		return true;
	}

	const UDS_EditorAssistantSettings* Settings = GetDefault<UDS_EditorAssistantSettings>();

	/**Find out if the current object we are trying to visualize is in the
	 * ThumbnailOverrides map or is a child of any classes in the map.
	 * If so, we want to visualize it.*/
	for(auto& CurrentThumbnail : Settings->ThumbnailOverrides)
	{
		
		if(CurrentThumbnail.Key.ToSoftObjectPath().IsValid() && CurrentThumbnail.Value.ToSoftObjectPath().IsValid())
		{
			if(Object->GetClass() == CurrentThumbnail.Key || Object->GetClass()->IsChildOf(CurrentThumbnail.Key.LoadSynchronous()))
			{
				return true;
			}
		}
	}

	for(const auto& [Property, Value] : TPropertyValueRange<FSoftObjectProperty>(Object->GetClass(), Object))
	{
		if (Property->PropertyClass != UTexture2D::StaticClass() || !Property->GetName().Contains("ThumbnailOverride"))
		{
			continue;
		}

		void* MutableValue = const_cast<void*>(Value);
		const TSoftObjectPtr<UTexture2D>& SoftTexture = *static_cast<TSoftObjectPtr<UTexture2D>*>(MutableValue);
		if(SoftTexture.ToSoftObjectPath().IsValid())
		{
			return true;
		}
	}

	for (const auto& [Property, Value] : TPropertyValueRange<FSoftObjectProperty>(Object->GetClass(), Object))
	{
		if (Property->PropertyClass != UTexture2D::StaticClass() || !Property->HasMetaData("ThumbnailOverride"))
		{
			continue;
		}

		void* MutableValue = const_cast<void*>(Value);
		const TSoftObjectPtr<UTexture2D>& SoftTexture = *static_cast<TSoftObjectPtr<UTexture2D>*>(MutableValue);
		if(SoftTexture.ToSoftObjectPath().IsValid())
		{
			return true;
		}
	}
	
	for (const auto& [Property, Value] : TPropertyValueRange<FObjectProperty>(Object->GetClass(), Object))
	{
		if(!Property->HasMetaData("ThumbnailOverride"))
		{
			continue;
		}

		void* MutableValue = const_cast<void*>(Value);
		const TSoftObjectPtr<UTexture2D> SoftTexture = Cast<UTexture2D>(Property->GetObjectPtrPropertyValue(Value));

		if(SoftTexture.ToSoftObjectPath().IsValid())
		{
			return true;
		}
	}

	return false;
}

UTexture2D* UEditorAssistantThumbnailRenderer::FindThumbnailOverrideForObject(UObject* Object)
{
	//ThumbnailOverrides has the highest priority, if anything is found, use that texture.
	UClass* ObjectClass = Object->GetClass();
	const UDS_EditorAssistantSettings* Settings = GetDefault<UDS_EditorAssistantSettings>();
	for(auto& CurrentThumbnail : Settings->ThumbnailOverrides)
	{
		if(CurrentThumbnail.Key.ToSoftObjectPath().IsValid() && CurrentThumbnail.Value.ToSoftObjectPath().IsValid())
		{
			//Find out if the class is in the override map or is a child of the classes in the map
			if(ObjectClass == CurrentThumbnail.Key.LoadSynchronous() || ObjectClass->IsChildOf(CurrentThumbnail.Key.LoadSynchronous()))
			{
				return CurrentThumbnail.Value.LoadSynchronous();;
			}
		}
	}

	//Go through all hard object references and find any with the name "ThumbnailOverride". If found, use that.
	for(const auto& [Property, Value] : TPropertyValueRange<FObjectProperty>(ObjectClass, ObjectClass->GetDefaultObject()))
	{
		if (Property->PropertyClass != UTexture2D::StaticClass() || !Property->GetName().Equals("ThumbnailOverride"))
		{
			continue;
		}

		void* MutableValue = const_cast<void*>(Value);
		UTexture2D* Texture = *static_cast<UTexture2D**>(MutableValue);
		if(!Texture)
		{
			continue;
		}
		
		return Texture;
	}

	//Go through all soft object references and find any with the name "ThumbnailOverride". If found, use that.
	for(const auto& [Property, Value] : TPropertyValueRange<FSoftObjectProperty>(ObjectClass, ObjectClass->GetDefaultObject()))
	{
		if (Property->PropertyClass != UTexture2D::StaticClass() || !Property->GetName().Equals("ThumbnailOverride"))
		{
			continue;
		}

		void* MutableValue = const_cast<void*>(Value);
		const TSoftObjectPtr<UTexture2D>& SoftTexture = *static_cast<TSoftObjectPtr<UTexture2D>*>(MutableValue);
		if(SoftTexture.ToSoftObjectPath().IsValid())
		{
			return SoftTexture.LoadSynchronous();
		}
	}

	//Go through any soft object properties with the ThumbnailOverride metadata. If found, use that.
	for (const auto& [Property, Value] : TPropertyValueRange<FSoftObjectProperty>(Object->GetClass(), Object))
	{
		if (Property->PropertyClass != UTexture2D::StaticClass() || !Property->HasMetaData("ThumbnailOverride"))
		{
			continue;
		}

		void* MutableValue = const_cast<void*>(Value);
		const TSoftObjectPtr<UTexture2D>& SoftTexture = *static_cast<TSoftObjectPtr<UTexture2D>*>(MutableValue);
		if(SoftTexture.ToSoftObjectPath().IsValid())
		{
			return SoftTexture.LoadSynchronous();
		}
	}

	//Go through any hard object properties with the ThumbnailOverride metadata. If found, use that.
	for(const auto& [Property, Value] : TPropertyValueRange<FObjectProperty>(Object->GetClass(), Object))
	{
		if (Property->PropertyClass != UTexture2D::StaticClass() || !Property->HasMetaData("ThumbnailOverride"))
		{
			continue;
		}

		void* MutableValue = const_cast<void*>(Value);

		UTexture2D* Texture = *static_cast<UTexture2D**>(MutableValue);
		if(!Texture)
		{
			continue;
		}
		
		return Texture;
	}

	return nullptr;
}
