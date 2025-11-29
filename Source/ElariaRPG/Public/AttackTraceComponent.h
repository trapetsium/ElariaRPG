// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackTraceComponent.generated.h"


UCLASS( ClassGroup=(Combat), meta=(BlueprintSpawnableComponent) )
class ELARIARPG_API UAttackTraceComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttackTraceComponent();

	// Включить/выключить окно удара (дергать из анимации или по кнопке)
	UFUNCTION(BlueprintCallable, Category = "Combat") void StartWindow();
	UFUNCTION(BlueprintCallable, Category = "Combat") void EndWindow();


	// Имена сокетов/костей на SkeletalMesh владельца.
	// По умолчанию "hand_r" существует у манекена UE4/UE5 (как кость).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FName StartSocket = TEXT("hand_r");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FName EndSocket = TEXT("weapon_tip");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float TraceRadius = 8.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float Damage = 25.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<class UDamageType> DamageTypeClass;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY() class USkeletalMeshComponent* Mesh = nullptr;
	bool bTracing = false;

	FVector PrevStart = FVector::ZeroVector;
	FVector PrevEnd = FVector::ZeroVector;

	// Чтобы не дамажить одну и ту же цель много раз в одном окне удара
	TSet<TWeakObjectPtr<AActor>> AlreadyHit;
	
	void RecordSockets(FVector& OutStart, FVector& OutEnd) const;
	void DoSweep(const FVector& From, const FVector& to);
};
