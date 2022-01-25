// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlackHoleActor.generated.h"

class USphereComponent;

UCLASS()
class FPSGAME_API ABlackHoleActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlackHoleActor();

protected:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereDestroyer;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereAtractor;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaSeconds) override;

	//virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
