#include "Theater.h"
#include "Act.h"


// Public methods
bool UTheater::IsEnabled() const
{
	return bIsEnabled;
}
void UTheater::SetEnabled(bool bNewEnabled)
{
	if(bNewEnabled == bIsEnabled)
	{
		return;
	}

	bIsEnabled = bNewEnabled;

	if(!bIsEnabled)
	{
		AbortAll();
	}

	OnEnableChangedBP.Broadcast(this, bIsEnabled);
	OnEnableChanged.Broadcast(this, bIsEnabled);
}
void UTheater::AbortAll()
{
	for(UAct* UAct : AllActs)
	{
		UAct->Abort();
	}
}
bool UTheater::AreAnyOngoing() const
{
	return OngoingActs.Num() != 0;
}
TSet<UAct*> UTheater::GetAllActs() const
{
	return AllActs;
}



// Private staging methods
void UTheater::AddAct(UAct* NewAct)
{
	AllActs.Add(NewAct);
}
void UTheater::RemoveAct(UAct* OldAct)
{
	AllActs.Remove(OldAct);
}
void UTheater::StageDeferred(UAct* UAct, EActTickFlags Flag)
{
	if(UAct == nullptr)
	{
		return;
	}

	DeferredActs.Add(UAct, Flag);
}
void UTheater::UnstageDeferred(UAct* UAct)
{
	if(UAct == nullptr)
	{
		return;
	}

	DeferredActs.Remove(UAct);
}
void UTheater::StageTick(UAct* UAct)
{
	if(UAct == nullptr)
	{
		return;
	}

	StagedTickActs.Add(UAct, true);
}
void UTheater::UnstageTick(UAct* UAct)
{
	if(UAct == nullptr)
	{
		return;
	}


	// Remove if not reference swapped yet else mark as pending removal
	if(StagedTickActs.Contains(UAct))
	{
		StagedTickActs.Remove(UAct);
	}
	else if(ActsToTick.Contains(UAct))
	{
		StagedTickActs.Add(UAct, false);
	}
}
void UTheater::StageOngoing(UAct* UAct)
{
	// Return if invalid act or already ongoing
	if(UAct == nullptr || OngoingActs.Contains(UAct))
	{
		return;
	}


	// Mark as ongoing act
	OngoingActs.Add(UAct);


	// Clear defer
	UnstageDeferred(UAct);


	// Broadcast act started
	OnPerformStartBP.Broadcast(this, UAct);
	OnPerformStart.Broadcast(this, UAct);
}
void UTheater::UnstageOngoing(UAct* UAct)
{
	// Remove as ongoing act
	OngoingActs.Remove(UAct);


	// Broadcast act ended
	OnPerformEndBP.Broadcast(this, UAct);
	OnPerformEnd.Broadcast(this, UAct);


	// Broadcast all ended if none ongoing
	if(!AreAnyOngoing())
	{
		OnAllPerformEndBP.Broadcast(this);
		OnAllPerformEnd.Broadcast(this);
	}
}



// Private tick methods
void UTheater::TickActs()
{
	// Return if no act to tick
	if(StagedTickActs.Num() == 0)
	{
		return;
	}


	// Reference swap to avoid mutation
	ActsToTick = StagedTickActs;
	StagedTickActs.Empty();


	// Tick all acts
	for(auto& Pair : ActsToTick)
	{
		Pair.Key->TickImpl();
	}


	// Merge back
	MergeMap(StagedTickActs, ActsToTick, false);


	// Clear
	ActsToTick.Empty();


	// Filter
	TArray<UAct*> Filter;
	for(auto& Pair : StagedTickActs)
	{
		if(!Pair.Value)
		{
			Filter.Add(Pair.Key);
		}
	}

	for(UAct* UAct : Filter)
	{
		StagedTickActs.Remove(UAct);
	}
}
void UTheater::DeferActs(EActTickFlags Flag)
{
	// Return if no acts to defer
	if(DeferredActs.Num() == 0)
	{
		return;
	}


	// Reference swap to avoid mutation
	TMap<UAct*, EActTickFlags> ActsToDefer = DeferredActs;
	DeferredActs.Empty();


	// Defer perform acts
	TArray<UAct*> Filter;
	for(auto& Pair : ActsToDefer)
	{
		if(EnumHasAnyFlags(Pair.Value, Flag))
		{
			Pair.Key->Perform();
			Filter.Add(Pair.Key);
		}
	}


	// Filter out
	for(UAct* UAct : Filter)
	{
		ActsToDefer.Remove(UAct);
	}


	// Merge back unperformed
	MergeMap(DeferredActs, ActsToDefer, false);
}



// Private override methods
void UTheater::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TickActs();
	DeferActs(EActTickFlags::Tick);
}
