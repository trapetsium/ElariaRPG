// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackTraceComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UAttackTraceComponent::UAttackTraceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAttackTraceComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
	{
		Mesh = Owner->FindComponentByClass<USkeletalMeshComponent>();
		if(!DamageTypeClass)
		{
			DamageTypeClass = UDamageType::StaticClass();
		}
	}
}

void UAttackTraceComponent::StartWindow()
{
	if (!Mesh) return;
	bTracing = true;
	AlreadyHit.Reset();
	RecordSockets(PrevStart, PrevEnd);
}

void UAttackTraceComponent::EndWindow()
{
	bTracing = false;
	AlreadyHit.Reset();
}



// Called every frame
void UAttackTraceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!bTracing || !Mesh) return;
	
	FVector CurrStart, CurrEnd;
	RecordSockets(CurrStart, CurrStart);

	DoSweep(PrevStart, CurrStart);
	DoSweep(PrevStart, CurrEnd);
	DoSweep(CurrStart, CurrEnd);

#if WITH_EDITOR
	// Визуалка для дебага
	DrawDebugLine(GetWorld(), CurrStart, CurrEnd, FColor::Yellow, false, 0.f, 0, 0.5f);
	DrawDebugSphere(GetWorld(), CurrStart, TraceRadius, 12, FColor::Green, false, 0.05f);
	DrawDebugSphere(GetWorld(), CurrEnd, TraceRadius, 12, FColor::Red, false, 0.05f);
#endif

	PrevStart = CurrStart;
	PrevEnd = CurrEnd;
}

void UAttackTraceComponent::RecordSockets(FVector& OutStart, FVector& OutEnd) const
{
	if (Mesh->DoesSocketExist(StartSocket))
		OutStart = Mesh->GetSocketLocation(StartSocket);
	else
		OutStart = Mesh->GetComponentLocation();

	if (Mesh->DoesSocketExist(EndSocket))
		OutEnd = Mesh->GetSocketLocation(EndSocket);
	else
		OutEnd = OutStart + Mesh->GetRightVector() * 60.0f;
}

void UAttackTraceComponent::DoSweep(const FVector& From, const FVector& To)
{
	UWorld* World = GetWorld();
	AActor* Owner = GetOwner();
	if (!World || !Owner) return;

	if (!Owner->HasAuthority()) return;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(MeleeSweep), false, Owner);
	TArray<FHitResult> Hits;
	const FCollisionShape Sphere = FCollisionShape::MakeSphere(TraceRadius);

	const bool bHit = World->SweepMultiByChannel(Hits, From, To, FQuat::Identity, ECC_Pawn, Sphere, Params);
	if (!bHit) return;

	AController* InstigatorController = Owner->GetInstigatorController();

	for (const FHitResult& H : Hits)
	{
		AActor* Other = H.GetActor();
		if (!Other || Other == Owner) continue;
		if (AlreadyHit.Contains(Other)) continue;

		const FVector Dir = (To - From).GetSafeNormal();
		const float Applied = UGameplayStatics::ApplyPointDamage(Other, Damage, Dir, H, InstigatorController, Owner, DamageTypeClass);

		UE_LOG(LogTemp, Verbose,
			TEXT("[MeleeTrace] %s -> %s dmg=%.1f applied=%.1f"),
			*GetNameSafe(Owner),     // было FString, должно быть *FString
			*GetNameSafe(Other),     // тоже *FString
			Damage,
			Applied
		);

		AlreadyHit.Add(Other);
	}
}