// Fill out your copyright notice in the Description page of Project Settings.

#include "PlanetWorld.h"
#include "FactoryComponent.h"
#include "RegistrationComponent.h"
#include "ItemRegistrationComponent.h"
#include "BuildingRegistrationComponent.h"

int32 UPlanetWorld::ChunkSize = 10;
float UPlanetWorld::GridSize = 100.f;

UFactoryComponent* UPlanetWorld::CreateFactoryComponent(
    FDataTableRowHandle const& InitFactoryComponentDataTableHandle, UFactoryComponent* OwnerFactoryComponent)
{
    auto NewFactoryComponent = NewObject<UFactoryComponent>(this);
    if (!NewFactoryComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("UPlanetWorld::CreateFactoryComponent: Failed to create UFactoryComponent"));
        return nullptr;
    }
    NewFactoryComponent->InitializeFactoryComponent(this, InitFactoryComponentDataTableHandle, OwnerFactoryComponent);
    AllCreatedFactoryComponents.Add(NewFactoryComponent);
    return NewFactoryComponent;
}

UItemRegistrationComponent* UPlanetWorld::RegisterItemInWorld(UFactoryComponent* HeadFactoryComponent, FVector const& WorldCoordinates)
{
    if (!HeadFactoryComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("UPlanetWorld::RegisterBuildingInWorld: HeadFactoryComponent is null"));
        return nullptr;
    }
    auto ItemRegistrationComponent = HeadFactoryComponent->GetAttachedChildComponent<UItemRegistrationComponent>();
    if (!ItemRegistrationComponent)
    {
        UE_LOG(LogTemp, Error,
            TEXT("UPlanetWorld::RegisterItemInWorld: Failed to get UItemRegistrationComponent from "
                 "HeadFactoryComponent"));
        return nullptr;
    }
    if (!CanRegister(ItemRegistrationComponent))
        return nullptr;
    RegisterItem(ItemRegistrationComponent, WorldCoordinates);
    return ItemRegistrationComponent;
}

UBuildingRegistrationComponent* UPlanetWorld::RegisterBuildingInWorld(
    UFactoryComponent* HeadFactoryComponent, FIntPoint const& GridCoordinates)
{
    if (!HeadFactoryComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("UPlanetWorld::RegisterBuildingInWorld: HeadFactoryComponent is null"));
        return nullptr;
    }
    auto BuildingRegistrationComponent = HeadFactoryComponent->GetAttachedChildComponent<UBuildingRegistrationComponent>();
    if (!BuildingRegistrationComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("UPlanetWorld::RegisterBuildingInWorld: Failed to get UBuildingRegistrationComponent from HeadFactoryComponent"));
        return nullptr;
    }
    if (!CanRegister(BuildingRegistrationComponent))
        return nullptr;

    if (!CanPlaceBuildingInWorld(GridCoordinates, BuildingRegistrationComponent->GetBuildingSize()))
    {
        UE_LOG(LogTemp, Error,
            TEXT("UPlanetWorld::RegisterBuildingInWorld: Cannot place building at GridCoordinates (%d, %d) due to collision"),
            GridCoordinates.X, GridCoordinates.Y);
        return nullptr;
    }

    RegisterBuilding(BuildingRegistrationComponent, GridCoordinates);
    return BuildingRegistrationComponent;
}

bool UPlanetWorld::CanPlaceBuildingInWorld(FIntPoint const& GridCoordinates, FIntPoint const& BuildingSize)
{
    for (auto& BuildingGridCoordinate : GetOccupiedBuildingGridCoordinates(GridCoordinates, BuildingSize))
        if (BuildingRegistrationComponentsByGrid.Contains(BuildingGridCoordinate))
            return false;
    return true;
}

TArray<FIntPoint> UPlanetWorld::GetOccupiedBuildingGridCoordinates(
    FIntPoint const& GridCoordinates, FIntPoint const& BuildingSize)
{
    TArray<FIntPoint> GridCoordinatesArray;
    for (int32 X = 0; X < BuildingSize.X; ++X)
        for (int32 Y = 0; Y < BuildingSize.Y; ++Y)
            GridCoordinatesArray.Add(FIntPoint(GridCoordinates.X + X, GridCoordinates.Y + Y));
    return GridCoordinatesArray;
}

TSet<FIntPoint> UPlanetWorld::GetChunksInRadius(const FIntPoint& CenterChunk, int32 Radius)
{
    TSet<FIntPoint> ChunkInRadius;
    const int32     R2 = Radius * Radius;
    for (int32 X = CenterChunk.X - Radius; X <= CenterChunk.X + Radius; ++X)
        for (int32 Y = CenterChunk.Y - Radius; Y <= CenterChunk.Y + Radius; ++Y)
        {
            const int32 DX = X - CenterChunk.X;
            const int32 DY = Y - CenterChunk.Y;

            if (DX * DX + DY * DY <= R2)
                ChunkInRadius.Add(FIntPoint(X, Y));
        }
    return ChunkInRadius;
}

bool UPlanetWorld::CanRegister(URegistrationComponent* RegistrationComponent)
{
    if (!RegistrationComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("UPlanetWorld::CanRegister: RegistrationComponent is null"));
        return false;
    }
    if (RegistrationComponent->IsRegistered())
    {
        UE_LOG(LogTemp, Error, TEXT("UPlanetWorld::CanRegister: RegistrationComponent %s is already registered"),
            *RegistrationComponent->GetName());
        return false;
    }
    if (RegistrationComponent->IsHead())
    {
        UE_LOG(LogTemp, Error, TEXT("UPlanetWorld::CanRegister: RegistrationComponent %s is a head component"),
            *RegistrationComponent->GetName());
        return false;
    }
    if (AllRegistrationComponents.Contains(RegistrationComponent))
    {
        UE_LOG(LogTemp, Error,
            TEXT("UPlanetWorld::CanRegister: RegistrationComponent %s is already registered"),
            *RegistrationComponent->GetName());
        return false;
    }

    return true;
}

void UPlanetWorld::RegisterInChunk(URegistrationComponent* RegistrationComponent, FIntPoint const& ChunkCoordinates)
{
    if (!RegistrationComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("UPlanetWorld::RegisterInChunk: RegistrationComponent is null"));
        return ;
    }

    AllRegistrationComponents.Add(RegistrationComponent);
    RegistrationComponentsInChunkMap.FindOrAdd(ChunkCoordinates).RegistrationComponents.Add(RegistrationComponent);
    RegistrationComponent->Register(ChunkCoordinates);

    OnRegisterRegistrationComponent.Broadcast(ChunkCoordinates, RegistrationComponent);
    return;
}

void UPlanetWorld::UnregisterInChunk(URegistrationComponent* RegistrationComponent)
{
    if (!RegistrationComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("UPlanetWorld::UnregisterRegistrationComponent: RegistrationComponent is null"));
        return;
    }

    auto& ChunkCoordinates = RegistrationComponent->GetChunkCoordinates();

    RegistrationComponent->Unregister();
    RegistrationComponentsInChunkMap.FindOrAdd(ChunkCoordinates)
        .RegistrationComponents.Remove(RegistrationComponent);
    AllRegistrationComponents.Remove(RegistrationComponent);

    OnUnregisterRegistrationComponent.Broadcast(ChunkCoordinates, RegistrationComponent);
    return;
}

void UPlanetWorld::RegisterItem(UItemRegistrationComponent* ItemRegistrationComponent, FVector const& WorldCoordinates)
{
    RegisterInChunk(ItemRegistrationComponent, GetChunkFromWorld(WorldCoordinates));

}

void UPlanetWorld::UnregisterItem(UItemRegistrationComponent* ItemRegistrationComponent)
{
    UnregisterInChunk(ItemRegistrationComponent);
}

void UPlanetWorld::RegisterBuilding(
    UBuildingRegistrationComponent* BuildingRegistrationComponent, FIntPoint const& GridCoordinates)
{
    RegisterInChunk(BuildingRegistrationComponent, GetChunkFromGrid(GridCoordinates));

    BuildingRegistrationComponent->RegisterBuildingPosition(GridCoordinates);
    auto BuildingGridCoordinates = BuildingRegistrationComponent->GetOccupiedBuildingGridCoordinates();
    for (auto& GridCoordinate : BuildingGridCoordinates)
        BuildingRegistrationComponentsByGrid.Add(GridCoordinate, BuildingRegistrationComponent);
}

void UPlanetWorld::UnregisterBuilding(UBuildingRegistrationComponent* BuildingRegistrationComponent)
{
    UnregisterInChunk(BuildingRegistrationComponent);

    auto BuildingGridCoordinates = BuildingRegistrationComponent->GetOccupiedBuildingGridCoordinates();
    for (auto& GridCoordinate : BuildingGridCoordinates)
        BuildingRegistrationComponentsByGrid.Remove(GridCoordinate);
    BuildingRegistrationComponent->UnregisterBuildingPosition();
}

void UPlanetWorld::WorldTick(float DeltaTime)
{
}
