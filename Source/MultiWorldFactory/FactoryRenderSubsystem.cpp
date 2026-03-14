// Fill out your copyright notice in the Description page of Project Settings.

#include "FactoryRenderSubsystem.h"
#include "PlanetSimulationSubsystem.h"
#include "PlanetWorld.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "VisualMetadataSubsystem.h"
#include "RegistrationComponent.h"

void UFactoryRenderSubsystem::UpdateCurrentChunkPositon(FIntPoint const& NewChunkPosition)
{
    if (CurrentChunkPosition == NewChunkPosition)
        return;

    auto CurrentChunks   = UPlanetWorld::GetChunksInRadius(CurrentChunkPosition, ChunkLoadRadius);
    auto NewChunks       = UPlanetWorld::GetChunksInRadius(NewChunkPosition, ChunkLoadRadius);
    auto ChunksToUnload  = CurrentChunks.Difference(NewChunks);
    auto ChunksToLoad    = NewChunks.Difference(CurrentChunks);
    CurrentChunkPosition = NewChunkPosition;
}

void UFactoryRenderSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    auto PlanetSimulationSubsystem = UPlanetSimulationSubsystem::Get(&InWorld);
    if (!PlanetSimulationSubsystem)
        return;

    CurrentPlanetWorld = PlanetSimulationSubsystem->GetCurrentPlanetWorld();
    if (!CurrentPlanetWorld)
    {
        UE_LOG(LogTemp, Error, TEXT("UFactoryRenderSubsystem::OnWorldBeginPlay: CurrentPlanetWorld is null"));
        return;
    }

    CurrentPlanetWorld->OnRegisterRegistrationComponent.AddUniqueDynamic(
        this, &UFactoryRenderSubsystem::OnRegisterRegistrationComponent);
    CurrentPlanetWorld->OnUnregisterRegistrationComponent.AddUniqueDynamic(
        this, &UFactoryRenderSubsystem::OnUnregisterRegistrationComponent);
}

void UFactoryRenderSubsystem::OnRegisterRegistrationComponent(
    FIntPoint ChunkCoordinates, URegistrationComponent* RegistrationComponent)
{
    if (!RegistrationComponentsByChunk.Contains(ChunkCoordinates))
        return;

    CreateInstances(RegistrationComponent);
}

void UFactoryRenderSubsystem::OnUnregisterRegistrationComponent(
    FIntPoint ChunkCoordinates, URegistrationComponent* RegistrationComponent)
{
    if (!RegistrationComponentsByChunk.Contains(ChunkCoordinates))
        return;

    DestroyInstances(RegistrationComponent);
}

void UFactoryRenderSubsystem::LoadChunk(FIntPoint const& ChunkCoordinates)
{
    if (!CurrentPlanetWorld)
        return;

    auto RegistrationComponentsInChunk = CurrentPlanetWorld->GetRegistrationComponentsInChunkPtr(ChunkCoordinates);
    if (!RegistrationComponentsInChunk)
        return;

    for (auto RegistrationComponent : RegistrationComponentsInChunk->RegistrationComponents)
        CreateInstances(RegistrationComponent);
}

void UFactoryRenderSubsystem::UnloadChunk(FIntPoint const& ChunkCoordinates)
{
    if (!CurrentPlanetWorld)
        return;

    auto RegistrationComponentsInChunk = RegistrationComponentsByChunk.Find(ChunkCoordinates);
    if (!RegistrationComponentsInChunk)
        return;

    for (auto RegistrationComponent : RegistrationComponentsInChunk->RegistrationComponents)
        DestroyInstances(RegistrationComponent);
}

void UFactoryRenderSubsystem::CreateInstances(URegistrationComponent* RegistrationComponent)
{
    if (!RegistrationComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("UFactoryRenderSubsystem::CreateInstances: RegistrationComponent is null"));
        return;
    }

    auto VisualMetadataSubsystem = UVisualMetadataSubsystem::Get(this);
    if (!VisualMetadataSubsystem)
        return;

    auto& FMeshInstances = RegistrationComponentData.Add(RegistrationComponent);
    auto& VisualMetadata = VisualMetadataSubsystem->GetVisualMetadataV2(RegistrationComponent->GetVisualizationActorClass());

    for (auto& VisualMesh : VisualMetadata.VisualMeshes)
    {
        auto SMIC = MeshHashMap.FindOrAdd(VisualMesh.Key);
        if (!SMIC)
        {
            SMIC = NewObject<UInstancedStaticMeshComponent>(GetWorld());
            SMIC->SetStaticMesh(VisualMesh.Key.StaticMesh);
            for (int32 MaterialIndex = 0; MaterialIndex < VisualMesh.Key.Materials.Num(); ++MaterialIndex)
                SMIC->SetMaterial(MaterialIndex, VisualMesh.Key.Materials[MaterialIndex]);
            SMIC->RegisterComponent();
        }
        if (!SMIC)
        {
            UE_LOG(LogTemp, Error, TEXT("UFactoryRenderSubsystem::CreateInstances: Failed to create or find InstancedStaticMeshComponent for mesh hash"));
            continue;
        }
        auto& ArrIndexes = FMeshInstances.MeshHashMap.Add(VisualMesh.Key).InstancesIndexesMap.Add(SMIC).Indexes;

        for (auto& VisualMeshInstance : VisualMesh.Value.Arr)
        {
            auto Index = SMIC->AddInstance(VisualMeshInstance.Transform);
            ArrIndexes.Add(Index);
        }

    }

}

void UFactoryRenderSubsystem::DestroyInstances(URegistrationComponent* RegistrationComponent)
{
    if (!RegistrationComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("UFactoryRenderSubsystem::DestroyInstances: RegistrationComponent is null"));
        return;
    }

    auto Find = RegistrationComponentData.Find(RegistrationComponent);
    if (!Find)
    {
        UE_LOG(LogTemp, Error, TEXT("UFactoryRenderSubsystem::DestroyInstances: No instance data found for RegistrationComponent %s"),
            *RegistrationComponent->GetName());
        return;
    }

    for (auto& Elem : Find->MeshHashMap)
    {
        auto& InstancedMeshData = Elem.Value;
        for (auto& InstancedMeshElem : InstancedMeshData.InstancesIndexesMap)
        {
            auto InstancedMeshComponent = InstancedMeshElem.Key;
            if (InstancedMeshComponent)
                for (auto const& Index : InstancedMeshElem.Value.Indexes)
                    InstancedMeshComponent->RemoveInstance(Index);
        }
    }

    RegistrationComponentData.Remove(RegistrationComponent);
}