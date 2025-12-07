// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "LobbyPawn.generated.h"

/**
 * 
 */
UCLASS()
class MATCHSYSTEM_API ALobbyPawn : public ASpectatorPawn
{
	GENERATED_BODY()
	
public:
	ALobbyPawn();

protected:
	// 로비용 캐릭터 외형 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* CharacterMesh;
	
	
};
