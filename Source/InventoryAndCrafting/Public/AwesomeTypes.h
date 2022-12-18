#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/DataTable.h"
#include "AwesomeTypes.generated.h"

class UTexture2D;
class UStaticMesh;

UENUM(BlueprintType)
enum class EItemLocationType : uint8
{
    Environment,
    Inventory,
    Equipment
};

USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase  // S_Inventory
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Description;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UTexture2D* Icon{nullptr};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UStaticMesh* Mesh{nullptr};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCanStack;

    FItemData()
    {
        Name = "Empty";
        Description = "";
        Icon = nullptr;
        Mesh = nullptr;
        bCanStack = false;
    }
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSlotsChangedSignature, const TArray<FSlot>&);

USTRUCT(BlueprintType)
struct FSlot
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDataTableRowHandle DataTableRowHandle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Amount{0};

    EItemLocationType ItemLocationType;

    // clang-format off
    FSlot(const FSlot& InSlot) 
        : DataTableRowHandle(InSlot.DataTableRowHandle)
        , Amount(InSlot.Amount)
        , ItemLocationType(InSlot.ItemLocationType)
    {}

    FSlot(FSlot& InSlot) 
        : DataTableRowHandle(InSlot.DataTableRowHandle)
        , Amount(InSlot.Amount)
        , ItemLocationType(InSlot.ItemLocationType)
    {}

    FSlot(const FDataTableRowHandle& InDataTableRowHandle = FDataTableRowHandle(),  //
            int32 InAmount = 0,                                                     //
            EItemLocationType InItemLocationType = EItemLocationType::Environment)  //
        : DataTableRowHandle(InDataTableRowHandle)
        , Amount(InAmount)
        , ItemLocationType(InItemLocationType)
    {}
    //clang-format on
};