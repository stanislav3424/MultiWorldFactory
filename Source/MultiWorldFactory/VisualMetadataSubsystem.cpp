// Fill out your copyright notice in the Description page of Project Settings.

#include "VisualMetadataSubsystem.h"

UVisualMetadataSubsystem* UVisualMetadataSubsystem::Get(UObject* WorldContextObject)
{
    if (!GEngine)
        return nullptr;

    auto World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
    if (!World)
        return nullptr;

    auto GameInstance = World->GetGameInstance();
    if (!GameInstance)
        return nullptr;

    return GameInstance->GetSubsystem<UVisualMetadataSubsystem>();
}

FVisualMetadata const& UVisualMetadataSubsystem::GetVisualMetadata(TSubclassOf<AActor> ActorClass)
{
    if (!ActorClass)
    {
        UE_LOG(LogTemp, Error, TEXT("UVisualMetadataSubsystem::GetVisualMetadata: ActorClass is null"));
        static FVisualMetadata EmptyMetadata;
        return EmptyMetadata;
    }

    auto FoundMetadata = VisualMetadataMap.Find(ActorClass);
    if (FoundMetadata)
        return *FoundMetadata;

    auto& NewMetadata = VisualMetadataMap.Add(ActorClass);
    CreateVisualMetadata(ActorClass, NewMetadata);

    return NewMetadata;
}

void UVisualMetadataSubsystem::CreateVisualMetadata(TSubclassOf<AActor> ActorClass, FVisualMetadata& OutMetadata)
{
    if (!ActorClass)
        return;

    auto World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("UVisualMetadataSubsystem::CreateVisualMetadata: Failed to get world context"));
        return;
    }

    auto TemporaryActor = World->SpawnActor<AActor>(ActorClass);
    if (!TemporaryActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("UVisualMetadataSubsystem::CreateVisualMetadata: Failed to spawn temporary actor of class %s"), *ActorClass->GetName());
        return;
    }

    // Map to group instances by mesh and material set
    TMap<UStaticMesh*, TMap<TArray<UMaterialInterface*>, FVisualMeshData*>> MeshDataMap;

    TArray<UStaticMeshComponent*> StaticMeshComponents;
    TemporaryActor->GetComponents<UStaticMeshComponent>(StaticMeshComponents);

    for (auto MeshComponent : StaticMeshComponents)
    {
        if (!MeshComponent || !MeshComponent->GetStaticMesh())
            continue;

        auto StaticMesh = MeshComponent->GetStaticMesh();

        // Collect materials from this component
        TArray<UMaterialInterface*> ComponentMaterials;
        for (int32 Index = 0; Index < MeshComponent->GetNumMaterials(); ++Index)
            if (auto Material = MeshComponent->GetMaterial(Index))
                ComponentMaterials.Add(Material);

        // Find or create mesh data entry
        auto& MeshEntries = MeshDataMap.FindOrAdd(StaticMesh);
        FVisualMeshData* MeshData = nullptr;

        // Check if we already have this material set combination
        if (auto* ExistingMeshData = MeshEntries.Find(ComponentMaterials))
        {
            MeshData = *ExistingMeshData;
        }
        else
        {
            // Create new mesh data
            FVisualMeshData NewMeshData;
            NewMeshData.StaticMesh = StaticMesh;

            // Add the material set
            FVisualMaterialSet MaterialSet;
            MaterialSet.Materials = ComponentMaterials;
            NewMeshData.MaterialSets.Add(MaterialSet);

            MeshData = &OutMetadata.VisualMeshData.AddDefaulted_GetRef();
            *MeshData = NewMeshData;
            MeshEntries.Add(ComponentMaterials, MeshData);
        }

        // Add instance to mesh data
        FVisualMeshInstance Instance;
        Instance.Transform = MeshComponent->GetComponentTransform();
        Instance.MaterialSetIndex = MeshData->MaterialSets.Num() - 1;
        MeshData->Instances.Add(Instance);
    }

    TemporaryActor->Destroy();
}