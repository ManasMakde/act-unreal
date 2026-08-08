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


#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Theater.generated.h"


// Forward declarations
class UAct;
enum class EActTickFlags : uint8;


// Delegate signatures
UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTheaterEnableBP, UTheater*, Theater, bool, bNewIsEnabled);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTheaterEnable, UTheater* /* Theater */, bool /* bNewIsEnabled */);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTheaterActBP, UTheater*, Theater, UAct*, Act);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTheaterAct, UTheater* /* Theater */, UAct* /* Act */);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTheaterBP, UTheater*, Theater);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTheater, UTheater* /* Theater */);



// Classes
UCLASS(meta = (BlueprintSpawnableComponent))
class ACTDEMO_API UTheater : public UActorComponent {

    GENERATED_BODY()

   public:
    // Friends
    friend UAct;


    // Constructors
    UTheater();


    // Public Delegates
    UPROPERTY(BlueprintAssignable, Category = "Theater")
    FOnTheaterEnableBP OnEnableChangedBP;

    UPROPERTY(BlueprintAssignable, Category = "Theater")
    FOnTheaterActBP OnPerformStartBP;

    UPROPERTY(BlueprintAssignable, Category = "Theater")
    FOnTheaterActBP OnPerformEndBP;

    UPROPERTY(BlueprintAssignable, Category = "Theater")
    FOnTheaterBP OnAllPerformEndBP;

    FOnTheaterEnable OnEnableChanged;
    FOnTheaterAct OnPerformStart;
    FOnTheaterAct OnPerformEnd;
    FOnTheater OnAllPerformEnd;


    // Public Methods
    UFUNCTION(BlueprintPure, Category = "Theater")
    bool IsEnabled() const;

    UFUNCTION(BlueprintCallable, Category = "Theater")
    void SetEnabled(bool bNewEnabled);

    UFUNCTION(BlueprintCallable, Category = "Theater")
    void AbortAll();

    UFUNCTION(BlueprintPure, Category = "Theater")
    bool AreAnyOngoing() const;

    UFUNCTION(BlueprintPure, Category = "Theater")
    TSet<UAct*> GetAllActs() const;


   private:
    // Private Properties
    UPROPERTY(VisibleAnywhere, Category = "Theater", meta = (AllowPrivateAccess = "true"))
    TSet<UAct*> AllActs;

    UPROPERTY(VisibleAnywhere, Category = "Theater", meta = (AllowPrivateAccess = "true"))
    TSet<UAct*> OngoingActs;

    UPROPERTY(VisibleAnywhere, Category = "Theater", meta = (AllowPrivateAccess = "true"))
    TMap<UAct*, bool> PendingModActs;

    UPROPERTY(VisibleAnywhere, Category = "Theater", meta = (AllowPrivateAccess = "true"))
    TMap<UAct*, EActTickFlags> StagedDeferActs;

    UPROPERTY(VisibleAnywhere, Category = "Theater", meta = (AllowPrivateAccess = "true"))
    TMap<UAct*, bool> StagedTickActs;

    UPROPERTY(VisibleAnywhere, Category = "Theater", meta = (AllowPrivateAccess = "true"))
    TMap<UAct*, EActTickFlags> ActsToDefer;

    UPROPERTY(VisibleAnywhere, Category = "Theater", meta = (AllowPrivateAccess = "true"))
    TMap<UAct*, bool> ActsToTick;

    UPROPERTY(VisibleAnywhere, Category = "Theater", meta = (AllowPrivateAccess = "true"))
    TArray<UAct*> DeferFilterList;

    UPROPERTY(VisibleAnywhere, Category = "Theater", meta = (AllowPrivateAccess = "true"))
    TArray<UAct*> TickFilterList;

    UPROPERTY(VisibleAnywhere, Category = "Theater", meta = (AllowPrivateAccess = "true"))
    bool bIsAbortingAll = false;

    UPROPERTY(VisibleAnywhere, Category = "Theater", meta = (AllowPrivateAccess = "true"))
    bool bIsEnabled = true;


    // Private Staging Methods
    void AddAct(UAct* NewAct);
    void RemoveAct(UAct* OldAct);
    void StageOngoing(UAct* Act);
    void UnstageOngoing(UAct* Act);
    void StageDeferred(UAct* Act, EActTickFlags Flag);
    void UnstageDeferred(UAct* Act);
    void StageTick(UAct* Act);
    void UnstageTick(UAct* Act);


    // Private Methods
    static void TickActs(TMap<UAct*, bool>& StagedActs, TMap<UAct*, bool>& ActsToTickInOut, TArray<UAct*>& FilterList);
    static void DeferActs(TMap<UAct*, EActTickFlags>& DeferredActs, TMap<UAct*, EActTickFlags>& ActsToDeferInOut, TArray<UAct*>& FilterList, EActTickFlags Flag);

    template <typename ValueType>
    static void MergeMap(TMap<UAct*, ValueType>& Dict, TMap<UAct*, ValueType>& Other, bool bOverwrite) {
        for (auto& Pair : Other) {
            if (bOverwrite || !Dict.Contains(Pair.Key)) {
                Dict.Add(Pair.Key, Pair.Value);
            }
        }
    }


    // Private Override Methods
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
