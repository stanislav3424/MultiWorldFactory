// Fill out your copyright notice in the Description page of Project Settings.

#include "PlanetSimulationSubsystem.h"
#include "PlanetWorld.h"


UPlanetSimulationSubsystem* UPlanetSimulationSubsystem::Get(UObject* WorldContextObject)
{
    if (!GEngine)
        return nullptr;

    auto World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
    if (!World)
        return nullptr;

    auto GameInstance = World->GetGameInstance();
    if (!GameInstance)
        return nullptr;

    return GameInstance->GetSubsystem<UPlanetSimulationSubsystem>();
}