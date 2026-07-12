#pragma once

#include "CoreMinimal.h"
#include "Act.generated.h"



// Macros
#define UE_STATUS_SAFEGUARD(CURRENT_VALUE, CHECK_VALUE) if (CURRENT_VALUE != CHECK_VALUE) { return; }



// Enums
UENUM(BlueprintType)
enum class EActTickFlags : uint8 {
    None = 0,
    Tick = 1 << 0
};
ENUM_CLASS_FLAGS(EActTickFlags)

UENUM(BlueprintType)
enum class EActStatus : uint8 {
    None = 0,
    Prologuing,
    Entering,
    Ticking,
    Exiting
};

UENUM(BlueprintType)
enum class EActOutcome : uint8 {
    Interrupted = -2,
    Failure = -1,
    Pending = 0,
    Success = 1,
    Retry = 2
};

UENUM(BlueprintType)
enum class EActBlockType : uint8 {
    Oneshot,
    Persistent
};



// Structs
USTRUCT(BlueprintType)
struct FActArray {
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Act")
    TArray<UAct*> Acts;
};



// Delegate signatures
UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActBP, UAct*, Act);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAct, UAct* /* Act */);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnableChangedBP, UAct*, Act, bool, bNewIsEnabled);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEnableChanged, UAct* /* Act */, bool /* bNewIsEnabled */);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnBlockChangedBP, UAct*, Act, UAct*, BlockingAct, EActBlockType, BlockType, bool, bDidBlock);
DECLARE_MULTICAST_DELEGATE_FourParams(FOnBlockChanged, UAct* /* Act */, UAct* /* BlockingAct */, EActBlockType /* BlockType */, bool /* bDidBlock */);

UDELEGATE()
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(TArray<UAct*>, FPrologueBP, UAct*, Act);

UDELEGATE()
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(bool, FPerformCondition, UAct*, Act);



// Classes
UCLASS(Blueprintable, BlueprintType, DefaultToInstanced)
class ACTDEMO_API UAct : public UObject {

    GENERATED_BODY()


   public:
    // Friends
    friend class UTheater;



    // Public Delegates
    UPROPERTY(BlueprintAssignable)
    FOnActBP OnPreSetupBP;

    UPROPERTY(BlueprintAssignable)
    FOnActBP OnPostSetupBP;

    UPROPERTY(BlueprintAssignable)
    FOnActBP OnPrePrologueBP;

    UPROPERTY(BlueprintAssignable)
    FOnActBP OnPostPrologueBP;

    UPROPERTY(BlueprintAssignable)
    FOnActBP OnPreEnterBP;

    UPROPERTY(BlueprintAssignable)
    FOnActBP OnPostEnterBP;

    UPROPERTY(BlueprintAssignable)
    FOnActBP OnPreTickBP;

    UPROPERTY(BlueprintAssignable)
    FOnActBP OnPostTickBP;

    UPROPERTY(BlueprintAssignable)
    FOnActBP OnPreExitBP;

    UPROPERTY(BlueprintAssignable)
    FOnActBP OnPostExitBP;

    UPROPERTY(BlueprintAssignable)
    FOnActBP OnPreCleanupBP;

    UPROPERTY(BlueprintAssignable)
    FOnActBP OnPostCleanupBP;

    UPROPERTY(BlueprintAssignable)
    FOnEnableChangedBP OnEnableChangedBP;

    UPROPERTY(BlueprintAssignable)
    FOnBlockChangedBP OnBlockChangedBP;

    FOnAct OnPreSetup;
    FOnAct OnPostSetup;
    FOnAct OnPrePrologue;
    FOnAct OnPostPrologue;
    FOnAct OnPreEnter;
    FOnAct OnPostEnter;
    FOnAct OnPreTick;
    FOnAct OnPostTick;
    FOnAct OnPreExit;
    FOnAct OnPostExit;
    FOnAct OnPreCleanup;
    FOnAct OnPostCleanup;
    FOnEnableChanged OnEnableChanged;
    FOnBlockChanged OnBlockChanged;



    // Public Properties
    UPROPERTY(BlueprintReadWrite)
    FPrologueBP PrologueBP;

    UPROPERTY(BlueprintReadWrite)
    TArray<FPerformCondition> PerformConditionsBP;

    TFunction<TArray<UAct*>(UAct*)> Prologue = [](UAct* /*Act*/) -> TArray<UAct*> { return TArray<UAct*>(); };
    TArray<TFunction<bool(UAct*)>> PerformConditions = {};



    // Public Methods
    UFUNCTION(BlueprintCallable, Category = "Act")
    void Init(class UTheater* NewTheater, const FString& InName = TEXT(""), bool bInitiallyEnabled = true);

    UFUNCTION(BlueprintCallable, Category = "Act")
    void Deinit();

    UFUNCTION(BlueprintCallable, Category = "Act")
    void Perform();

    UFUNCTION(BlueprintCallable, Category = "Act")
    void PerformDeferred(EActTickFlags TickFlag = EActTickFlags::Tick);

    UFUNCTION(BlueprintCallable, Category = "Act")
    void Retry();

    UFUNCTION(BlueprintCallable, Category = "Act")
    void Abort();

    UFUNCTION(BlueprintCallable, Category = "Act")
    void AddToBlock(const TArray<UAct*>& Acts, EActBlockType BlockType = EActBlockType::Persistent);

    UFUNCTION(BlueprintCallable, Category = "Act")
    void RemoveFromBlock(const TArray<UAct*>& Acts);

    UFUNCTION(BlueprintCallable, Category = "Act")
    void SetEnabled(bool bNewEnabled);

    UFUNCTION(BlueprintCallable, Category = "Act")
    bool DidPerform(EActTickFlags TickFlag = EActTickFlags::Tick);  // True if act was performed at least once during current tick

    UFUNCTION(BlueprintCallable, Category = "Act")
    bool DidPerformEver() const;  // True if act was performed at least once since init

    UFUNCTION(BlueprintCallable, Category = "Act")
    bool IsOngoing() const;

    UFUNCTION(BlueprintCallable, Category = "Act")
    bool IsEnabled() const;

    UFUNCTION(BlueprintCallable, Category = "Act")
    bool IsBlocked() const;

    UFUNCTION(BlueprintCallable, Category = "Act")
    bool DidEnter() const;

    UFUNCTION(BlueprintCallable, Category = "Act")
    bool CanTick(EActTickFlags Type) const;

    UFUNCTION(BlueprintCallable, Category = "Act")
    EActOutcome GetOutcome() const;

    UFUNCTION(BlueprintCallable, Category = "Act")
    UTheater* GetTheater() const;

    UFUNCTION(BlueprintCallable, Category = "Act")
    AActor* GetOwner() const;

    UFUNCTION(BlueprintCallable, Category = "Act")
    float GetDelta() const;

    UFUNCTION(BlueprintCallable, Category = "Act")
    FString GetName() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Act")
    static TArray<UAct*> SeqBP(TArray<FActArray> PArrays);  // Only use inside Prologue

    static TArray<UAct*> Seq(TArray<TArray<UAct*>> PArrays);  // Only use inside Prologue



   protected:
    // Protected Properties
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Act")
    bool bCanReperform = false;  // Indicates if act can interrupt itself and restart perform, assign in Setup

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Act")
    EActTickFlags TickFlags = EActTickFlags::None;  // Indicates if act will be ticking after entering, assign in Setup



    // Protected Methods
    UFUNCTION(BlueprintNativeEvent, Category = "Act")
    void Setup();

    UFUNCTION(BlueprintNativeEvent, Category = "Act")
    bool CanPerform();

    UFUNCTION(BlueprintNativeEvent, Category = "Act")
    EActOutcome Enter();

    UFUNCTION(BlueprintNativeEvent, Category = "Act")
    EActOutcome Tick();

    UFUNCTION(BlueprintNativeEvent, Category = "Act")
    void Exit();

    UFUNCTION(BlueprintNativeEvent, Category = "Act")
    void Cleanup();

    UFUNCTION(Category = "Act")
    void Finish(EActOutcome NewOutcome = EActOutcome::Success);  // Call in Enter() if Exit() needs to be delayed

    UFUNCTION(BlueprintNativeEvent, Category = "Act")
    void BlockSelf(UAct* ByAct, EActBlockType BlockType);

    UFUNCTION(BlueprintNativeEvent, Category = "Act")
    void UnblockSelf(UAct* ByAct);

    UFUNCTION(BlueprintNativeEvent, Category = "Act")
    void BlockOthers();

    UFUNCTION(BlueprintNativeEvent, Category = "Act")
    void UnblockOthers();



   private:
    // Private Properties
    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    FString Name;  // Useful for debugging

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    UTheater* Theater;  // Which theater this act belongs to

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    EActStatus Status = EActStatus::None;

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    EActOutcome Outcome = EActOutcome::Pending;

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    bool bDidEnter = false;

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    TMap<UAct*, EActBlockType> ActsToBlock;  // Which acts to block when performing this act

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    TSet<UAct*> BlockedByActs;  // Which acts are blocking this act

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    TSet<UAct*> TopEpilogueActs;

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    TSet<UAct*> EpilogueActs;

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    TSet<UAct*> PrologueActs;

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    int32 PrologueCompleteCount = 0;

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    int64 PerformedOnTick = -1;



    // Private Methods
    static void LinkPrologueArrays(const TArray<UAct*>& ArrayB, const TArray<UAct*>& ArrayA);
    static bool InSamePrologueChain(UAct* ActA, UAct* ActB);
    static void FinishEpilogues(UAct* OfAct, EActOutcome NewOutcome);
    static void FinishPrologues(UAct* OfAct, EActOutcome NewOutcome);
    static void ClearPrologueChain(UAct* OfAct);
    static void AssignPrologueEpilogue(UAct* EAct, UAct* PAct);
    static void AssignTopEpilogues(UAct* EAct, TSet<UAct*> TopEpilogues = TSet<UAct*>());
    bool CanPerformImpl();
    void PerformImpl();
    void PrologueImpl();
    void ContinuePrologue(UAct* PAct, EActOutcome NewOutcome = EActOutcome::Pending);
    void EnterImpl();
    void TickImpl();
    void ExitImpl();
    void Redirect(EActStatus NewStatus, EActOutcome NewOutcome = EActOutcome::Pending);
};
