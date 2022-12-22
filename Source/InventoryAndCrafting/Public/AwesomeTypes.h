#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/DataTable.h"
#include "AwesomeTypes.generated.h"

class UTexture2D;
class UStaticMesh;

UENUM(BlueprintType)
enum class ESlotLocationType : uint8
{
    Environment,
    Equipment,
    Inventory,
    PersonalSlots
};

USTRUCT(BlueprintType)
struct FSlot
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDataTableRowHandle DataTableRowHandle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Amount{0};

    // clang-format off
    FSlot(const FSlot& InSlot) 
        : DataTableRowHandle(InSlot.DataTableRowHandle)
        , Amount(InSlot.Amount)
    {}

    FSlot(FSlot& InSlot) 
        : DataTableRowHandle(InSlot.DataTableRowHandle)
        , Amount(InSlot.Amount)
    {}

    FSlot(const FDataTableRowHandle& InDataTableRowHandle = FDataTableRowHandle(),  //
            int32 InAmount = 0)                                                     //
        : DataTableRowHandle(InDataTableRowHandle)
        , Amount(InAmount)
    {}
    //clang-format on
};

USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ItemType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AActor> ActorClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UTexture2D* Icon{nullptr};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UStaticMesh* Mesh{nullptr};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCanStack;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    bool bCanCraft;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "bCanCraft"))
    TArray<FSlot> Recipe;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "bCanCraft"))
    int32 OutCraftedAmount;
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSlotDataChangedSignature, const FSlot&, const uint8);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStuffEquipedSignature, const TArray<FSlot>&, ESlotLocationType);

