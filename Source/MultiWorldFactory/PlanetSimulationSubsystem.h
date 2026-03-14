// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PlanetSimulationSubsystem.generated.h"

class UPlanetWorld;

/**
 * Subsystem responsible for managing the simulation of planet worlds. It keeps track of all planet worlds in the game
 * instance and provides functionality for updating their simulations.
 */
UCLASS(NotBlueprintable)
class MULTIWORLDFACTORY_API UPlanetSimulationSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    /** Static halper. Gets the instance of this subsystem from the specified world context object. */
    static UPlanetSimulationSubsystem* Get(UObject* WorldContextObject);
    /** Gets the current planet world. */
    UFUNCTION(BlueprintCallable, Category = "Planet Simulation")
    UPlanetWorld* GetCurrentPlanetWorld() { return CurrentPlanetWorld; };

private:
    /**  */
    UPROPERTY(BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
    TArray<UPlanetWorld*> PlanetWorlds;
    /**  */
    UPROPERTY(BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
    UPlanetWorld* CurrentPlanetWorld;
};
