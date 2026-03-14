// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PlanetWorld.h"
#include "FactoryRenderSubsystem.generated.h"

class UPlanetWorld;
class UInstancedStaticMeshComponent;
struct FRegistrationComponentsInChunk;

USTRUCT(BlueprintType)
struct FMeshHash
{
    GENERATED_BODY()

    /**  */
    UPROPERTY(BlueprintReadOnly, Category = "Debug")
    UStaticMesh* StaticMesh = nullptr;

    /**  */
    UPROPERTY(BlueprintReadOnly, Category = "Debug")
    TArray<UMaterialInterface*> Materials;

    bool operator==(FMeshHash const& Other) const
    {
        if (StaticMesh != Other.StaticMesh)
            return false;
        if (Materials.Num() != Other.Materials.Num())
            return false;
        for (int32 Index = 0; Index < Materials.Num(); ++Index)
            if (Materials[Index] != Other.Materials[Index])
                return false;
        return true;
    }
};

FORCEINLINE uint32 GetTypeHash(FMeshHash const& Key)
{
    uint32 Hash = 0;
    Hash = HashCombine(Hash, GetTypeHash(Key.StaticMesh));
    for (auto Material : Key.Materials)
        Hash = HashCombine(Hash, GetTypeHash(Material));
    return Hash;
}

USTRUCT(BlueprintType)
struct FInstancesIndexes
{
    GENERATED_BODY()

    /**  */
    UPROPERTY(BlueprintReadOnly, Category = "Debug")
    TArray<int32> Indexes;
};

USTRUCT(BlueprintType)
struct FInstancedStaticMeshComponentData
{
    GENERATED_BODY()

    /**  */
    UPROPERTY(BlueprintReadOnly, Category = "Debug")
    TMap<UInstancedStaticMeshComponent*, FInstancesIndexes> InstancesIndexesMap;
};

USTRUCT(BlueprintType)
struct FMeshInstances
{
    GENERATED_BODY()

    /**  */
    UPROPERTY(BlueprintReadOnly, Category = "Debug")
    TMap<FMeshHash, FInstancedStaticMeshComponentData> MeshHashMap;
};

/**
 * 
 */
UCLASS(NotBlueprintable)
class MULTIWORLDFACTORY_API UFactoryRenderSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "FactoryRenderSubsystem")
    void UpdateCurrentChunkPositon(FIntPoint const& NewCurrentChunk);

protected:
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;

private:
    UFUNCTION()
    void OnRegisterRegistrationComponent(FIntPoint ChunkCoordinates, URegistrationComponent* RegistrationComponent);
    UFUNCTION()
    void OnUnregisterRegistrationComponent(FIntPoint ChunkCoordinates, URegistrationComponent* RegistrationComponent);

    void LoadChunk(FIntPoint const& ChunkCoordinates);
    void UnloadChunk(FIntPoint const& ChunkCoordinates);

    void CreateInstances(URegistrationComponent* RegistrationComponent);
    void DestroyInstances(URegistrationComponent* RegistrationComponent);

    /** Reference to the current planet world */
    UPROPERTY(BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
    UPlanetWorld* CurrentPlanetWorld;
    /** The radius around the current chunk to load */
    UPROPERTY(BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
    int32 ChunkLoadRadius = 10;
    /** The current chunk position */
    UPROPERTY(BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
    FIntPoint CurrentChunkPosition = FIntPoint::ZeroValue;
    /** Mapping of chunk coordinates to their registration components */
    UPROPERTY(BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
    TMap<FIntPoint, FRegistrationComponentsInChunk> RegistrationComponentsByChunk;
    /** Mapping of mesh hashes to their instanced static mesh components */
    UPROPERTY(BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
    TMap<FMeshHash, UInstancedStaticMeshComponent*> MeshHashMap;
    /**  */
    UPROPERTY(BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
    TMap<URegistrationComponent*, FMeshInstances> RegistrationComponentData;
};
