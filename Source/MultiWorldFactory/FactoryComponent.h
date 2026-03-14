// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FactoryComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(FactoryComponent, Log, All);

class UPlanetWorld;

/**
 * Base class for all factory components. Factory components can be attached to each other to form a hierarchy.
 */
UCLASS(NotBlueprintable)
class MULTIWORLDFACTORY_API UFactoryComponent : public UObject
{
    GENERATED_BODY()

    friend class UPlanetWorld;
    friend class UFactorySubComponent;

public:
    // BlueprintCallable functions for managing attached factory components

    /** Attaches another factory component to this component. */
    UFUNCTION(BlueprintCallable, Category = "FactoryComponent")
    void AttachChildComponent(UFactoryComponent* OtherFactoryComponent);
    /** Detaches a previously attached factory component from this component. */
    UFUNCTION(BlueprintCallable, Category = "FactoryComponent")
    void DetachChildComponent(UFactoryComponent* OtherFactoryComponent);
    /** Gets a previously attached factory component of the specified class. */
    UFUNCTION(BlueprintCallable, Category = "FactoryComponent")
    UFactoryComponent* GetAttachedChildComponent(
        TSubclassOf<UFactoryComponent> ComponentClass, bool bIncludeChildren = false);
    /** Gets all previously attached factory components of the specified class. */
    UFUNCTION(BlueprintCallable, Category = "FactoryComponent")
    void GetAllAttachedChildComponents(TSubclassOf<UFactoryComponent> ComponentClass,
        TArray<UFactoryComponent*>& OutAttachedFactoryComponents, bool bIncludeChildren = false);
    /** Checks if this factory component is attached to the specified factory component. */
    UFUNCTION(BlueprintCallable, Category = "FactoryComponent")
    bool IsAttachedToFactoryComponent(UFactoryComponent* OtherFactoryComponent) const
    {
        return AttachedFactoryComponents.Contains(OtherFactoryComponent);
    }
    /** Checks if this factory component is attached to any other factory component. */
    UFUNCTION(BlueprintCallable, Category = "FactoryComponent")
    bool IsAttached() const { return OwnerFactoryComponent_Internal != nullptr; }
    /** Checks if this factory component is a head factory component. */
    UFUNCTION(BlueprintCallable, Category = "FactoryComponent")
    bool IsHead() const { return OwnerFactoryComponent_Internal == nullptr; }
    /** Detaches this factory component from its owner. */
    UFUNCTION(BlueprintCallable, Category = "FactoryComponent")
    void DetachFromOwner();

    // Cpp functions for managing attached factory components

    /** Gets a previously attached factory component of the specified class. */
    template <typename TypeFactoryComponent>
    TypeFactoryComponent* GetAttachedChildComponent(bool bIncludeChildren = false)
    {
        auto Class = TypeFactoryComponent::StaticClass();
        return Cast<TypeFactoryComponent>(GetAttachedChildComponent(Class, bIncludeChildren));
    }
    /** Gets all previously attached factory components of the specified class. */
    template <typename TypeFactoryComponent>
    void GetAllAttachedChildComponents(
        TArray<TypeFactoryComponent*>& OutAttachedFactoryComponents, bool bIncludeChildren = false)
    {
        for (auto AttachedComponent : AttachedFactoryComponents)
            if (auto CastedComponent = Cast<TypeFactoryComponent>(AttachedComponent))
            {
                OutAttachedFactoryComponents.Add(CastedComponent);
                if (bIncludeChildren)
                    CastedComponent->GetAllAttachedChildComponents(OutAttachedFactoryComponents, bIncludeChildren);
            }
    }

protected:
    /**
     * Called when this factory component is initialized by the planet world. The data table row handle used for
     * initialization is passed in as a parameter.
     */
    virtual void OnInitializeFactoryComponent(FDataTableRowHandle const& InitFactoryComponentDataTableHandle) {};
    /**
     * Called after this factory component has been initialized by the planet world. The data table row handle used for
     * initialization is passed in as a parameter.
     */
    virtual void OnPostInitializeFactoryComponent(FDataTableRowHandle const& InitFactoryComponentDataTableHandle) {};
    /**
     * Called when this factory component is attached to an owner factory component.
     */
    virtual void OnAttachedToOwner(UFactoryComponent* OwnerFactoryComponent) {};
    /**
     * Called when this factory component is detached from an owner factory component.
     */
    virtual void OnDetachedFromOwner(UFactoryComponent* OwnerFactoryComponent) {};
    /**
     * Called every tick for this factory component.
     */
    virtual void OnTickComponent(float DeltaTime) {};

private:
    virtual void InitializeFactoryComponent(UPlanetWorld* PlanetWorld,
        FDataTableRowHandle const&                        InitFactoryComponentDataTableHandle,
        UFactoryComponent*                                OwnerFactoryComponent = nullptr);
    void         AttachedToOwner(UFactoryComponent* OwnerFactoryComponent);
    void         DetachedFromOwner(UFactoryComponent* OwnerFactoryComponent);
    void         TickComponent(float DeltaTime);

    /** The data table row handle used to initialize this factory component. */
    UPROPERTY(BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
    FDataTableRowHandle FactoryComponentDataTableRowHandle;
    /** The owner factory component to which this component is attached. */
    UPROPERTY(BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
    UFactoryComponent* OwnerFactoryComponent_Internal;
    /** All factory components attached to this component. */
    UPROPERTY(BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
    TSet<UFactoryComponent*> AttachedFactoryComponents;
    /** The planet world to which this factory component is registered. */
    UPROPERTY(BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
    UPlanetWorld* RegisteredPlanetWorld;
};
