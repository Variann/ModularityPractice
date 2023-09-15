//==========================================================================//
// Copyright Elhoussine Mehnik (ue4resources@gmail.com). All Rights Reserved.
//================== http://unrealengineresources.com/ =====================//

#include "AssetThumbnailWidgetCustom.h"

#include "LevelEditor.h"
#include "Modules/ModuleManager.h"
#include "AssetThumbnail.h"

#include "Widgets/SNullWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "UMGStyle.h"

#define LOCTEXT_NAMESPACE "UMG"

UAssetThumbnailWidgetCustom::UAssetThumbnailWidgetCustom(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Asset(nullptr)
	, ThumbnailSize(64.0f, 64.0f)
	, AssetThumbnailPtr(nullptr)
{

}

void UAssetThumbnailWidgetCustom::SetThumbnail(UObject* NewAsset)
{
	Asset = NewAsset;
	UpdateWidgetContent();
}

void UAssetThumbnailWidgetCustom::RefreshThumbnail()
{
	if (AssetThumbnailPtr.IsValid())
		AssetThumbnailPtr->RefreshThumbnail();
}

void UAssetThumbnailWidgetCustom::SetThumbnailSize(FVector2D NewSize /*= FVector2D(256.0f, 256.0f)*/)
{
	ThumbnailSize = NewSize;
	UpdateWidgetContent();
}

void UAssetThumbnailWidgetCustom::UpdateWidgetContent()
{
	ThumbnailSize.X = FMath::Max<float>(ThumbnailSize.X, 4.0f);
	ThumbnailSize.Y = FMath::Max<float>(ThumbnailSize.Y, 4.0f);

	FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	TWeakPtr<class ILevelEditor> LevelEditor = LevelEditorModule.GetLevelEditorInstance();
	TSharedPtr<FAssetThumbnailPool> ThumbnailPool = LevelEditor.Pin()->GetThumbnailPool();
	TSharedPtr < FAssetThumbnail> AssetThumbnail = MakeShareable(new FAssetThumbnail(FAssetData(Asset, true), ThumbnailSize.X, ThumbnailSize.Y, ThumbnailPool));


	TSharedPtr<SWidget> StableMyWidget = MyWidget.Pin();
	if (StableMyWidget.IsValid())
	{
		TSharedPtr<SBox> MyBox = StaticCastSharedPtr<SBox>(StableMyWidget);
		MyBox->SetContent(AssetThumbnail->MakeThumbnailWidget());
	}

	AssetThumbnailPtr = AssetThumbnail;
}

void UAssetThumbnailWidgetCustom::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	SlateWidget.Reset();
}

TSharedRef<SWidget> UAssetThumbnailWidgetCustom::RebuildWidget()
{
	return SNew(SBox)
		[
			(SlateWidget.IsValid()) ? SlateWidget.ToSharedRef() : GetDefaultContent()
		];
}

void UAssetThumbnailWidgetCustom::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	UpdateWidgetContent();
}

TSharedRef<SWidget> UAssetThumbnailWidgetCustom::GetDefaultContent()
{
	if ( IsDesignTime() )
	{
		return SNew(SBorder)
			.Visibility(EVisibility::HitTestInvisible)
			.BorderImage(FUMGStyle::Get().GetBrush("MarchingAnts"))
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("AssetThumbnailWidgetText", "Asset Thumbnail Widget"))
			];
	}
	else
	{
		return SNullWidget::NullWidget;
	}
}

#undef LOCTEXT_NAMESPACE
