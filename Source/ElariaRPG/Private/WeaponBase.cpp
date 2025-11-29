#include "WeaponBase.h"
#include "ElariaRPG.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

AWeaponBase::AWeaponBase()
{
    PrimaryActorTick.bCanEverTick = false;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(Root);
    Mesh->SetNotifyRigidBodyCollision(true);
    Mesh->SetGenerateOverlapEvents(true);
    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Mesh->OnComponentHit.AddDynamic(this, &AWeaponBase::OnWeaponHit);
}

void AWeaponBase::BeginPlay()
{
    Super::BeginPlay();
}

void AWeaponBase::OnWeaponHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    FVector NormalImpulse, const FHitResult& Hit)
{
    if (!OtherActor || OtherActor == this) return;

    AActor* MyOwner = GetOwner();
    AController* InstigatorController = MyOwner ? MyOwner->GetInstigatorController() : nullptr;

    float Applied = UGameplayStatics::ApplyPointDamage(
        OtherActor,
        Damage,
        Hit.TraceStart - Hit.ImpactPoint,
        Hit,
        InstigatorController,
        this,
        DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass()
    );

    UE_LOG(LogElaria, Warning, TEXT("[WeaponHit] %s hit %s damage=%.2f resultApplied=%.2f"),
        *GetNameSafe(this), *GetNameSafe(OtherActor), Damage, Applied);
}