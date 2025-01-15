// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Types/FlowDataPinBlueprintLibrary.h"
#include "FlowLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowDataPinBlueprintLibrary)

uint8 UFlowDataPinBlueprintLibrary::AutoConvert_FlowDataPinPropertyEnumToEnum(const FFlowDataPinOutputProperty_Enum& EnumProperty)
{
	if (IsValid(EnumProperty.EnumClass))
	{
		const uint64 EnumValueAsInt = EnumProperty.EnumClass->GetValueByName(EnumProperty.Value);

		// At least For Now(tm) Blueprint Enums want to be uint8's
		return static_cast<uint8>(EnumValueAsInt);
	}

	UE_LOG(LogFlow, Error, TEXT("Could not cast enum value %s, because missing enum class"), *EnumProperty.Value.ToString());

	return static_cast<uint8>(INDEX_NONE);
}

uint8 UFlowDataPinBlueprintLibrary::AutoConvert_FlowDataPinResultEnumToEnum(const FFlowDataPinResult_Enum& EnumProperty)
{
	if (IsValid(EnumProperty.EnumClass))
	{
		const uint64 EnumValueAsInt = EnumProperty.EnumClass->GetValueByName(EnumProperty.Value);

		// At least For Now(tm) Blueprint Enums want to be uint8's
		return static_cast<uint8>(EnumValueAsInt);
	}

	UE_LOG(LogFlow, Error, TEXT("Could not cast enum value %s, because missing enum class"), *EnumProperty.Value.ToString());

	return static_cast<uint8>(INDEX_NONE);
}
