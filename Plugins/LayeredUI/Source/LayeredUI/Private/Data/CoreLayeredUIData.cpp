#pragma once

#include "CoreMinimal.h"
#include "Data/CoreLayeredUIData.h"


void FLayeredWidget::ResetStruct()
{
	Widget = nullptr;
	Layer = FGameplayTag();
	ZOrder = -1;
	LayerPriority = -1;
	HideCursor = false;
}
