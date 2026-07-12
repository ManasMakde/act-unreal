#include "Act.h"
#include "Theater.h"


// Public
void UAct::Init(UTheater* NewTheater, const FString& InName, bool bInitiallyEnabled) {

    // Warn if null theater provided
    if (NewTheater == nullptr) {
        UE_LOG(LogTemp, Warning, TEXT("%s Null theater provided for initialization"), *InName);
        return;
    }


    // Assign new owning theater
    Theater = NewTheater;
    Theater->AddAct(this);


    // Assign new name
    Name = InName;


    // Disable initially
    if (!bInitiallyEnabled) {
        BlockSelf(this, EActBlockType::Persistent);
    }


    // Broadcast pre setup
    OnPreSetupBP.Broadcast(this);
    OnPreSetup.Broadcast(this);


    // Core setup
    Setup();


    // Broadcast post setup
    OnPostSetupBP.Broadcast(this);
    OnPostSetup.Broadcast(this);
}
void UAct::Deinit() {

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
    }
    Theater = nullptr;


    // Reset performed on ticks
    PerformedOnTick = -1;
}
void UAct::Perform() {
    if (CanPerformImpl()) {
        PerformImpl();
    }
}
void UAct::PerformDeferred(EActTickFlags TickFlag) {
    if (IsValid(Theater)) {
        Theater->StageDeferred(this, TickFlag);
    }
}
void UAct::Retry() {
    if (IsOngoing()) {
        Finish(EActOutcome::Retry);
    }
    else {
        Perform();
    }
}
void UAct::Abort() {
    Finish(EActOutcome::Interrupted);
}
void UAct::AddToBlock(const TArray<UAct*>& Acts, EActBlockType BlockType) {
    for (UAct* BAct : Acts) {
        // Skip if self reserved for enable disable
        if (BAct == this) {
            UE_LOG(LogTemp, Warning, TEXT("%s Trying to block self"), *Name);
            continue;
        }


        // Add to block list
        ActsToBlock.Add(BAct, BlockType);
    }
}
void UAct::RemoveFromBlock(const TArray<UAct*>& Acts) {
    for (UAct* BAct : Acts) {
        // Skip if self
        if (BAct == this) {
            continue;
        }


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
bool UAct::DidPerform(EActTickFlags TickFlag) {

    // Return false if no flag provided
    if (TickFlag == EActTickFlags::None) {
        return false;
    }


    // Check based on tick types
    bool bPerformed = false;
    if (EnumHasAnyFlags(TickFlag, EActTickFlags::Tick)) {
        bPerformed = bPerformed || PerformedOnTick == static_cast<int64>(GFrameCounter);
    }

    return bPerformed;
}
bool UAct::DidPerformEver() const {
    return PerformedOnTick != -1;
}
bool UAct::IsOngoing() const {
    return Status != EActStatus::None;
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
bool UAct::DidEnter() const {
    return bDidEnter;
}
bool UAct::CanTick(EActTickFlags Type) const {
    return EnumHasAnyFlags(TickFlags, Type);
}
EActOutcome UAct::GetOutcome() const {
    return Outcome;
}
UTheater* UAct::GetTheater() const {
    return Theater;
}
AActor* UAct::GetOwner() const {
    return IsValid(Theater) ? Theater->GetOwner() : nullptr;
}
float UAct::GetDelta() const {
    return IsValid(Theater) ? Theater->GetWorld()->GetDeltaSeconds() : -1;
}
FString UAct::GetName() const {
    return Name;
}
TArray<UAct*> UAct::SeqBP(TArray<FActArray> PArrays) {

    // Convert to plain nested array
    TArray<TArray<UAct*>> PlainArrays;
    for (FActArray& PArr : PArrays) {
        PlainArrays.Add(PArr.Acts);
    }

    return Seq(PlainArrays);
}
TArray<UAct*> UAct::Seq(TArray<TArray<UAct*>> PArrays) {

    // Remove empty lists before chaining
    PArrays.RemoveAll([](const TArray<UAct*>& PArr) { return PArr.Num() == 0; });


    // Return if empty list
    int32 PLength = PArrays.Num();
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
    return EActOutcome::Pending;
}
void UAct::Exit_Implementation() {
}
void UAct::Cleanup_Implementation() {
}
void UAct::Finish(EActOutcome NewOutcome) {

    // If currently prologuing
    if (Status == EActStatus::Prologuing) {
        ContinuePrologue(nullptr, NewOutcome);
    }

    // If currently entering or ticking
    else if (Status == EActStatus::Entering || Status == EActStatus::Ticking) {
        Redirect(EActStatus::Exiting, NewOutcome);
    }
}
void UAct::BlockSelf_Implementation(UAct* ByAct, EActBlockType BlockType) {

    // Return if already blocked or if both are in the same prologue chain
    if (BlockedByActs.Contains(ByAct) || InSamePrologueChain(this, ByAct)) {
        return;
    }


    // Finish interrupted incase ongoing
    Finish(EActOutcome::Interrupted);


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
        Pair.Key->UnblockSelf(this);
    }
}



// Private
void UAct::LinkPrologueArrays(const TArray<UAct*>& ArrayB, const TArray<UAct*>& ArrayA) {
    for (int32 i = 0; i < ArrayB.Num(); i++) {
        UAct* ActB = ArrayB[i];
        for (int32 j = 0; j < ArrayA.Num(); j++) {
            UAct* ActA = ArrayA[j];
            AssignPrologueEpilogue(ActB, ActA);
        }
    }
}
bool UAct::InSamePrologueChain(UAct* ActA, UAct* ActB) {

    // Incase both are the same acts
    if (ActA == ActB) {
        return false;
    }


    // Incase act a is a top epilogue
    if (ActA->EpilogueActs.Num() == 0 && ActB->TopEpilogueActs.Contains(ActA)) {
        return true;
    }


    // Incase act b is a top epilogue
    if (ActB->EpilogueActs.Num() == 0 && ActA->TopEpilogueActs.Contains(ActB)) {
        return true;
    }


    // Check for overlap in top epilogue of both
    for (UAct* EAct : ActA->TopEpilogueActs) {
        if (ActB->TopEpilogueActs.Contains(EAct)) {
            return true;
        }
    }

    return false;
}
void UAct::FinishEpilogues(UAct* OfAct, EActOutcome NewOutcome) {

    // Copy since ContinuePrologue may mutate epilogue set
    TArray<UAct*> EpiloguesCopy = OfAct->EpilogueActs.Array();
    for (UAct* EAct : EpiloguesCopy) {
        if (EAct != nullptr) {
            EAct->ContinuePrologue(OfAct, NewOutcome);
        }
    }
}
void UAct::FinishPrologues(UAct* OfAct, EActOutcome NewOutcome) {

    // Copy since Finish may mutate prologue set
    TArray<UAct*> ProloguesCopy = OfAct->PrologueActs.Array();
    for (UAct* PAct : ProloguesCopy) {
        if (PAct != nullptr) {
            PAct->Finish(NewOutcome);
        }
    }
}
void UAct::ClearPrologueChain(UAct* OfAct) {

    // Recurse clear
    for (UAct* PAct : OfAct->PrologueActs) {
        if (PAct != nullptr) {
            ClearPrologueChain(PAct);
        }
    }

    OfAct->EpilogueActs.Empty();
    OfAct->TopEpilogueActs.Empty();
    OfAct->PrologueActs.Empty();
}
void UAct::AssignPrologueEpilogue(UAct* EAct, UAct* PAct) {

    // Assign prologue
    EAct->PrologueActs.Add(PAct);


    // Assign epilogue
    PAct->EpilogueActs.Add(EAct);
}
void UAct::AssignTopEpilogues(UAct* EAct, TSet<UAct*> TopEpilogues) {

    // Get top epilogues to pass on
    if (TopEpilogues.Num() == 0) {
        if (EAct->EpilogueActs.Num() == 0) {
            TopEpilogues.Add(EAct);
        }
        else {
            TopEpilogues = EAct->TopEpilogueActs;
        }
    }


    // Recurse into prologues
    for (UAct* PAct : EAct->PrologueActs) {

        // Skip null
        if (PAct == nullptr) {
            continue;
        }


        // Assign top epilogues
        PAct->TopEpilogueActs.Append(TopEpilogues);


        // Recurse further down chain
        AssignTopEpilogues(PAct, TopEpilogues);
    }
}
bool UAct::CanPerformImpl() {

    // Return if null theater
    if (!IsValid(Theater)) {
        UE_LOG(LogTemp, Warning, TEXT("%s Null theater found, initialize first!"), *Name);
        return false;
    }


    // Return conditions
    if (!IsEnabled() || !Theater->IsEnabled() || IsBlocked() || (!bCanReperform && IsOngoing())) {
        return false;
    }


    // Return if any external BP condition is false
    for (FPerformCondition& CondBP : PerformConditionsBP) {
        if (CondBP.IsBound() && !CondBP.Execute(this)) {
            return false;
        }
    }


    // Return if any external condition is false
    for (TFunction<bool(UAct*)>& Cond : PerformConditions) {
        if (!Cond(this)) {
            return false;
        }
    }

    return CanPerform();
}
void UAct::PerformImpl() {

    // Store tick
    PerformedOnTick = static_cast<int64>(GFrameCounter);


    // Mark outcome as pending
    Outcome = EActOutcome::Pending;


    // Finish any ongoing perform
    Finish(EActOutcome::Interrupted);


    // Redirect to prologue
    Redirect(EActStatus::Prologuing);
}
void UAct::PrologueImpl() {

    // Let theater know this act is now ongoing
    if (IsValid(Theater)) {
        Theater->StageOngoing(this);
    }
    UE_STATUS_SAFEGUARD(Status, EActStatus::Prologuing)  // Guard


	// Assign all prologues & epilogues
    TArray<UAct*> AllPrologues = PrologueBP.IsBound() ? PrologueBP.Execute(this) : TArray<UAct*>();
    AllPrologues.Append(Prologue(this));
    for (UAct* PAct : AllPrologues) {

        // Skip self
        if (PAct == this) {
            continue;
        }

        // Fail incase null
        if (PAct == nullptr) {
            Redirect(EActStatus::Exiting, EActOutcome::Failure);
            return;
        }

        // Assign prologue & epilogue
        AssignPrologueEpilogue(this, PAct);
    }


    // Assign all top epilogues
    AssignTopEpilogues(this);


    // Block
    BlockOthers();
    UE_STATUS_SAFEGUARD(Status, EActStatus::Prologuing)  // Guard


    // Skip if no prologues
    if (PrologueActs.Num() == 0) {
        Redirect(EActStatus::Entering);  // Intentional to skip pre prologue signal
        return;
    }


    // Broadcast pre prologue
    OnPrePrologueBP.Broadcast(this);
    UE_STATUS_SAFEGUARD(Status, EActStatus::Prologuing)  // Guard
    
    OnPrePrologue.Broadcast(this);
    UE_STATUS_SAFEGUARD(Status, EActStatus::Prologuing)  // Guard


    // Perform all prologues
    for (UAct* PAct : PrologueActs) {

        // Skip if ongoing
        if (PAct->IsOngoing()) {
            continue;
        }

        // Fail incase cannot perform
        if (!PAct->CanPerformImpl()) {
            Redirect(EActStatus::Exiting, EActOutcome::Failure);
            return;
        }

        // Perform
        PAct->PerformImpl();
        UE_STATUS_SAFEGUARD(Status, EActStatus::Prologuing)  // Guard
    }
}
void UAct::ContinuePrologue(UAct* PAct, EActOutcome NewOutcome) {

    // Guard
    UE_STATUS_SAFEGUARD(Status, EActStatus::Prologuing)


    // Wait for all prologues to complete
    bool bPrologueSucceeded = NewOutcome == EActOutcome::Success && PAct != nullptr;
    if (bPrologueSucceeded && PrologueCompleteCount + 1 != PrologueActs.Num()) {
        PrologueCompleteCount += 1;
        return;
    }


    // Broadcast post prologue
    if (bPrologueSucceeded) {
        OnPostPrologueBP.Broadcast(this);
        UE_STATUS_SAFEGUARD(Status, EActStatus::Prologuing)  // Guard

        OnPostPrologue.Broadcast(this);
        UE_STATUS_SAFEGUARD(Status, EActStatus::Prologuing)  // Guard
    }


    // If prologue succeeded goto enter otherwise exit
    Redirect(bPrologueSucceeded ? EActStatus::Entering : EActStatus::Exiting, NewOutcome);
}
void UAct::EnterImpl() {

    // Broadcast pre enter
    OnPreEnterBP.Broadcast(this);
    UE_STATUS_SAFEGUARD(Status, EActStatus::Entering)  // Guard

    OnPreEnter.Broadcast(this);
    UE_STATUS_SAFEGUARD(Status, EActStatus::Entering)  // Guard


    // Core enter
    EActOutcome NewOutcome = Enter();
    UE_STATUS_SAFEGUARD(Status, EActStatus::Entering)  // Guard


    // Broadcast post enter
    OnPostEnterBP.Broadcast(this);
    UE_STATUS_SAFEGUARD(Status, EActStatus::Entering)  // Guard

    OnPostEnter.Broadcast(this);
    UE_STATUS_SAFEGUARD(Status, EActStatus::Entering)  // Guard


    // Redirect to exit
    if (NewOutcome != EActOutcome::Pending) {
        Redirect(EActStatus::Exiting, NewOutcome);
        return;
    }


    // Start ticking
    if (CanTick(EActTickFlags::Tick) && IsValid(Theater)) {
        Theater->StageTick(this);
    }
    UE_STATUS_SAFEGUARD(Status, EActStatus::Entering)  // Guard


    // Redirect to ticking
    Redirect(EActStatus::Ticking);
}
void UAct::TickImpl() {

    // Guard
    UE_STATUS_SAFEGUARD(Status, EActStatus::Ticking)


    // Broadcast pre tick
    OnPreTickBP.Broadcast(this);
    UE_STATUS_SAFEGUARD(Status, EActStatus::Ticking)  // Guard
    OnPreTick.Broadcast(this);
    UE_STATUS_SAFEGUARD(Status, EActStatus::Ticking)  // Guard


    // Core tick
    EActOutcome NewOutcome = Tick();
    UE_STATUS_SAFEGUARD(Status, EActStatus::Ticking)  // Guard


    // Broadcast post tick
    OnPostTickBP.Broadcast(this);
    UE_STATUS_SAFEGUARD(Status, EActStatus::Ticking)  // Guard

    OnPostTick.Broadcast(this);
    UE_STATUS_SAFEGUARD(Status, EActStatus::Ticking)  // Guard


    // Check if exit was requested
    if (NewOutcome != EActOutcome::Pending) {
        Redirect(EActStatus::Exiting, NewOutcome);
    }
}
void UAct::ExitImpl() {

    // Stop ticking
    if (CanTick(EActTickFlags::Tick) && IsValid(Theater)) {
        Theater->UnstageTick(this);
    }
    UE_STATUS_SAFEGUARD(Status, EActStatus::Exiting)  // Guard


    // Broadcast pre exit
    OnPreExitBP.Broadcast(this);
    UE_STATUS_SAFEGUARD(Status, EActStatus::Exiting)  // Guard

    OnPreExit.Broadcast(this);
    UE_STATUS_SAFEGUARD(Status, EActStatus::Exiting)  // Guard


    // Core exit
    Exit();
    UE_STATUS_SAFEGUARD(Status, EActStatus::Exiting)  // Guard


    // Broadcast post exit
    OnPostExitBP.Broadcast(this);
    UE_STATUS_SAFEGUARD(Status, EActStatus::Exiting)  // Guard

    OnPostExit.Broadcast(this);
    UE_STATUS_SAFEGUARD(Status, EActStatus::Exiting)  // Guard


    // Finish epilogues
    if (Outcome != EActOutcome::Retry) {
        FinishEpilogues(this, Outcome);
    }
    UE_STATUS_SAFEGUARD(Status, EActStatus::Exiting)  // Guard


    // Finish prologues
    FinishPrologues(this, Outcome == EActOutcome::Retry ? EActOutcome::Interrupted : Outcome);
    UE_STATUS_SAFEGUARD(Status, EActStatus::Exiting)  // Guard


    // Clear chain
    ClearPrologueChain(this);


    // Reset properties
    bool bToRetry = Outcome == EActOutcome::Retry;
    Status = EActStatus::None;
    bDidEnter = false;
    PrologueCompleteCount = 0;


    // Unblock
    UnblockOthers();


    // Retry perform
    if (bToRetry && CanPerformImpl()) {
        PerformImpl();
        return;
    }


    // Let theater know this act has ended
    Theater->UnstageOngoing(this);
}
void UAct::Redirect(EActStatus NewStatus, EActOutcome NewOutcome) {

    // None to Prologue
    if (Status == EActStatus::None && NewStatus == EActStatus::Prologuing) {
        Status = EActStatus::Prologuing;
        PrologueImpl();
    }

    // Prologue to Enter
    else if (Status == EActStatus::Prologuing && NewStatus == EActStatus::Entering) {
        Status = EActStatus::Entering;
        EnterImpl();
    }

    // Enter to Tick
    else if (Status == EActStatus::Entering && NewStatus == EActStatus::Ticking) {
        Status = EActStatus::Ticking;
    }

    // Prologue or Enter or Tick to Exit
    else if ((Status == EActStatus::Prologuing || Status == EActStatus::Entering || Status == EActStatus::Ticking) && NewStatus == EActStatus::Exiting) {
        bDidEnter = Status == EActStatus::Entering || Status == EActStatus::Ticking;
        Status = EActStatus::Exiting;
        Outcome = NewOutcome;
        ExitImpl();
    }
}
