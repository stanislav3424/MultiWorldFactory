// Fill out your copyright notice in the Description page of Project Settings.

#include "FactoryComponent.h"

DEFINE_LOG_CATEGORY(FactoryComponent);

void UFactoryComponent::AttachChildComponent(UFactoryComponent* OtherFactoryComponent)
{
    if (!OtherFactoryComponent)
    {
        UE_LOG(
            FactoryComponent, Error, TEXT("UFactoryComponent::AtachFactoryComponent: OtherFactoryComponent is null"));
        return;
    }
    if (this == OtherFactoryComponent)
    {
        UE_LOG(FactoryComponent, Error, TEXT("UFactoryComponent::AtachFactoryComponent: Cannot attach self"));
        return;
    }
    if (IsAttachedToFactoryComponent(OtherFactoryComponent))
    {
        UE_LOG(FactoryComponent, Error,
            TEXT("UFactoryComponent::AtachFactoryComponent: OtherFactoryComponent is already attached"));
        return;
    }
    if (OtherFactoryComponent->IsAttached())
    {
        UE_LOG(FactoryComponent, Error,
            TEXT("UFactoryComponent::AtachFactoryComponent: OtherFactoryComponent is already attached to another "
                 "component"));
        return;
    }
    
    AttachedFactoryComponents.Add(OtherFactoryComponent);
    OtherFactoryComponent->AttachedToOwner(this);
}

void UFactoryComponent::DetachChildComponent(UFactoryComponent* OtherFactoryComponent)
{
    if (!OtherFactoryComponent)
    {
        UE_LOG(FactoryComponent, Error, TEXT("UFactoryComponent::DetachChildComponent: OtherFactoryComponent is null"));
        return;
    }
    if (this == OtherFactoryComponent)
    {
        UE_LOG(FactoryComponent, Error, TEXT("UFactoryComponent::DetachChildComponent: Cannot detach self"));
        return;
    }
    if (!IsAttachedToFactoryComponent(OtherFactoryComponent))
    {
        UE_LOG(FactoryComponent, Error,
            TEXT("UFactoryComponent::DetachChildComponent: OtherFactoryComponent is not attached"));
        return;
    }

    OtherFactoryComponent->DetachedFromOwner(this);
    AttachedFactoryComponents.Remove(OtherFactoryComponent);
}

UFactoryComponent* UFactoryComponent::GetAttachedChildComponent(
    TSubclassOf<UFactoryComponent> ComponentClass, bool bIncludeChildren)
{
    for (auto AttachedComponent : AttachedFactoryComponents)
    {
        if (!AttachedComponent)
            return nullptr;
        if (AttachedComponent->IsA(ComponentClass))
            return AttachedComponent;
        if (bIncludeChildren)
            return AttachedComponent->GetAttachedChildComponent(ComponentClass, bIncludeChildren);
    }
    return nullptr;
}

void UFactoryComponent::GetAllAttachedChildComponents(TSubclassOf<UFactoryComponent> ComponentClass,
    TArray<UFactoryComponent*>& OutAttachedFactoryComponents, bool bIncludeChildren)
{
    for (auto AttachedComponent : AttachedFactoryComponents)
    {
        if (!AttachedComponent)
            return;
        if (AttachedComponent->IsA(ComponentClass))
            OutAttachedFactoryComponents.Add(AttachedComponent);
        if (bIncludeChildren)
            AttachedComponent->GetAllAttachedChildComponents(
                ComponentClass, OutAttachedFactoryComponents, bIncludeChildren);
    }
}

void UFactoryComponent::DetachFromOwner()
{
    if (!OwnerFactoryComponent_Internal)
    {
        UE_LOG(FactoryComponent, Error, TEXT("UFactoryComponent::DetachFromOwner: No owner to detach from"));
        return;
    }

    OwnerFactoryComponent_Internal->DetachChildComponent(this);
}

void UFactoryComponent::InitializeFactoryComponent(UPlanetWorld* PlanetWorld,
    FDataTableRowHandle const&                                   InitFactoryComponentDataTableHandle,
    UFactoryComponent*                                           OwnerFactoryComponent)
{
    if (!PlanetWorld)
    {
        UE_LOG(FactoryComponent, Error, TEXT("UFactoryComponent::InitializeFactoryComponent: PlanetWorld is null"));
        return;
    }
    if (RegisteredPlanetWorld)
    {
        UE_LOG(FactoryComponent, Error,
            TEXT("UFactoryComponent::InitializeFactoryComponent: Factory component is already initialized"));
        return;
    }
    if (InitFactoryComponentDataTableHandle.IsNull())
    {
        UE_LOG(FactoryComponent, Error,
            TEXT("UFactoryComponent::InitializeFactoryComponent: InitFactoryComponentDataTableHandle is null"));
        return;
    }

    RegisteredPlanetWorld = PlanetWorld;

    OnInitializeFactoryComponent(InitFactoryComponentDataTableHandle);
    OnPostInitializeFactoryComponent(InitFactoryComponentDataTableHandle);

    if (OwnerFactoryComponent)
        OwnerFactoryComponent->AttachChildComponent(this);
}

void UFactoryComponent::AttachedToOwner(UFactoryComponent* OwnerFactoryComponent)
{
    OwnerFactoryComponent_Internal = OwnerFactoryComponent;

    OnAttachedToOwner(OwnerFactoryComponent);
}

void UFactoryComponent::DetachedFromOwner(UFactoryComponent* OwnerFactoryComponent)
{
    OnDetachedFromOwner(OwnerFactoryComponent);

    OwnerFactoryComponent_Internal = nullptr;
}

void UFactoryComponent::TickComponent(float DeltaTime)
{
    OnTickComponent(DeltaTime);
}
