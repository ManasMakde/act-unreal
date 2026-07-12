#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Theater.generated.h"



// Forward declarations
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
    friend class UAct;



    // Public Delegates
    UPROPERTY(BlueprintAssignable)
    FOnTheaterEnableBP OnEnableChangedBP;

	UPROPERTY(BlueprintAssignable)
    FOnTheaterActBP OnPerformStartBP;

	UPROPERTY(BlueprintAssignable)
	FOnTheaterActBP OnPerformEndBP;

	UPROPERTY(BlueprintAssignable)
	FOnTheaterBP OnAllPerformEndBP;

	FOnTheaterEnable OnEnableChanged;
    FOnTheaterAct OnPerformStart;
	FOnTheaterAct OnPerformEnd;
	FOnTheater OnAllPerformEnd;



    // Public methods
    UFUNCTION(BlueprintCallable, Category = "Theater")
    bool IsEnabled() const;

    UFUNCTION(BlueprintCallable, Category = "Theater")
    void SetEnabled(bool bNewEnabled);

	UFUNCTION(BlueprintCallable, Category = "Theater")
    void AbortAll();

	UFUNCTION(BlueprintCallable, Category = "Theater")
	bool AreAnyOngoing() const;

	UFUNCTION(BlueprintCallable, Category = "Theater")
	TSet<UAct*> GetAllActs() const;



   private:
    // Private properties
    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
    TSet<UAct*> AllActs;

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
	TSet<UAct*> OngoingActs;

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
	TMap<UAct*, EActTickFlags> DeferredActs;

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
	TMap<UAct*, bool> StagedTickActs;

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
	TMap<UAct*, bool> ActsToTick;

    UPROPERTY(VisibleAnywhere, Category = "Act", meta = (AllowPrivateAccess = "true"))
	bool bIsEnabled = true;



	// Private Staging Methods
    void AddAct(UAct* NewAct);
    void RemoveAct(UAct* OldAct);
    void StageDeferred(UAct* UAct, EActTickFlags Flag);
    void UnstageDeferred(UAct* UAct);
    void StageTick(UAct* UAct);
    void UnstageTick(UAct* UAct);
	void StageOngoing(UAct* UAct);
    void UnstageOngoing(UAct* UAct);



    // Private Methods
    void TickActs();
    void DeferActs(EActTickFlags Flag);



    // Private Override Methods
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;



	// Private Template Methods
	template<typename ValueType>
	void MergeMap(TMap<UAct*, ValueType>& Dict, TMap<UAct*, ValueType>& Other, bool bOverwrite)
	{
		for(auto& Pair : Other)
		{
			if(bOverwrite || !Dict.Contains(Pair.Key))
			{
				Dict.Add(Pair.Key, Pair.Value);
			}
		}
	}
};
