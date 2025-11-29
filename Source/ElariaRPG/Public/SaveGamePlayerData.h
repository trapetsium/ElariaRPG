#pragma once
#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveGamePlayerData.generated.h"

USTRUCT(BlueprintType)
struct ELARIARPG_API FItemData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName Id;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Count = 1;
};

UCLASS()
class ELARIARPG_API USaveGamePlayerData : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Player")
    TArray<FItemData> Inventory;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Player")
    float Health = 100.f;
};