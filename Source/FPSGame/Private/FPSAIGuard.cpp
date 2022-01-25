// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSAIGuard.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "FPSGameMode.h"
#include "AIController.h"
#include "Engine/TargetPoint.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AFPSAIGuard::AFPSAIGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));

	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnPawnSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnNoiseHeard);

	GuardState = EAIState::Idle;

	CurrentWaypoint = -1;
}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();
	
	OriginalRotation = GetActorRotation();
	TargetRotation = OriginalRotation;

	CurrentPos = GetActorLocation();
	PreviousPos = CurrentPos;
}

void AFPSAIGuard::OnPawnSeen(APawn* SeenPawn)
{
	if (SeenPawn == nullptr) return;

	DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.0f, 12, FColor::Red, false, 10.0f);

	AFPSGameMode* GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->CompleteMission(SeenPawn, false);
	}

	SetGuardState(EAIState::Alerted);
}

void AFPSAIGuard::OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	if (GuardState == EAIState::Alerted) return;

	DrawDebugSphere(GetWorld(), Location, 32.0f, 12, FColor::Green, false, 10.0f);

	FVector Direction = Location - GetActorLocation();
	Direction.Normalize();

	TargetRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
	TargetRotation.Pitch = 0.0f;
	TargetRotation.Roll = 0.0f;

	GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientaton);
	GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientaton, this, &AFPSAIGuard::ResetOrientation, 3.0f);

	SetGuardState(EAIState::Suspicious);
}

void AFPSAIGuard::ResetOrientation()
{
	if (GuardState == EAIState::Alerted) return;

	TargetRotation = OriginalRotation;

	SetGuardState(EAIState::Idle);
}

void AFPSAIGuard::OnRep_GuardState()
{
	OnStateChanged(GuardState);
}

void AFPSAIGuard::SetGuardState(EAIState NewState)
{
	if (GuardState == NewState) return;

	GuardState = NewState;

	OnRep_GuardState();

	if (GuardState == EAIState::Idle)
		MoveToNextWaypoint();
	else {
		auto AIController = Cast<AAIController>(GetController());

		if (!AIController) return;
		AIController->StopMovement();
		--CurrentWaypoint;
	}
}

void AFPSAIGuard::MoveToNextWaypoint()
{
	if (GuardState != EAIState::Idle || Waypoints.Num() == 0) return;

	auto AIController = Cast<AAIController>(GetController());

	if(!AIController) return;

	++CurrentWaypoint;

	if (!Waypoints.IsValidIndex(CurrentWaypoint))
		CurrentWaypoint = 0;

	AIController->MoveToActor(Waypoints[CurrentWaypoint]);
}

void AFPSAIGuard::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ATargetPoint* targetPoint = Cast<ATargetPoint>(OtherActor);
	if (targetPoint != nullptr) {
		MoveToNextWaypoint();
	}
}

// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentPos = GetActorLocation();

	if(CurrentPos == PreviousPos)
		MoveToNextWaypoint();

	PreviousPos = CurrentPos;

	SetActorRotation(FMath::RInterpConstantTo(GetActorRotation(), TargetRotation, FApp::GetDeltaTime(), 100.0f));
}

void AFPSAIGuard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSAIGuard, GuardState);
	DOREPLIFETIME(AFPSAIGuard, TargetRotation);
}