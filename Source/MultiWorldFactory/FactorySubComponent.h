// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FactoryComponent.h"
#include "FactorySubComponent.generated.h"

/**
 * Base class for all factory sub-components. 
 * As a only first-generation component, not head.
 */
UCLASS(NotBlueprintable, Abstract)
class MULTIWORLDFACTORY_API UFactorySubComponent : public UFactoryComponent
{
    GENERATED_BODY()

private:
    virtual void InitializeFactoryComponent(UPlanetWorld* PlanetWorld,
        FDataTableRowHandle const&                        InitFactoryComponentDataTableHandle,
        UFactoryComponent*                                OwnerFactoryComponent = nullptr) override final;
};
