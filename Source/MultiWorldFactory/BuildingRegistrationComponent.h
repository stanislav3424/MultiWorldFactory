// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RegistrationComponent.h"
#include "BuildingRegistrationComponent.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable)
class MULTIWORLDFACTORY_API UBuildingRegistrationComponent : public URegistrationComponent
{
    GENERATED_BODY()

    friend class UPlanetWorld;

public:
    /** Gets the grid coordinates occupied by the building. */
    UFUNCTION(BlueprintCallable, Category = "BuildingRegistrationComponent")
    TArray<FIntPoint> GetOccupiedBuildingGridCoordinates() const;
    /** Gets the top-left grid coordinates of the building. */
    UFUNCTION(BlueprintCallable, Category = "BuildingRegistrationComponent")
    FIntPoint const& GetBuildingGridCoordinates() const { return GridCoordinates_Internal; }
    /** Gets the size of the building in grid units. */
    UFUNCTION(BlueprintCallable, Category = "BuildingRegistrationComponent")
    FIntPoint const& GetBuildingSize() const { return BuildingSize_Internal; }

protected:
    virtual void OnRegisterBuildingPosition() {};
    virtual void OnUnregisterBuildingPosition() {};

private:
    void RegisterBuildingPosition(FIntPoint const& GridCoordinates);
    void UnregisterBuildingPosition();
    /** The top-left grid coordinates of the building. */
    UPROPERTY(BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
    FIntPoint GridCoordinates_Internal = FIntPoint::ZeroValue;
    /** The size of the building in grid units. */
    UPROPERTY(BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
    FIntPoint BuildingSize_Internal = FIntPoint::ZeroValue;
};
