#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

UCLASS()
class ELARIARPG_API ACharacterBase : public ACharacter
{
    GENERATED_BODY()

public:
    ACharacterBase(const FObjectInitializer& ObhectInitializer);
    ACharacterBase();

protected:
    virtual void BeginPlay() override;

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
    float MaxHealth = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
    float CurrentHealth;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
    bool bIsDead = false;

    UFUNCTION(BlueprintCallable, Category = "Health")
    void HandleDeath();

    UFUNCTION()
    void OnAutoDestroy();

    FTimerHandle AutoDestroyTimer;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Death")
    float TimeBeforeAutoDestroy = 30.f;

    // ===== Combat =====
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    class UAttackTraceComponent* AttackTrace = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    class UAnimMontage* AttackMontage = nullptr;

    UFUNCTION()
    void AttackInput();

    UFUNCTION(Server, Reliable)
    void Server_StartAttack();

    UFUNCTION(NetMulticast, Unreliable)
    void Multicast_PlayAttackMontage();

public:
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator, class AActor* DamageCauser) override;
};
