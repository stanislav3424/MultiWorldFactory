// Fill out your copyright notice in the Description page of Project Settings.

#include "RegistrationComponent.h"
#include "VisualizationComponent.h"
#include "VisualMetadataSubsystem.h"

void URegistrationComponent::Register(FIntPoint const& ChunkCoordinates)
{
    if (bIsRegistered)
    {
        UE_LOG(LogTemp, Error, TEXT("URegistrationComponent::Register: RegistrationComponent %s is already registered"),
            *GetName());
    }

    ChunkCoordinates_Internal = ChunkCoordinates;
    bIsRegistered = true;
}

void URegistrationComponent::Unregister()
{
    if (!bIsRegistered)
    {
        UE_LOG(LogTemp, Error, TEXT("URegistrationComponent::Unregister: RegistrationComponent %s is not registered"),
            *GetName());
    }
    ChunkCoordinates_Internal = FIntPoint::ZeroValue;
    bIsRegistered = false;
}
