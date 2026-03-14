// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FactoryRenderSubsystem.h"
#include "VisualMetadataSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FVisualMeshInstance
{
    GENERATED_BODY()

    /** Transform of this mesh instance */
    UPROPERTY(BlueprintReadOnly, Category = "Debug")
    FTransform Transform = FTransform::Identity;

    /** Index into the MaterialSets array for this mesh */
    UPROPERTY(BlueprintReadOnly, Category = "Debug")
    int32 MaterialSetIndex = 0;
};

USTRUCT(BlueprintType)
struct FVisualMaterialSet
{
    GENERATED_BODY()

    /** Array of materials for this set */
    UPROPERTY(BlueprintReadOnly, Category = "Debug")
    TArray<UMaterialInterface*> Materials;
};

USTRUCT(BlueprintType)
struct FVisualMeshData
{
    GENERATED_BODY()

    /** Static mesh used for all instances in this group */
    UPROPERTY(BlueprintReadOnly, Category = "Debug")
    UStaticMesh* StaticMesh = nullptr;

    /** Unique material sets used across instances */
    UPROPERTY(BlueprintReadOnly, Category = "Debug")
    TArray<FVisualMaterialSet> MaterialSets;

    /** All instances of this mesh */
    UPROPERTY(BlueprintReadOnly, Category = "Debug")
    TArray<FVisualMeshInstance> Instances;
};

USTRUCT(BlueprintType)
struct FVisualMetadata
{
    GENERATED_BODY()

    /** Array of mesh data grouped by static mesh */
    UPROPERTY(BlueprintReadOnly, Category = "Debug")
    TArray<FVisualMeshData> VisualMeshData;
};

USTRUCT(BlueprintType)
struct FVisualMeshInstances
{
    GENERATED_BODY()

    /**  */
    UPROPERTY(BlueprintReadOnly, Category = "Debug")
    TArray<FVisualMeshInstance> Arr;
};

USTRUCT(BlueprintType)
struct FVisualMetadataV2
{
    GENERATED_BODY()

    /**  */
    UPROPERTY(BlueprintReadOnly, Category = "Debug")
    TMap<FMeshHash, FVisualMeshInstances> VisualMeshes;
};

/**
 * Subsystem responsible for managing visual metadata for actor classes.
 */
UCLASS()
class MULTIWORLDFACTORY_API UVisualMetadataSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    /** Static halper. Gets the instance of this subsystem from the specified world context object. */
    static UVisualMetadataSubsystem* Get(UObject* WorldContextObject);

    /** Gets the visual metadata associated with the specified actor class. */
    UFUNCTION(BlueprintCallable, Category = "Metadata")
    FVisualMetadata const& GetVisualMetadata(TSubclassOf<AActor> ActorClass);
    /** Gets the visual metadata associated with the specified actor class. */
    UFUNCTION(BlueprintCallable, Category = "Metadata")
    FVisualMetadataV2 const& GetVisualMetadataV2(TSubclassOf<AActor> ActorClass);

private:
    /** Creates and populates visual metadata for the specified actor class. */
    void CreateVisualMetadata(TSubclassOf<AActor> ActorClass, FVisualMetadata& OutMetadata);
    /** Map storing visual metadata for each actor class. */
    UPROPERTY(BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
    TMap<TSubclassOf<AActor>, FVisualMetadata> VisualMetadataMap;
};
