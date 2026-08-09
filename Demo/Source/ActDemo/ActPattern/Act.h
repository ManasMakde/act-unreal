// v0.2.0-alpha
//
// Copyright (c) 2025-present Manas Ravindra Makde
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Act.generated.h"



// Macros
#define UE_STATUS_SAFEGUARD(CURRENT_VALUE, CHECK_VALUE) if (CURRENT_VALUE != CHECK_VALUE) { return; }


// Forward declarations
class UTheater;


// Enums
UENUM(BlueprintType)
enum class EActTickFlags : uint8 {
    None = 0,
    Tick = 1 << 0
};
ENUM_CLASS_FLAGS(EActTickFlags)

UENUM(BlueprintType)
enum class EActStatus : uint8 {
    None,
    Prologuing,
    Entering,
    Ticking,
    Exiting
};

UENUM(BlueprintType)
enum class EActOutcome : uint8 {
    Interrupted,
    Failure,
    Pending,
    Success,
    Retry
};

UENUM(BlueprintType)
enum class EActBlockType : uint8 {
    Interrupt,
    Persistent
};



// Structs
USTRUCT(BlueprintType)
struct FActArray {
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Act")
    TArray<class UAct*> Acts;
};



// Delegate signatures
UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActBP, UAct*, Act);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAct, UAct* /* Act */);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPrologueCompleteBP, UAct*, Act, UAct*, PrologueAct, EActOutcome, Outcome);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnPrologueComplete, UAct* /* Act */, UAct* /* PrologueAct */, EActOutcome /* Outcome */);

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
    UPROPERTY(BlueprintAssignable, Category = "Act")
    FOnActBP OnPreSetupBP;

    UPROPERTY(BlueprintAssignable, Category = "Act")
    FOnActBP OnPostSetupBP;

    UPROPERTY(BlueprintAssignable, Category = "Act")
    FOnActBP OnPerformStartBP;

    UPROPERTY(BlueprintAssignable, Category = "Act")
    FOnActBP OnPrePrologueBP;

    UPROPERTY(BlueprintAssignable, Category = "Act")
    FOnPrologueCompleteBP OnPrologueCompleteBP;

    UPROPERTY(BlueprintAssignable, Category = "Act")
    FOnActBP OnPostPrologueBP;

    UPROPERTY(BlueprintAssignable, Category = "Act")
    FOnActBP OnPreEnterBP;

    UPROPERTY(BlueprintAssignable, Category = "Act")
    FOnActBP OnPostEnterBP;

    UPROPERTY(BlueprintAssignable, Category = "Act")
    FOnActBP OnPreTickBP;

    UPROPERTY(BlueprintAssignable, Category = "Act")
    FOnActBP OnPostTickBP;

    UPROPERTY(BlueprintAssignable, Category = "Act")
    FOnActBP OnPreExitBP;

    UPROPERTY(BlueprintAssignable, Category = "Act")
    FOnActBP OnPostExitBP;

    UPROPERTY(BlueprintAssignable, Category = "Act")
    FOnActBP OnPerformEndBP;

    UPROPERTY(BlueprintAssignable, Category = "Act")
    FOnActBP OnPreCleanupBP;

    UPROPERTY(BlueprintAssignable, Category = "Act")
    FOnActBP OnPostCleanupBP;

    UPROPERTY(BlueprintAssignable, Category = "Act")
    FOnEnableChangedBP OnEnableChangedBP;

    UPROPERTY(BlueprintAssignable, Category = "Act")
    FOnBlockChangedBP OnBlockChangedBP;

    FOnAct OnPreSetup;
    FOnAct OnPostSetup;
    FOnAct OnPerformStart;
    FOnAct OnPrePrologue;
    FOnPrologueComplete OnPrologueComplete;
    FOnAct OnPostPrologue;
    FOnAct OnPreEnter;
    FOnAct OnPostEnter;
    FOnAct OnPreTick;
    FOnAct OnPostTick;
    FOnAct OnPreExit;
    FOnAct OnPostExit;
    FOnAct OnPerformEnd;
    FOnAct OnPreCleanup;
    FOnAct OnPostCleanup;
    FOnEnableChanged OnEnableChanged;
    FOnBlockChanged OnBlockChanged;



    // Public Properties
    UPROPERTY(BlueprintReadWrite, Category = "Act")
    FPrologueBP PrologueBP;

    UPROPERTY(BlueprintReadWrite, Category = "Act")
    TArray<FPerformCondition> PerformConditionsBP;

    TFunction<TArray<UAct*>(UAct*)> Prologue = [](UAct* /*Act*/) -> TArray<UAct*> { return TArray<UAct*>(); };
    TArray<TFunction<bool(UAct*)>> PerformConditions = {};

    UPROPERTY(BlueprintReadWrite, Category = "Act")
    bool bIsVerbose = false;  // Toggle for warning messages



    // Public Methods
    UFUNCTION(BlueprintCallable, Category = "Act")
    void Init(FString NewName = TEXT(""), class UTheater* NewTheater = nullptr, bool bIsInitiallyEnabled = true);

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
    void AddToBlock(TArray<UAct*> Acts, EActBlockType BlockType = EActBlockType::Persistent);

    UFUNCTION(BlueprintCallable, Category = "Act")
    void RemoveFromBlock(TArray<UAct*> Acts);

    UFUNCTION(BlueprintCallable, Category = "Act")
    void SetEnabled(bool bNewEnabled);

    UFUNCTION(BlueprintPure, Category = "Act")
    bool DidPerform(EActTickFlags TickFlag = EActTickFlags::Tick) const;

    UFUNCTION(BlueprintPure, Category = "Act")
    bool HasInitialized() const;

    UFUNCTION(BlueprintPure, Category = "Act")
    bool IsInitializing() const;

    UFUNCTION(BlueprintPure, Category = "Act")
    bool IsOngoing() const;

    UFUNCTION(BlueprintPure, Category = "Act")
    bool IsActive() const;

    UFUNCTION(BlueprintPure, Category = "Act")
    bool IsEnabled() const;

    UFUNCTION(BlueprintPure, Category = "Act")
    bool IsBlocked() const;

    UFUNCTION(BlueprintPure, Category = "Act")
    bool CanTick(EActTickFlags Type) const;

    UFUNCTION(BlueprintPure, Category = "Act")
    class UTheater* GetTheater() const;

    UFUNCTION(BlueprintPure, Category = "Act")
    class AActor* GetOwner() const;

    UFUNCTION(BlueprintPure, Category = "Act")
    TSet<UAct*> GetBlockedByActs() const;

    UFUNCTION(BlueprintPure, Category = "Act")
    TMap<UAct*, EActBlockType> GetActsToBlock() const;

    UFUNCTION(BlueprintPure, Category = "Act")
    EActStatus GetStatus() const;

    UFUNCTION(BlueprintPure, Category = "Act")
    EActOutcome GetOutcome() const;

    UFUNCTION(BlueprintPure, Category = "Act")
    int32 GetPerformCount() const;

    UFUNCTION(BlueprintPure, Category = "Act")
    int32 GetTickCount() const;

    UFUNCTION(BlueprintPure, Category = "Act")
    float GetDelta() const;

    UFUNCTION(BlueprintPure, Category = "Act")
    FString GetName() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Act")
    static TArray<UAct*> SeqBP(TArray<FActArray> PArrays);

    static TArray<UAct*> Seq(TArray<TArray<UAct*>> PArrays);



   protected:
    // Protected Properties
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Act")
    FString Name = "";

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Act")
    bool bCanReperform = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Act")
    EActTickFlags TickFlags = EActTickFlags::None;



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

    UFUNCTION(BlueprintCallable, Category = "Act")
    void Finish(EActOutcome NewOutcome = EActOutcome::Success);

    UFUNCTION(BlueprintNativeEvent, Category = "Act")
    void BlockSelf(class UAct* ByAct, EActBlockType BlockType);

    UFUNCTION(BlueprintNativeEvent, Category = "Act")
    void UnblockSelf(UAct* ByAct);

    UFUNCTION(BlueprintNativeEvent, Category = "Act")
    void BlockOthers();

    UFUNCTION(BlueprintNativeEvent, Category = "Act")
    void UnblockOthers();

    UFUNCTION(BlueprintNativeEvent, Category = "Act")
    void WriteLog(const FString& Message, const FString& OverrideName = "");



   private:
    // Private Properties
    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    class UTheater* Theater = nullptr;

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    EActStatus Status = EActStatus::None;

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    EActStatus PrevStatus = EActStatus::None;

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    EActOutcome Outcome = EActOutcome::Pending;

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    TMap<UAct*, EActBlockType> ActsToBlock;

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    TSet<UAct*> BlockedByActs;

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    TSet<UAct*> EpilogueActs;

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    TSet<UAct*> PendingEpilogueActs;

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    TSet<UAct*> PrologueActs;

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    TSet<UAct*> PendingPrologueActs;

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    TSet<UAct*> CompletedPrologueActs;

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    TSet<UAct*> ResultTopEpilogues;

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    TSet<UAct*> VisitedTopEpilogues;

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    bool bHasInitialized = false;

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    bool bIsInitializing = false;

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    bool bHasPrecomputedPrologues = false;

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    int32 PerformCount = 0;

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    int32 TickCount = 0;

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    int32 TickReqCount = 0;

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    int64 PerformedOnTick = -1;


    // Private Methods
    static void LinkPrologueArrays(const TArray<UAct*>& ArrayB, const TArray<UAct*>& ArrayA);
    static TSet<UAct*> GetTopEpilogues(UAct* OfAct, TSet<UAct*>& Result, TSet<UAct*>& Visited);
    static void PrecomputePrologueChain(UAct* OfAct);
    static void FinishPrologues(UAct* OfAct, EActOutcome NewOutcome);
    static void ContinueEpilogues(UAct* OfAct, EActOutcome NewOutcome);
    static void ClearPrologueChain(UAct* OfAct);
    static UAct* GetFirst(const TSet<UAct*>& Data);
	static bool DoesOverlap(const TSet<UAct*>& A, const TSet<UAct*>& B);

    bool CanPerformImpl(bool bIsRetrying = false);
    void PerformImpl();
    void PrologueImpl();
    void CompletedPrologue(UAct* PAct, EActOutcome NewOutcome);
    void EnterImpl();
    void HandleTickingImpl();
    void TickImpl();
    void ExitImpl();
    void Redirect(EActStatus NewStatus, EActOutcome NewOutcome = EActOutcome::Pending);
};
