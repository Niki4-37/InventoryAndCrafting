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
    PersonalSlots,
    ShopSlots
};

// clang-format off
UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
    Head,
    RightArm            UMETA(ToolTip="choose this for all main weapons"),
    LeftArm,
    Legs,
    Torso,
    Back,
    NotEquipment,

    Begin = Head        UMETA(Hidden),
    End = NotEquipment  UMETA(Hidden)
};
// clang-format on

static EEquipmentType& operator++(EEquipmentType& EType)
{
    EType = EEquipmentType(static_cast<std::underlying_type<EEquipmentType>::type>(EType) + 1);
    return EType;
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

    bool operator==(const FSlot& S) const
    {
        return DataTableRowHandle == S.DataTableRowHandle;
    }
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
    EEquipmentType EquipmnetType{EEquipmentType::NotEquipment};

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    bool bCanIncreasePersonalSlots{false};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "bCanIncreasePersonalSlots"))
    TArray<FSlot> PresonalExtraSlots;

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

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    int32 Cost;
};

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnSlotDataChangedSignature, const FSlot&, const uint8, ESlotLocationType);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStuffEquipedSignature, const TArray<FSlot>&, ESlotLocationType);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEquipmentSlotDataChangedSignature, const FSlot&, EEquipmentType);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTradingSignature, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMoneyValueChangedSignature, int32);

USTRUCT()
struct FEquipmentSlot
{
    GENERATED_USTRUCT_BODY()

    EEquipmentType EquipmentType;
    UPROPERTY()
    FDataTableRowHandle DataTableRowHandle;
    int32 Amount;

    FEquipmentSlot(EEquipmentType Type = EEquipmentType::End, FDataTableRowHandle DataTable = FDataTableRowHandle(), int32 InAmount = 0)
    : EquipmentType(Type), DataTableRowHandle(DataTable), Amount(InAmount)
    { }

    FEquipmentSlot(FEquipmentSlot&& InSlot)
    : EquipmentType(InSlot.EquipmentType), DataTableRowHandle(InSlot.DataTableRowHandle), Amount(InSlot.Amount)
    { }

    FEquipmentSlot(const FEquipmentSlot& InSlot)
    : EquipmentType(InSlot.EquipmentType), DataTableRowHandle(InSlot.DataTableRowHandle), Amount(InSlot.Amount)
    { }

    FEquipmentSlot& operator=(const FEquipmentSlot& InSlot)
    {
        EquipmentType = InSlot.EquipmentType;
        DataTableRowHandle = InSlot.DataTableRowHandle;
        Amount = InSlot.Amount;

        return *this;
    }
};

USTRUCT()
struct FDragDropData
{
    GENERATED_USTRUCT_BODY()

    ESlotLocationType FromLocationType;
    EEquipmentType FromEquipmentType;
    uint8 FromSlotIndex;
    ESlotLocationType ToLocationType;
    EEquipmentType ToEquipmentType;
    uint8 ToSlotIndex;

    FDragDropData()
    {
        FromLocationType = ESlotLocationType::Environment;
        FromEquipmentType = EEquipmentType::NotEquipment;
        FromSlotIndex = 0;
        ToLocationType = ESlotLocationType::Environment;
        ToEquipmentType = EEquipmentType::NotEquipment;
        ToSlotIndex = 0;
    }

    FDragDropData(ESlotLocationType InFromLocationType //
    , EEquipmentType InFromEquipmentType //
    , uint8 InFromSlotIndex //
    , ESlotLocationType InToLocationType //
    , EEquipmentType InToEquipmentType //
    , uint8 InToSlotIndex)
    : FromLocationType(InFromLocationType)//
    , FromEquipmentType(InFromEquipmentType)//
    , FromSlotIndex(InFromSlotIndex)//
    , ToLocationType(InToLocationType)//
    , ToEquipmentType(InToEquipmentType)//
    , ToSlotIndex(InToSlotIndex)
    { }   

    FDragDropData(FDragDropData&& Data)
    : FromLocationType(Data.FromLocationType)//
    , FromEquipmentType(Data.FromEquipmentType)//
    , FromSlotIndex(Data.FromSlotIndex)//
    , ToLocationType(Data.ToLocationType)//
    , ToEquipmentType(Data.ToEquipmentType)//
    , ToSlotIndex(Data.ToSlotIndex)
    { }   

    FDragDropData(const FDragDropData& Data)
    : FromLocationType(Data.FromLocationType)//
    , FromEquipmentType(Data.FromEquipmentType)//
    , FromSlotIndex(Data.FromSlotIndex)//
    , ToLocationType(Data.ToLocationType)//
    , ToEquipmentType(Data.ToEquipmentType)//
    , ToSlotIndex(Data.ToSlotIndex)
    { }

    FDragDropData& operator=(const FDragDropData& Data)
    {
        FromLocationType = Data.FromLocationType;
        FromEquipmentType = Data.FromEquipmentType;
        FromSlotIndex = Data.FromSlotIndex;
        ToLocationType = Data.ToLocationType;
        ToEquipmentType = Data.ToEquipmentType;
        ToSlotIndex = Data.ToSlotIndex;

        return *this;
    }
};