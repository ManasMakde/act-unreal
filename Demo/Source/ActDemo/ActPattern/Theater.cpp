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


#include "Theater.h"
#include "Act.h"


// Constructors
UTheater::UTheater() {
    PrimaryComponentTick.bCanEverTick = true;
}


// Public Methods
bool UTheater::IsEnabled() const {
    return bIsEnabled;
}
void UTheater::SetEnabled(bool bNewEnabled) {

    if (bNewEnabled == bIsEnabled) {
        return;
    }

    bIsEnabled = bNewEnabled;

    if (!bIsEnabled) {
        AbortAll();
    }

    OnEnableChangedBP.Broadcast(this, bIsEnabled);
    OnEnableChanged.Broadcast(this, bIsEnabled);
}
void UTheater::AbortAll() {

    // Return if already in between aborting all
    if (bIsAbortingAll) {
        return;
    }


    // Guard to avoid mutation
    bIsAbortingAll = true;


    // Abort all acts
    for (UAct* Act : AllActs) {
        Act->Abort();
    }


    // Reset guard
    bIsAbortingAll = false;


    // Apply pending adds & removes after loop
    for (auto& Pair : PendingModActs) {
        if (Pair.Value) {
            AllActs.Add(Pair.Key);
        }
        else {
            AllActs.Remove(Pair.Key);
        }
    }
    PendingModActs.Empty();
}
bool UTheater::AreAnyOngoing() const {
    return OngoingActs.Num() != 0;
}
TSet<UAct*> UTheater::GetAllActs() const {
    return TSet<UAct*>(AllActs);
}



// Private Staging Methods
void UTheater::AddAct(UAct* NewAct) {

    // Return if null act
    if (NewAct == nullptr) {
        UE_LOG(LogTemp, Warning, TEXT("[Theater] Failed to add act, null act provided!"));
        return;
    }


    // Mark as pending if abort all is ongoing
    if (bIsAbortingAll) {
        PendingModActs.Add(NewAct, true);
        return;
    }

    AllActs.Add(NewAct);
}
void UTheater::RemoveAct(UAct* OldAct) {

    // Return if null act
    if (OldAct == nullptr) {
        UE_LOG(LogTemp, Warning, TEXT("[Theater] Failed to remove act, null act provided!"));
        return;
    }


    // Mark as pending if abort all is ongoing
    if (bIsAbortingAll) {
        PendingModActs.Add(OldAct, false);
        return;
    }


    // Unstage from everything
    UnstageDeferred(OldAct);
    UnstageTick(OldAct);

    AllActs.Remove(OldAct);
}
void UTheater::StageOngoing(UAct* Act) {

    // Return if invalid act or already ongoing
    if (Act == nullptr || OngoingActs.Contains(Act)) {
        return;
    }


    // Mark as ongoing act
    OngoingActs.Add(Act);


    // Broadcast act started
    OnPerformStartBP.Broadcast(this, Act);
    OnPerformStart.Broadcast(this, Act);
}
void UTheater::UnstageOngoing(UAct* Act) {

    // Return if act is null or was never staged ongoing
    if (Act == nullptr || !OngoingActs.Contains(Act)) {
        return;
    }


    // Remove as ongoing act
    OngoingActs.Remove(Act);


    // Broadcast act ended
    OnPerformEndBP.Broadcast(this, Act);
    OnPerformEnd.Broadcast(this, Act);


    // Broadcast all ended if none ongoing
    if (!AreAnyOngoing()) {
        OnAllPerformEndBP.Broadcast(this);
        OnAllPerformEnd.Broadcast(this);
    }
}
void UTheater::StageDeferred(UAct* Act, EActTickFlags Flag) {
    if (Act == nullptr || Flag == EActTickFlags::None) {
        return;
    }

    StagedDeferActs.Add(Act, StagedDeferActs.Contains(Act) ? (StagedDeferActs.FindChecked(Act) | Flag) : Flag);
}
void UTheater::UnstageDeferred(UAct* Act) {

    if (Act == nullptr) {
        return;
    }

    StagedDeferActs.Remove(Act);
}
void UTheater::StageTick(UAct* Act) {

    if (Act == nullptr) {
        return;
    }

    StagedTickActs.Add(Act, true);
}
void UTheater::UnstageTick(UAct* Act) {

    if (Act == nullptr) {
        return;
    }


    // Mark as pending removal if reference swapped else Remove
    if (ActsToTick.Contains(Act)) {
        StagedTickActs.Add(Act, false);
    }
    else if (StagedTickActs.Contains(Act)) {
        StagedTickActs.Remove(Act);
    }
}



// Private Methods
void UTheater::TickActs(TMap<UAct*, bool>& StagedActs, TMap<UAct*, bool>& ActsToTickInOut, TArray<UAct*>& FilterList) {

    // Return if no act to process
    if (StagedActs.Num() == 0) {
        return;
    }


    // Reference swap to avoid mutation
    Swap(StagedActs, ActsToTickInOut);


    // Tick all acts
    for (auto& Pair : ActsToTickInOut) {
        Pair.Key->TickImpl();
    }


    // Merge back & clear
    MergeMap(StagedActs, ActsToTickInOut, false);
    ActsToTickInOut.Empty();


    // Filter using reused list to avoid alloc
    FilterList.Empty();
    for (auto& Pair : StagedActs) {
        if (!Pair.Value) {
            FilterList.Add(Pair.Key);
        }
    }
    for (UAct* Act : FilterList) {
        StagedActs.Remove(Act);
    }
}
void UTheater::DeferActs(TMap<UAct*, EActTickFlags>& DeferredActs, TMap<UAct*, EActTickFlags>& ActsToDeferInOut, TArray<UAct*>& FilterList, EActTickFlags Flag) {

    // Return if no acts to defer
    if (DeferredActs.Num() == 0) {
        return;
    }


    // Reference swap to avoid mutation
    Swap(DeferredActs, ActsToDeferInOut);


    // Defer perform acts using reused list to avoid alloc
    FilterList.Empty();
    for (auto& Pair : ActsToDeferInOut) {
        if (EnumHasAnyFlags(Pair.Value, Flag)) {
            Pair.Key->Perform();
            FilterList.Add(Pair.Key);
        }
    }


    // Filter out
    for (UAct* Act : FilterList) {
        ActsToDeferInOut.Remove(Act);
    }


    // Merge back unperformed
    MergeMap(DeferredActs, ActsToDeferInOut, false);
    ActsToDeferInOut.Empty();
}



// Private Override Methods
void UTheater::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    TickActs(StagedTickActs, ActsToTick, TickFilterList);
    DeferActs(StagedDeferActs, ActsToDefer, DeferFilterList, EActTickFlags::Tick);
}
