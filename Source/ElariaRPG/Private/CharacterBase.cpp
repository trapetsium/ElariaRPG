#include "CharacterBase.h"
#include "ElariaRPG.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "TimerManager.h"
#include "AttackTraceComponent.h"
#include "Animation/AnimMontage.h"
#include "Components/InputComponent.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"


//Управление
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ACharacterBase::AttackInput);
}

void ACharacterBase::AttackInput()
{
    if (HasAuthority())
    {
        if (AttackMontage) { PlayAnimMontage(AttackMontage); }
        Multicast_PlayAttackMontage();
    }
    else
    {
        Server_StartAttack(); // клиент просит сервер начать атаку
    }
}

void ACharacterBase::Server_StartAttack_Implementation()
{
    if (AttackMontage) { PlayAnimMontage(AttackMontage); }
    Multicast_PlayAttackMontage();
}

void ACharacterBase::Multicast_PlayAttackMontage_Implementation()
{
    // На клиентах проигрываем (на сервере уже сыграно выше)
    if (!HasAuthority() && AttackMontage)
    {
        PlayAnimMontage(AttackMontage);
    }
}


ACharacterBase::ACharacterBase(const FObjectInitializer& ObhectInitializer)
{
    PrimaryActorTick.bAllowTickOnDedicatedServer = false;
    CurrentHealth = MaxHealth;

    bReplicates = true;
    AttackTrace = CreateDefaultSubobject<UAttackTraceComponent>(TEXT("AttackTrace"));
}

ACharacterBase::ACharacterBase()
: ACharacterBase(FObjectInitializer::Get())
{

}

void ACharacterBase::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = FMath::Clamp(CurrentHealth, 0.f, MaxHealth);
}

float ACharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (bIsDead || DamageAmount <= 0.0f || !CanBeDamaged())
        return 0;

    const float Applied = FMath::Max(DamageAmount, 0.f);
    const float HealthBefore = CurrentHealth;

    const FString InstigatorName = EventInstigator ? *EventInstigator->GetName() : TEXT("None");
    const FString CauserName = DamageCauser ? *DamageCauser->GetName() : TEXT("None");

    UE_LOG(LogElaria, Warning, TEXT("[Damage] Target=%s Amount=%.2f Instigator=%s Causer=%s DamageType=%s"),
        *GetName(), Applied, *InstigatorName, *CauserName,
        DamageEvent.DamageTypeClass ? *DamageEvent.DamageTypeClass->GetName() : TEXT("None"));

    CurrentHealth = FMath::Max(CurrentHealth - Applied, 0.0f);

    if (CurrentHealth <= 0.f && !bIsDead)
    {
        bIsDead = true;
        SetCanBeDamaged(false);
        HandleDeath();
    }

    return FMath::Min(Applied, HealthBefore);
}

void ACharacterBase::HandleDeath()
{
    UE_LOG(LogElaria, Warning, TEXT("[Death] %s died"), *GetName());

    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->DisableMovement();
    }

    if (UCapsuleComponent* Capsule = GetCapsuleComponent())
    {
        Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Capsule->SetGenerateOverlapEvents(false);
    }

    if (USkeletalMeshComponent* MeshComp = GetMesh())
    {
        MeshComp->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
        MeshComp->SetSimulatePhysics(true);
        MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));
    }

    if (TimeBeforeAutoDestroy > 0.f)
    {
        GetWorldTimerManager().SetTimer(AutoDestroyTimer, this, &ACharacterBase::OnAutoDestroy, TimeBeforeAutoDestroy, false);
    }
}

void ACharacterBase::OnAutoDestroy()
{
    UE_LOG(LogElaria, Warning, TEXT("[Cleanup] Destroying %s after death timeout"), *GetName());
    Destroy();
}