// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingRegistrationComponent.h"
#include "PlanetWorld.h"

TArray<FIntPoint> UBuildingRegistrationComponent::GetOccupiedBuildingGridCoordinates() const
{
    return UPlanetWorld::GetOccupiedBuildingGridCoordinates(GridCoordinates_Internal, BuildingSize_Internal);
}

void UBuildingRegistrationComponent::RegisterBuildingPosition(FIntPoint const& GridCoordinates)
{
    GridCoordinates_Internal = GridCoordinates;
    OnRegisterBuildingPosition();
}

void UBuildingRegistrationComponent::UnregisterBuildingPosition()
{
    OnUnregisterBuildingPosition();
    GridCoordinates_Internal = FIntPoint::ZeroValue;
}
