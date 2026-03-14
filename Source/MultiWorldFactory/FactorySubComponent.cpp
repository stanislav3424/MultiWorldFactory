// Fill out your copyright notice in the Description page of Project Settings.


#include "FactorySubComponent.h"

void UFactorySubComponent::InitializeFactoryComponent(UPlanetWorld* PlanetWorld,
    FDataTableRowHandle const& InitFactoryComponentDataTableHandle, UFactoryComponent* OwnerFactoryComponent)
{
    if (!OwnerFactoryComponent)
    {
        UE_LOG(FactoryComponent, Error, TEXT("Factory sub-component %s must have an owner factory component."),
            *GetName());
        return;
    }
    if (OwnerFactoryComponent->IsAttached())
    {
        UE_LOG(FactoryComponent, Error,
            TEXT("Factory sub-component %s cannot be attached to an already attached owner factory component."),
            *GetName());
        return;
    }

    Super::InitializeFactoryComponent(PlanetWorld, InitFactoryComponentDataTableHandle, OwnerFactoryComponent);
}