// Fill out your copyright notice in the Description page of Project Settings.


#include "Challenges/BlackHole/BlackHoleActor.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABlackHoleActor::ABlackHoleActor()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = MeshComp;

	SphereDestroyer = CreateDefaultSubobject<USphereComponent>(TEXT("SphereDestroyer"));
	SphereDestroyer->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereDestroyer->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereDestroyer->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereDestroyer->SetupAttachment(MeshComp);

	SphereAtractor = CreateDefaultSubobject<USphereComponent>(TEXT("SphereAtractor"));
	SphereAtractor->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereAtractor->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereAtractor->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereAtractor->SetupAttachment(MeshComp);
}

// Called when the game starts or when spawned
void ABlackHoleActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABlackHoleActor::Tick(float DeltaSeconds)
{
	TArray<UPrimitiveComponent*> atracted;
	SphereAtractor->GetOverlappingComponents(atracted);


	if (atracted.Num() > 0) {
		for (UPrimitiveComponent* it : atracted)
		{
			it->AddRadialForce(GetActorLocation(), SphereAtractor->GetScaledSphereRadius(), -3000, ERadialImpulseFalloff::RIF_Constant, true);
		}

		TArray<UPrimitiveComponent*> toDestroy;
		SphereDestroyer->GetOverlappingComponents(toDestroy);
		//if (GEngine)
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("yolo"));

		if (toDestroy.Num() > 0) {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("wololo " + toDestroy.Num()));

			for (UPrimitiveComponent* it : toDestroy)
			{
				it->DestroyComponent();
			}
		}
	}
}

/*void ABlackHoleActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	Destroy();
}*/

