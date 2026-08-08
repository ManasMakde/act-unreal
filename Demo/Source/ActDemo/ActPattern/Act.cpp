// v0.1.1-alpha
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


#include "Act.h"
#include "Theater.h"
#include "Misc/App.h"


void UAct::Init(FString NewName, class UTheater* NewTheater, bool bInitiallyEnabled) {

    // Return if trying to reinitialize
    if (bHasInitialized) {
        WriteLog("Failed Init(), Already initialized!");
        return;
    }


    // Return if already initialized
    if (bIsInitializing) {
        WriteLog("Failed Init(), Already initializing or deinitializing!");
        return;
    }


    // Mark as initialization started
    bIsInitializing = true;


    // Assign new name
    if (NewName != "") {
        Name = NewName;
    }


    // Assign new owning theater
    if (IsValid(NewTheater)) {
        Theater = NewTheater;
        Theater->AddAct(this);
    }


    // Disable Initially
    if (!bInitiallyEnabled) {
        BlockSelf(this, EActBlockType::Persistent);
    }


    // Broadcast pre setup
    OnPreSetupBP.Broadcast(this);
    OnPreSetup.Broadcast(this);


    // Core setup
    Setup();


    // Mark as initialization completed
    bIsInitializing = false;  // Intentionally before post setup broadcast DO NOT CHANGE
    bHasInitialized = true;


    // Broadcast post setup
    OnPostSetupBP.Broadcast(this);
    OnPostSetup.Broadcast(this);
}
void UAct::Deinit() {

    // Return if trying to redeinitialize
    if (!bHasInitialized) {
        WriteLog("Failed Deinit(), Already deinitialized!");
        return;
    }


    // Return if not initialized
    if (bIsInitializing) {
        WriteLog("Failed Deinit(), Already initializing or deinitializing!");
        return;
    }


    // Mark as deinitialization started
    bIsInitializing = true;


    // Make sure act is not ongoing
    Abort();


    // Broadcast pre cleanup
    OnPreCleanupBP.Broadcast(this);
    OnPreCleanup.Broadcast(this);


    // Core cleanup
    Cleanup();


    // Broadcast post cleanup
    OnPostCleanupBP.Broadcast(this);
    OnPostCleanup.Broadcast(this);


    // Unassign owning theater
    if (IsValid(Theater)) {
        Theater->RemoveAct(this);
        Theater = nullptr;
    }


    // Reset performed on ticks
    PerformCount = 0;
    PerformedOnTick = -1;


    // Mark as deinitialization completed
    bIsInitializing = false;
    bHasInitialized = false;
}
void UAct::Perform() {
    if (CanPerformImpl()) {
        PerformImpl();
    }
}
void UAct::PerformDeferred(EActTickFlags TickFlag) {

    // Warn if null theater provided
    if (!IsValid(Theater)) {
        WriteLog("Cannot perform deferred, Assign a theater first!");
        return;
    }

    Theater->StageDeferred(this, TickFlag);
}
void UAct::Retry() {

    if (IsOngoing()) {
        Redirect(EActStatus::Exiting, EActOutcome::Retry);
    }
    else {
        Perform();
    }
}
void UAct::Abort() {

    Redirect(EActStatus::Exiting, EActOutcome::Interrupted);


    // Clear deferred
    if (Theater != nullptr) {
        Theater->UnstageDeferred(this);
        return;
    }
}
void UAct::AddToBlock(TArray<UAct*> Acts, EActBlockType BlockType) {

    for (UAct* BAct : Acts) {

        // Skip if self (reserved for enable/disable)
        if (BAct == this) {
            WriteLog("Trying to block self!");
            continue;
        }


        // Add to block list
        ActsToBlock.Add(BAct, BlockType);


        // Block if ongoing
        if (IsOngoing()) {
            BAct->BlockSelf(this, BlockType);
        }
    }
}
void UAct::RemoveFromBlock(TArray<UAct*> Acts) {

    for (UAct* BAct : Acts) {

        // Skip if self (reserved for enable/disable)
        if (BAct == this) {
            WriteLog("Trying to unblock self!");
            continue;
        }


        // Unblock if ongoing
        BAct->UnblockSelf(this);


        // Remove from block list
        ActsToBlock.Remove(BAct);
    }
}
void UAct::SetEnabled(bool bNewEnabled) {

    // Return if trying to reassign same value
    if (bNewEnabled == IsEnabled()) {
        return;
    }


    // Block unblock self
    if (!bNewEnabled) {
        BlockSelf(this, EActBlockType::Persistent);
    }
    else {
        UnblockSelf(this);
    }


    // Broadcast enabled disabled
    OnEnableChangedBP.Broadcast(this, IsEnabled());
    OnEnableChanged.Broadcast(this, IsEnabled());
}
bool UAct::DidPerform(EActTickFlags TickFlag) const {

    // Return false if no flag provided
    if (TickFlag == EActTickFlags::None) {
        return false;
    }


    // Check based on tick types
    bool bHasPerformed = false;
    if (EnumHasAnyFlags(TickFlag, EActTickFlags::Tick)) {
        bHasPerformed = bHasPerformed || PerformedOnTick == static_cast<int64>(GFrameCounter);
    }

    return bHasPerformed;
}
bool UAct::IsOngoing() const {
    return Status != EActStatus::None;
}
bool UAct::IsActive() const {
    return Status != EActStatus::None && Status != EActStatus::Prologuing;
}
bool UAct::IsEnabled() const {
    return !BlockedByActs.Contains(this);
}
bool UAct::IsBlocked() const {

    // Incase act is disabled
    if (BlockedByActs.Num() == 1 && BlockedByActs.Contains(this)) {
        return false;
    }

    return BlockedByActs.Num() != 0;
}
bool UAct::CanTick(EActTickFlags Type) const {
    return EnumHasAnyFlags(TickFlags, Type);
}
class UTheater* UAct::GetTheater() const {
    return Theater;
}
class AActor* UAct::GetOwner() const {
    return Theater != nullptr ? Theater->GetOwner() : nullptr;
}
TSet<UAct*> UAct::GetBlockedByActs() const {
    return TSet<UAct*>(BlockedByActs);
}
TMap<UAct*, EActBlockType> UAct::GetActsToBlock() const {
    return TMap<UAct*, EActBlockType>(ActsToBlock);
}
EActStatus UAct::GetStatus() const {
    return Status;
}
EActOutcome UAct::GetOutcome() const {
    return Outcome;
}
int32 UAct::GetPerformCount() const {
    return PerformCount;
}
int32 UAct::GetTickCount() const {
    return TickCount;
}
float UAct::GetDelta() const {
    return FApp::GetDeltaTime();
}
FString UAct::GetName() const {
    return Name;
}
TArray<UAct*> UAct::SeqBP(TArray<FActArray> PArrays) {

    TArray<TArray<UAct*>> RawArrays;
    for (const FActArray& PArray : PArrays) {
        RawArrays.Add(PArray.Acts);
    }

    return Seq(RawArrays);
}
TArray<UAct*> UAct::Seq(TArray<TArray<UAct*>> PArrays) {

    // Return if any null
    for (const TArray<UAct*>& PArray : PArrays) {
        if (PArray.Contains(nullptr)) {
            return TArray<UAct*>{nullptr};
        }
    }


    // Remove empty lists before chaining
    PArrays.RemoveAll([](const TArray<UAct*>& PArr) { return PArr.Num() == 0; });


    // Return if empty list
    const int32 PLength = PArrays.Num();
    if (PLength == 0) {
        return TArray<UAct*>();
    }


    // Chain all prologues
    for (int32 i = PLength - 1; i > 0; i--) {
        LinkPrologueArrays(PArrays[i], PArrays[i - 1]);
    }

    return PArrays[PLength - 1];  // Return last acts
}



// Protected
void UAct::Setup_Implementation() {
}
bool UAct::CanPerform_Implementation() {
    return true;
}
EActOutcome UAct::Enter_Implementation() {
    return TickFlags != EActTickFlags::None ? EActOutcome::Pending : EActOutcome::Success;
}
EActOutcome UAct::Tick_Implementation() {
    return EActOutcome::Success;
}
void UAct::Exit_Implementation() {
}
void UAct::Cleanup_Implementation() {
}
void UAct::Finish(EActOutcome NewOutcome) {
    Redirect(EActStatus::Exiting, NewOutcome);
}
void UAct::BlockSelf_Implementation(class UAct* ByAct, EActBlockType BlockType) {

    // Return incase null act
    if (ByAct == nullptr) {
        WriteLog("Failed to block, null act provided!");
        return;
    }


    // Return if already blocked
    if (BlockedByActs.Contains(ByAct)) {
        return;
    }


    // Return if both acts are in the same prologue chain
    if (this != ByAct && (EpilogueActs.Num() != 0 || ByAct->EpilogueActs.Num() != 0)) {
        ResultTopEpilogues.Empty();
        VisitedTopEpilogues.Empty();
        ByAct->ResultTopEpilogues.Empty();
        ByAct->VisitedTopEpilogues.Empty();
        if (DoesOverlap(GetTopEpilogues(this, ResultTopEpilogues, VisitedTopEpilogues), GetTopEpilogues(ByAct, ByAct->ResultTopEpilogues, ByAct->VisitedTopEpilogues))) {
            WriteLog("Failed to block, Both " + Name + " and " + ByAct->Name + " are in the same prologue chain!");
            return;
        }
    }


    // Finish interrupted incase ongoing
    Redirect(EActStatus::Exiting, EActOutcome::Interrupted);


    // Add to blocked by list if persistent
    if (BlockType == EActBlockType::Persistent) {
        BlockedByActs.Add(ByAct);
    }


    // Broadcast blocked
    if (ByAct != this) {
        OnBlockChangedBP.Broadcast(this, ByAct, BlockType, true);
        OnBlockChanged.Broadcast(this, ByAct, BlockType, true);
    }
}
void UAct::UnblockSelf_Implementation(UAct* ByAct) {

    // Return incase null act
    if (ByAct == nullptr) {
        WriteLog("Failed to unblock, null act provided!");
        return;
    }


    // Return if not currently blocked by act
    if (!BlockedByActs.Contains(ByAct)) {
        return;
    }


    // Persistent unblocking
    BlockedByActs.Remove(ByAct);


    // Broadcast unblocked
    if (ByAct != this) {
        OnBlockChangedBP.Broadcast(this, ByAct, EActBlockType::Persistent, false);
        OnBlockChanged.Broadcast(this, ByAct, EActBlockType::Persistent, false);
    }
}
void UAct::BlockOthers_Implementation() {
    for (auto& Pair : ActsToBlock) {
        Pair.Key->BlockSelf(this, Pair.Value);
    }
}
void UAct::UnblockOthers_Implementation() {
    for (auto& Pair : ActsToBlock) {
        if (Pair.Value == EActBlockType::Persistent) {
            Pair.Key->UnblockSelf(this);
        }
    }
}
void UAct::WriteLog_Implementation(const FString& Message, const FString& OverrideName) {
    if (!bIsVerbose) {
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[%s] %s"), *(OverrideName != TEXT("") ? OverrideName : Name), *Message);
}



// Private
void UAct::LinkPrologueArrays(const TArray<UAct*>& ArrayB, const TArray<UAct*>& ArrayA) {
    for (int32 i = 0; i < ArrayB.Num(); i++) {
        UAct* ActB = ArrayB[i];
        for (int32 j = 0; j < ArrayA.Num(); j++) {
            UAct* ActA = ArrayA[j];
            ActB->PrologueActs.Add(ActA);
            ActA->EpilogueActs.Add(ActB);
            ActA->PendingEpilogueActs.Add(ActB);
        }
    }
}
TSet<UAct*> UAct::GetTopEpilogues(UAct* OfAct, TSet<UAct*>& Result, TSet<UAct*>& Visited) {

    // Skip if already visited
    if (Visited.Contains(OfAct)) {
        return Result;
    }


    // Mark as visited
    Visited.Add(OfAct);


    // Add if top epilogue
    if (OfAct->EpilogueActs.Num() == 0) {
        Result.Add(OfAct);
        return Result;
    }


    // Recurse into each epilogue
    for (UAct* EAct : OfAct->EpilogueActs) {
        GetTopEpilogues(EAct, Result, Visited);
    }

    return Result;
}
void UAct::PrecomputePrologueChain(UAct* OfAct) {

    // Fail Incase directly null provided
    TArray<UAct*> PrologueActList;
    if (OfAct->PrologueBP.IsBound()) {
        PrologueActList = OfAct->PrologueBP.Execute(OfAct);
    }
    else if (OfAct->Prologue) {
        PrologueActList = OfAct->Prologue(OfAct);
    }
    if (PrologueActList.Num() == 1 && PrologueActList.Contains(nullptr)) {
        OfAct->Redirect(EActStatus::Exiting, EActOutcome::Failure);
        return;
    }


    // Iterate through prologue acts
    for (UAct* PAct : PrologueActList) {

        // Skip self
        if (PAct == OfAct) {
            continue;
        }


        // Fail incase null
        if (PAct == nullptr) {
            OfAct->Redirect(EActStatus::Exiting, EActOutcome::Failure);
            return;
        }


        // Assign prologue and epilogue
        OfAct->PrologueActs.Add(PAct);
        PAct->EpilogueActs.Add(OfAct);


        // Recurse into prologue
        PrecomputePrologueChain(PAct);
    }


    // Mark as precomputed
    OfAct->bHasPrecomputedPrologues = true;
}
void UAct::FinishPrologues(UAct* OfAct, EActOutcome NewOutcome) {

    // Set outcome to iterrupted incase retrying
    EActOutcome POutcome = NewOutcome == EActOutcome::Retry ? EActOutcome::Interrupted : NewOutcome;


    // Finish all pending prologues
    while (OfAct->PendingPrologueActs.Num() != 0) {
        UAct* PAct = GetFirst(OfAct->PendingPrologueActs);
        OfAct->PendingPrologueActs.Remove(PAct);
        if (PAct != nullptr) {
            PAct->Finish(POutcome);
        }
    }
}
void UAct::ContinueEpilogues(UAct* OfAct, EActOutcome NewOutcome) {

    // Continue and clear out epilogues
    while (OfAct->PendingEpilogueActs.Num() != 0) {
        UAct* EAct = GetFirst(OfAct->PendingEpilogueActs);
        OfAct->PendingEpilogueActs.Remove(EAct);
        EAct->CompletedPrologueActs.Add(OfAct);
        EAct->CompletedPrologue(OfAct, NewOutcome);
    }
}
void UAct::ClearPrologueChain(UAct* OfAct) {

    while (OfAct->PrologueActs.Num() != 0 || OfAct->CompletedPrologueActs.Num() != 0) {

        // Get prologue act
        UAct* PAct;
        if (OfAct->PrologueActs.Num() == 0) {
            PAct = GetFirst(OfAct->CompletedPrologueActs);
            OfAct->CompletedPrologueActs.Remove(PAct);
        }
        else {
            PAct = GetFirst(OfAct->PrologueActs);
            OfAct->PrologueActs.Remove(PAct);
        }


        // Skip if null
        if (PAct == nullptr) {
            continue;
        }


        // Remove self from epilogue
        PAct->EpilogueActs.Remove(OfAct);
        PAct->PendingEpilogueActs.Remove(OfAct);


        // Recurse down, Incase Seq() linked stale acts that were never performed
        if (PAct->EpilogueActs.Num() == 0) {
            ClearPrologueChain(PAct);
        }
    }
}
UAct* UAct::GetFirst(const TSet<UAct*>& Data) {

    if (Data.Num() == 0) {
        return nullptr;
    }

    for (UAct* Act : Data) {
        return Act;
    }

    return nullptr;
}
bool UAct::DoesOverlap(const TSet<UAct*>& A, const TSet<UAct*>& B) {
    for (const UAct* K : A) {
        if (B.Contains(K)) {
            return true;
        }
    }

    return false;
}
bool UAct::CanPerformImpl(bool bIsRetrying) {

    // Return if in between initialization
    if (bIsInitializing) {
        WriteLog("Cannot perform, act is initializing or deinitializing!");
        return false;
    }


    // Return if exiting
    if (!bIsRetrying && Status == EActStatus::Exiting) {
        WriteLog("Cannot perform, act is between exiting!");
        return false;
    }


    // Return if disabled or theater is disabled
    if (!IsEnabled() || (Theater != nullptr && !Theater->IsEnabled())) {
        WriteLog("Cannot perform, act or theater is disabled!");
        return false;
    }


    // Return if blocked
    if (IsBlocked()) {
        WriteLog("Cannot perform, act is blocked!");
        return false;
    }


    // Return if already ongoing
    if (!bIsRetrying && !bCanReperform && IsOngoing()) {
        WriteLog("Cannot perform, act is ongoing!");
        return false;
    }


    // Return if any external condition is false
    for (TFunction<bool(UAct*)>& Cond : PerformConditions) {
        if (Cond && !Cond(this)) {
            return false;
        }
    }
    for (FPerformCondition& CondBP : PerformConditionsBP) {
        if (CondBP.IsBound() && !CondBP.Execute(this)) {
            return false;
        }
    }

    return CanPerform();
}
void UAct::PerformImpl() {

    // Finish any ongoing perform
    if (Status != EActStatus::None) {
        Finish(EActOutcome::Interrupted);
    }


    // Store during which tick act was performed
    PerformCount++;
    PerformedOnTick = static_cast<int64>(GFrameCounter);


    // Clear deferred
    if (Theater != nullptr) {
        Theater->UnstageDeferred(this);
    }


    // Start prologuing
    Redirect(EActStatus::Prologuing);
}
void UAct::PrologueImpl() {

    // Broadcast perform start
    OnPerformStartBP.Broadcast(this);
    UE_STATUS_SAFEGUARD(Status, EActStatus::Prologuing);  // Guard

    OnPerformStart.Broadcast(this);
    UE_STATUS_SAFEGUARD(Status, EActStatus::Prologuing);  // Guard


    // Let theater know this act has started
    if (Theater != nullptr) {
        Theater->StageOngoing(this);
    }
    UE_STATUS_SAFEGUARD(Status, EActStatus::Prologuing);  // Guard


    // Precompute prologue chain
    if (!bHasPrecomputedPrologues) {
        PrecomputePrologueChain(this);
    }
    UE_STATUS_SAFEGUARD(Status, EActStatus::Prologuing);  // Guard


    // Assign self as pending epilogue
    for (UAct* PAct : PrologueActs) {
        PAct->PendingEpilogueActs.Add(this);
    }


    // Block
    BlockOthers();
    UE_STATUS_SAFEGUARD(Status, EActStatus::Prologuing);  // Guard


    // Skip if no prologues
    if (PrologueActs.Num() == 0) {
        Redirect(EActStatus::Entering);  // Intentional to skip pre prologue signal
        return;
    }


    // Broadcast pre prologue
    OnPrePrologue.Broadcast(this);
    UE_STATUS_SAFEGUARD(Status, EActStatus::Prologuing);  // Guard

    OnPrePrologueBP.Broadcast(this);
    UE_STATUS_SAFEGUARD(Status, EActStatus::Prologuing);  // Guard


    // Perform all prologues
    while (PrologueActs.Num() != 0) {

        // Guard
        UE_STATUS_SAFEGUARD(Status, EActStatus::Prologuing);


        // Skip prologue if ongoing
        UAct* PAct = GetFirst(PrologueActs);
        if (PAct->IsOngoing()) {
            PrologueActs.Remove(PAct);
            PendingPrologueActs.Add(PAct);
            continue;
        }


        // Skip if already completed
        if (CompletedPrologueActs.Contains(PAct)) {
            PrologueActs.Remove(PAct);
            CompletedPrologue(PAct, EActOutcome::Success);
            continue;
        }


        // Perform prologue
        if (PAct->CanPerformImpl()) {
            PrologueActs.Remove(PAct);
            PendingPrologueActs.Add(PAct);
            PAct->PerformImpl();
            continue;
        }


        // Exit with failure if failed to perform
        Redirect(EActStatus::Exiting, EActOutcome::Failure);
        return;
    }
}
void UAct::CompletedPrologue(UAct* PAct, EActOutcome NewOutcome) {

    // Guard
    UE_STATUS_SAFEGUARD(Status, EActStatus::Prologuing);


    // Remove from pending and move to completed
    PendingPrologueActs.Remove(PAct);


    // Broadcast prologue completed
    OnPrologueComplete.Broadcast(this, PAct, NewOutcome);
    UE_STATUS_SAFEGUARD(Status, EActStatus::Prologuing);

    OnPrologueCompleteBP.Broadcast(this, PAct, NewOutcome);
    UE_STATUS_SAFEGUARD(Status, EActStatus::Prologuing);


    // Exit if prologue act did not succeed
    if (NewOutcome != EActOutcome::Success) {
        Redirect(EActStatus::Exiting, NewOutcome);
        return;
    }


    // Wait for all prologues to complete
    if (PendingPrologueActs.Num() != 0 || PrologueActs.Num() != 0) {
        return;
    }


    // Broadcast post prologue
    OnPostPrologue.Broadcast(this);
    UE_STATUS_SAFEGUARD(Status, EActStatus::Prologuing);  // Guard

    OnPostPrologueBP.Broadcast(this);
    UE_STATUS_SAFEGUARD(Status, EActStatus::Prologuing);  // Guard


    // Redirect to enter
    Redirect(EActStatus::Entering);
}
void UAct::EnterImpl() {

    // Broadcast pre enter
    OnPreEnter.Broadcast(this);
    UE_STATUS_SAFEGUARD(Status, EActStatus::Entering);  // Guard

    OnPreEnterBP.Broadcast(this);
    UE_STATUS_SAFEGUARD(Status, EActStatus::Entering);  // Guard


    // Core enter
    EActOutcome NewOutcome = Enter();
    UE_STATUS_SAFEGUARD(Status, EActStatus::Entering);  // Guard


    // Broadcast post enter
    OnPostEnter.Broadcast(this);
    UE_STATUS_SAFEGUARD(Status, EActStatus::Entering);  // Guard

    OnPostEnterBP.Broadcast(this);
    UE_STATUS_SAFEGUARD(Status, EActStatus::Entering);  // Guard


    // Redirect to exit
    if (NewOutcome != EActOutcome::Pending) {
        Redirect(EActStatus::Exiting, NewOutcome);
        return;
    }


    // Return if no ticking
    if (TickFlags == EActTickFlags::None) {
        return;
    }


    // Return if no theater assigned for ticking
    if (Theater == nullptr) {
        WriteLog("Cannot tick, Assign a theater first!");
        return;
    }


    // Redirect to ticking
    Redirect(EActStatus::Ticking);
}
void UAct::HandleTickingImpl() {
    if (CanTick(EActTickFlags::Tick)) {
        TickReqCount++;
        Theater->StageTick(this);
    }
}
void UAct::TickImpl() {

    // Guard
    UE_STATUS_SAFEGUARD(Status, EActStatus::Ticking);


    // Increment tick count
    TickCount++;


    // Save tick request count
    int32 CurrTickReqCount = TickReqCount;


    // Broadcast pre tick
    OnPreTick.Broadcast(this);
    if (Status != EActStatus::Ticking || CurrTickReqCount != TickReqCount) {
        return;  // Guard
    }

    OnPreTickBP.Broadcast(this);
    if (Status != EActStatus::Ticking || CurrTickReqCount != TickReqCount) {
        return;  // Guard
    }


    // Core tick
    EActOutcome NewOutcome = Tick();
    if (Status != EActStatus::Ticking || CurrTickReqCount != TickReqCount) {
        return;  // Guard
    }


    // Broadcast post tick
    OnPostTick.Broadcast(this);
    if (Status != EActStatus::Ticking || CurrTickReqCount != TickReqCount) {
        return;  // Guard
    }


    OnPostTickBP.Broadcast(this);
    if (Status != EActStatus::Ticking || CurrTickReqCount != TickReqCount) {
        return;  // Guard
    }


    // Check if exit was requested
    if (NewOutcome != EActOutcome::Pending) {
        Redirect(EActStatus::Exiting, NewOutcome);
    }
}
void UAct::ExitImpl() {

    // Only exit if coming from enter or tick
    if (PrevStatus == EActStatus::Entering || PrevStatus == EActStatus::Ticking) {

        // Stop ticking
        if (CanTick(EActTickFlags::Tick) && Theater != nullptr) {
            Theater->UnstageTick(this);
        }


        // Broadcast pre exit
        OnPreExitBP.Broadcast(this);
        OnPreExit.Broadcast(this);


        // Core exit
        Exit();


        // Broadcast post exit
        OnPostExitBP.Broadcast(this);
        OnPostExit.Broadcast(this);
    }


    // Cleanup prologues
    FinishPrologues(this, Outcome);
    ClearPrologueChain(this);
    bHasPrecomputedPrologues = false;
    PrologueActs.Empty();
    PendingPrologueActs.Empty();
    CompletedPrologueActs.Empty();


    // Retry
    if (Outcome == EActOutcome::Retry) {
        if (CanPerformImpl(true)) {
            Status = EActStatus::None;
            PerformImpl();
            return;
        }

        // Change outcome to failure since could not retry
        Outcome = EActOutcome::Failure;
    }


    // Unblock & Continue Epilogues
    UnblockOthers();
    ContinueEpilogues(this, Outcome);
    EpilogueActs.Empty();
    PendingEpilogueActs.Empty();


    // Reset status
    Status = EActStatus::None;


    // Let theater know this act has ended
    if (IsValid(Theater)) {
        Theater->UnstageOngoing(this);
    }


    // Broadcast perform end
    OnPerformEndBP.Broadcast(this);
    OnPerformEnd.Broadcast(this);
}
void UAct::Redirect(EActStatus NewStatus, EActOutcome NewOutcome) {

    // None -> prologue
    if (Status == EActStatus::None && NewStatus == EActStatus::Prologuing) {
        PrevStatus = Status;
        Status = EActStatus::Prologuing;
        Outcome = EActOutcome::Pending;
        PrologueImpl();
    }

    // prologue -> Enter
    else if (Status == EActStatus::Prologuing && NewStatus == EActStatus::Entering) {
        PrevStatus = Status;
        Status = EActStatus::Entering;
        EnterImpl();
    }

    // Enter -> Tick
    else if (Status == EActStatus::Entering && NewStatus == EActStatus::Ticking) {
        PrevStatus = Status;
        Status = EActStatus::Ticking;
        HandleTickingImpl();
    }

    // prologue or Enter or Tick -> Exit
    else if ((Status == EActStatus::Prologuing || Status == EActStatus::Entering || Status == EActStatus::Ticking) && NewStatus == EActStatus::Exiting) {
        PrevStatus = Status;
        Status = EActStatus::Exiting;
        Outcome = NewOutcome;
        ExitImpl();
    }
}
