// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PlanetWorld.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnRegisterRegistrationComponent, FIntPoint, ChunkCoordinates, URegistrationComponent*, RegistrationComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnUnregisterRegistrationComponent, FIntPoint, ChunkCoordinates, URegistrationComponent*, RegistrationComponent);

class UFactoryComponent;
class URegistrationComponent;
class UItemRegistrationComponent;
class UBuildingRegistrationComponent;

USTRUCT(BlueprintType)
struct FRegistrationComponentsInChunk
{
    GENERATED_BODY()

    /**  */
    UPROPERTY(BlueprintReadOnly, Category = "Debug")
    TSet<URegistrationComponent*> RegistrationComponents;
};

/**
 * Class representing a planet world.
 */
UCLASS(NotBlueprintable)
class MULTIWORLDFACTORY_API UPlanetWorld : public UObject
{
    GENERATED_BODY()

public:
    /** Size of a chunk in grid units. */
    static int32 ChunkSize;
    /** Size of a grid unit in world units. */
    static float GridSize;

    /**
     * Creates a factory component and initializes it with the specified data table row handle. The created factory
     * component is owned by this planet world.
     */
    UFUNCTION(BlueprintCallable, Category = "PlanetWorld")
    UFactoryComponent* CreateFactoryComponent(FDataTableRowHandle const& InitFactoryComponentDataTableHandle,
        UFactoryComponent*                                               OwnerFactoryComponent = nullptr);
    /** Registers an item in the world. */
    UFUNCTION(BlueprintCallable, Category = "PlanetWorld")
    UItemRegistrationComponent* RegisterItemInWorld(
        UFactoryComponent* HeadFactoryComponent, FVector const& WorldCoordinates);
    /** Registers a building in the world. */
    UFUNCTION(BlueprintCallable, Category = "PlanetWorld")
    UBuildingRegistrationComponent* RegisterBuildingInWorld(
        UFactoryComponent* HeadFactoryComponent, FIntPoint const& GridCoordinates);
    /** Checks if a building can be placed at the specified grid coordinates with the given size. */
    UFUNCTION(BlueprintCallable, Category = "PlanetWorld")
    bool CanPlaceBuildingInWorld(FIntPoint const& GridCoordinates, FIntPoint const& BuildingSize);


    /** Converts grid coordinates to chunk coordinates. */
    UFUNCTION(BlueprintCallable, Category = "PlanetWorld")
    static FIntPoint GetChunkFromGrid(FIntPoint const& GridCoordinates)
    {
        return FIntPoint(FMath::FloorToInt(static_cast<float>(GridCoordinates.X) / ChunkSize),
            FMath::FloorToInt(static_cast<float>(GridCoordinates.Y) / ChunkSize));
    }
    /** Converts world coordinates to grid coordinates. */
    UFUNCTION(BlueprintCallable, Category = "PlanetWorld")
    static FIntPoint GetGridFromWorld(FVector const& WorldCoordinates)
    {
        return FIntPoint(FMath::FloorToInt(WorldCoordinates.X / GridSize),
            FMath::FloorToInt(WorldCoordinates.Y / GridSize));
    }
    /** Converts world coordinates to chunk coordinates. */
    UFUNCTION(BlueprintCallable, Category = "PlanetWorld")
    static FIntPoint GetChunkFromWorld(FVector const& WorldCoordinates)
    {
        return GetChunkFromGrid(GetGridFromWorld(WorldCoordinates));
    }
    /** Gets the grid coordinates occupied by a building given its top-left grid coordinates and size. */
    UFUNCTION(BlueprintCallable, Category = "PlanetWorld")
    static TArray<FIntPoint> GetOccupiedBuildingGridCoordinates(
        FIntPoint const& GridCoordinates, FIntPoint const& BuildingSize);
    /** Gets the chunk coordinates within a specified radius from a center chunk. */
    UFUNCTION(BlueprintCallable, Category = "PlanetWorld")
    static TSet<FIntPoint> GetChunksInRadius(const FIntPoint& CenterChunk, int32 Radius);

    /** Internal function for non-UFunction callers to access chunk data */
    FRegistrationComponentsInChunk const* GetRegistrationComponentsInChunkPtr(FIntPoint const& ChunkCoordinates) const
    {
        return RegistrationComponentsInChunkMap.Find(ChunkCoordinates);
    }

    /** Delegate called when a registration component is registered. */
    UPROPERTY(BlueprintAssignable, Category = "PlanetWorld")
    FOnRegisterRegistrationComponent OnRegisterRegistrationComponent;
    /** Delegate called when a registration component is unregistered. */
    UPROPERTY(BlueprintAssignable, Category = "PlanetWorld")
    FOnUnregisterRegistrationComponent OnUnregisterRegistrationComponent;

private:
    bool CanRegister(URegistrationComponent* RegistrationComponent);
    void RegisterInChunk(URegistrationComponent* RegistrationComponent, FIntPoint const& ChunkCoordinates);
    void UnregisterInChunk(URegistrationComponent* RegistrationComponent);
    void RegisterItem(UItemRegistrationComponent* ItemRegistrationComponent, FVector const& WorldCoordinates);
    void UnregisterItem(UItemRegistrationComponent* ItemRegistrationComponent);
    void RegisterBuilding(
        UBuildingRegistrationComponent* BuildingRegistrationComponent, FIntPoint const& GridCoordinates);
    void UnregisterBuilding(UBuildingRegistrationComponent* BuildingRegistrationComponent);

    void WorldTick(float DeltaTime);

    /** All factory components created by this planet world. */
    UPROPERTY(BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
    TSet<UFactoryComponent*> AllCreatedFactoryComponents;
    /** All registration components in the world. */
    UPROPERTY(BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
    TSet<URegistrationComponent*> AllRegistrationComponents;
    /** A map of registration components grouped by their chunk coordinates. */
    UPROPERTY(BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
    TMap<FIntPoint, FRegistrationComponentsInChunk> RegistrationComponentsInChunkMap;
    /** A map of building registration components keyed by their grid coordinates. */
    UPROPERTY(BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
    TMap<FIntPoint, UBuildingRegistrationComponent*> BuildingRegistrationComponentsByGrid;
};
