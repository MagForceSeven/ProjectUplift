// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"

#include "ShellExtensionWidget.generated.h"

// The base type for a widget that can be included in the list of Main Menu options using the
// 'Add Shell Widgets' Game Feature Action
UCLASS( Abstract )
class UPLIFT_API UShellExtensionWidget : public UUserWidget
{
	GENERATED_BODY( )
public:
};