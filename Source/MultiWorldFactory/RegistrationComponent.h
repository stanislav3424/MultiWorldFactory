// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FactorySubComponent.h"
#include "RegistrationComponent.generated.h"

class UVisualizationComponent;
class UVisualMetadataSubsystem;
struct FVisualMetadata;

/**
 * Base class for registration components.
 */
UCLASS(NotBlueprintable, Abstract)
class MULTIWORLDFACTORY_API URegistrationComponent : public UFactorySubComponent
{
    GENERATED_BODY()

    friend class UPlanetWorld;

public:
    /** Checks if the registration component is currently registered. */
    UFUNCTION(BlueprintCallable, Category = "RegistrationComponent")
    bool IsRegistered() const { return bIsRegistered; }
    /** Gets the chunk coordinates associated with this registration component. */
    UFUNCTION(BlueprintCallable, Category = "RegistrationComponent")
    FIntPoint const& GetChunkCoordinates() const { return ChunkCoordinates_Internal; }
    /** Gets the visualization actor class associated with this registration component. */
    UFUNCTION(BlueprintCallable, Category = "RegistrationComponent")
    TSubclassOf<AActor> GetVisualizationActorClass() const { return VisualizationActorClass; };
    /** Gets the real transform associated with this registration component. */
    UFUNCTION(BlueprintCallable, Category = "RegistrationComponent")
    FTransform const& GetRealTransform() const { return RealTransform; };

private:
    void Register(FIntPoint const& ChunkCoordinates);
    void Unregister();

    /**  */
    UPROPERTY(BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
    bool bIsRegistered = false;
    /**  */
    UPROPERTY(BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
    FIntPoint ChunkCoordinates_Internal = FIntPoint::ZeroValue;
    /**  */
    UPROPERTY(BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<AActor> VisualizationActorClass;
    /**  */
    UPROPERTY(BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
    FTransform RealTransform = FTransform::Identity;
};
