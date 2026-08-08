# 🎭 Act Unreal Documentation

## ⚙️ UAct Class

| Enums   | Constants |
|---------|----------|
| [EActTickFlags](#eacttickflags) | `None`, `Tick` |
| [EActStatus](#eactstatus) | `None`, `Prologuing`, `Entering`, `Ticking`, `Exiting` |
| [EActOutcome](#eactoutcome) | `Interrupted`, `Failure`, `Pending`, `Success`, `Retry` |
| [EActBlockType](#eactblocktype) | `Interrupt`, `Persistent` |


| Signature    | Events |
|--------------|-------|
| \<UAct* Act\> | [OnPreSetup](#onpresetup) |
| \<UAct* Act\> | [OnPostSetup](#onpostsetup) |
| \<UAct* Act\> | [OnPerformStart](#onperformstartact) |
| \<UAct* Act\> | [OnPrePrologue](#onpreprologue) |
| \<UAct* Act,<br> UAct* PrologueAct,<br> [EActOutcome](#eactoutcome) Outcome\> | [OnPrologueComplete](#onprologuecomplete) |
| \<UAct* Act\> | [OnPostPrologue](#onpostprologue) |
| \<UAct* Act\> | [OnPreEnter](#onpreenter) |
| \<UAct* Act\> | [OnPostEnter](#onpostenter) |
| \<UAct* Act\> | [OnPreTick](#onpretick) |
| \<UAct* Act\> | [OnPostTick](#onposttick) |
| \<UAct* Act\> | [OnPreExit](#onpreexit) |
| \<UAct* Act\> | [OnPostExit](#onpostexit) |
| \<UAct* Act\> | [OnPerformEnd](#onperformendact) |
| \<UAct* Act\> | [OnPreCleanup](#onprecleanup) |
| \<UAct* Act\> | [OnPostCleanup](#onpostcleanup) |
| \<UAct* Act,<br> bool bNewIsEnabled\> | [OnEnableChanged](#onenablechanged) |
| \<UAct* Act,<br> UAct* BlockingAct,<br> [EActBlockType](#eactblocktype) BlockType,<br>bool bDidBlock\> | [OnBlockChanged](#onblockchanged) |


| Access | Type | Properties |
|--------|------|--------------|
| public | TFunction\<TArray\<UAct*\>(UAct*)\> | [Prologue](#prologue) |
| public | TArray\<TFunction\<bool(UAct*)\>\> | [PerformConditions](#performconditions) |
| public | bool | [bIsVerbose](#bisverbose) |
| protected | FString | [Name](#name) |
| protected | bool | [bCanReperform](#bcanreperform) |
| protected | [EActTickFlags](#eacttickflags) | [TickFlags](#tickflags) |


| Access | Type | Methods |
|--------|------|--------------|
| public | void | [Init](#init)(FString NewName, UTheater* NewTheater, bool bInitiallyEnabled) |
| public | void | [Deinit](#deinit)() |
| public | void | [Perform](#perform)() |
| public | void | [PerformDeferred](#performdeferred)([EActTickFlags](#eacttickflags) TickFlag) |
| public | void | [Retry](#retry)() |
| public | void | [Abort](#abort)() |
| public | void | [AddToBlock](#addtoblock)(TArray\<UAct*\> Acts, [EActBlockType](#eactblocktype) BlockType) |
| public | void | [RemoveFromBlock](#removefromblock)(TArray\<UAct*\> Acts) |
| public | void | [SetEnabled](#setenabled)(bool bNewEnabled) |
| public const | bool | [DidPerform](#didperform)([EActTickFlags](#eacttickflags) TickFlag) |
| public const | bool | [IsOngoing](#isongoing)() |
| public const | bool | [IsActive](#isactive)() |
| public const | bool | [IsEnabled](#isenabled)() |
| public const | bool | [IsBlocked](#isblocked)() |
| public const | bool | [CanTick](#cantick)([EActTickFlags](#eacttickflags) Type) |
| public const | UTheater* | [GetTheater](#gettheater)() |
| public const | AActor* | [GetOwner](#getowner)() |
| public const | TSet\<UAct*\> | [GetBlockedByActs](#getblockedbyacts)() |
| public const | TMap\<UAct*, [EActBlockType](#eactblocktype)\> | [GetActsToBlock](#getactstoblock)() |
| public const | [EActStatus](#eactstatus) | [GetStatus](#getstatus)() |
| public const | [EActOutcome](#eactoutcome) | [GetOutcome](#getoutcome)() |
| public const | int32 | [GetPerformCount](#getperformcount)() |
| public const | int32 | [GetTickCount](#gettickcount)() |
| public const | float | [GetDelta](#getdelta)() |
| public const | FString | [GetName](#getname)() |
| public static | TArray\<UAct*\> | [Seq](#seq)(TArray\<TArray\<UAct*\>\> PArrays) |
| protected<br>BlueprintNativeEvent | void | [Setup](#setup)() <abbr title="">Virtual</abbr> |
| protected<br>BlueprintNativeEvent | bool | [CanPerform](#canperform)() <abbr title="">Virtual</abbr> |
| protected<br>BlueprintNativeEvent | [EActOutcome](#eactoutcome) | [Enter](#enter)() <abbr title="">Virtual</abbr> |
| protected<br>BlueprintNativeEvent | [EActOutcome](#eactoutcome) | [Tick](#tick)() <abbr title="">Virtual</abbr> |
| protected<br>BlueprintNativeEvent | void | [Exit](#exit)() <abbr title="">Virtual</abbr> |
| protected<br>BlueprintNativeEvent | void | [Cleanup](#cleanup)() <abbr title="">Virtual</abbr> |
| protected | void | [Finish](#finish)([EActOutcome](#eactoutcome) NewOutcome) |
| protected<br>BlueprintNativeEvent | void | [BlockSelf](#blockself)(UAct* ByAct, [EActBlockType](#eactblocktype) BlockType) <abbr title="">Virtual</abbr> |
| protected<br>BlueprintNativeEvent | void | [UnblockSelf](#unblockself)(UAct* ByAct) <abbr title="">Virtual</abbr> |
| protected<br>BlueprintNativeEvent | void | [BlockOthers](#blockothers)() <abbr title="">Virtual</abbr> |
| protected<br>BlueprintNativeEvent | void | [UnblockOthers](#unblockothers)() <abbr title="">Virtual</abbr> |
| protected<br>BlueprintNativeEvent | void | [WriteLog](#writelog)(FString Message, FString OverrideName) <abbr title="">Virtual</abbr> |


<br/>


## ⚙️ UTheater Class

| Signature    | Events |
|--------------|-------|
| \<UTheater* Theater, bool bNewIsEnabled\> | [OnEnableChanged](#onenablechangedtheater) |
| \<UTheater* Theater, UAct* Act\> | [OnPerformStart](#onperformstart) |
| \<UTheater* Theater, UAct* Act\> | [OnPerformEnd](#onperformend) |
| \<UTheater* Theater\> | [OnAllPerformEnd](#onallperformend) |


| Access | Type | Methods |
|--------|------|--------------|
| public const | bool | [IsEnabled](#isenabledtheater)() |
| public | void | [SetEnabled](#setenabledtheater)(bool bNewEnabled) |
| public | void | [AbortAll](#abortall)() |
| public const | bool | [AreAnyOngoing](#areanyongoing)() |
| public const | TSet\<UAct*\> | [GetAllActs](#getallacts)() |


<br/>


## 📖 UAct Descriptions

### <a id="eacttickflags"></a> enum class EActTickFlags
- `None`: Indicates no ticking should occur.
- `Tick`: Indicates [`TickComponent()`][UE-TickComponent] should be invoked for the act (via the owning `Theater`).


---


### <a id="eactstatus"></a> enum class EActStatus
- `None`: Indicates the act is not ongoing.
- `Prologuing`: Indicates the act is waiting on pending prologues to complete.
- `Entering`: Indicates the act is carrying out its core behaviour.
- `Ticking`: Indicates the act is ticking within [`Tick()`](#tick).
- `Exiting`: Indicates the act perform has ended and is now finalizing.


---


### <a id="eactoutcome"></a> enum class EActOutcome
- `Interrupted`: Indicates the act was interrupted externally while performing.
- `Failure`: Indicates the act failed to complete its core behaviour.
- `Pending`: Indicates the act is still pending for its core behaviour to complete which might also indicate ticking if [TickFlags](#tickflags) is assigned.
- `Success`: Indicates the act successfully completed its core behaviour.
- `Retry`: Indicates the act is retrying its core behaviour.


---


### <a id="eactblocktype"></a> enum class EActBlockType
- `Interrupt`: Merely interrupts the act (if ongoing) when the blocker act starts performing.
- `Persistent`: Keeps the act blocked for the entire duration of the blocker act performing.


---


### <a id="onpresetup"></a> public \<UAct* Act\> OnPreSetup
Invoked just before [Setup](#setup)() method is called.

> **Note:** Use `OnPreSetupBP` in Blueprints


---


### <a id="onpostsetup"></a> public \<UAct* Act\> OnPostSetup
Invoked just after [Setup](#setup)() method has been called.

> **Note:** Use `OnPostSetupBP` in Blueprints


---


### <a id="onperformstartact"></a> public \<UAct* Act\> OnPerformStart
Invoked just as [`Perform()`](#perform) has started, before prologuing.

> **Note:** Use `OnPerformStartBP` in Blueprints


---


### <a id="onpreprologue"></a> public \<UAct* Act\> OnPrePrologue
Invoked just before prologue acts start performing.  
Will not be invoked if act has no prologues.

> **Note:** Use `OnPrePrologueBP` in Blueprints


---


### <a id="onprologuecomplete"></a> public \<UAct* Act, UAct* PrologueAct, [EActOutcome](#eactoutcome) Outcome\> OnPrologueComplete
Invoked whenever a single prologue act has completed.

> **Note:** Use `OnPrologueCompleteBP` in Blueprints


---


### <a id="onpostprologue"></a> public \<UAct* Act\> OnPostPrologue
Invoked just after all prologue acts have performed.  
Will not be invoked if act has no prologues or if any of the prologues failed.

> **Note:** Use `OnPostPrologueBP` in Blueprints


---


### <a id="onpreenter"></a> public \<UAct* Act\> OnPreEnter
Invoked just before [Enter](#enter)() method is called.

> **Note:** Use `OnPreEnterBP` in Blueprints


---


### <a id="onpostenter"></a> public \<UAct* Act\> OnPostEnter
Invoked just after [Enter](#enter)() method has been called.

> **Note:** Use `OnPostEnterBP` in Blueprints


---


### <a id="onpretick"></a> public \<UAct* Act\> OnPreTick
Invoked just before [`Tick()`](#tick) method is called.

> **Note:** Use `OnPreTickBP` in Blueprints


---


### <a id="onposttick"></a> public \<UAct* Act\> OnPostTick
Invoked just after [`Tick()`](#tick) method has been called.

> **Note:** Use `OnPostTickBP` in Blueprints


---


### <a id="onpreexit"></a> public \<UAct* Act\> OnPreExit
Invoked just before [`Exit()`](#exit) method is called.

> **Note:** Use `OnPreExitBP` in Blueprints


---


### <a id="onpostexit"></a> public \<UAct* Act\> OnPostExit
Invoked just after [`Exit()`](#exit) method has been called.

> **Note:** Use `OnPostExitBP` in Blueprints


---


### <a id="onperformendact"></a> public \<UAct* Act\> OnPerformEnd
Invoked just as [`Perform()`](#perform) has ended, after exiting.

> **Note:** Use `OnPerformEndBP` in Blueprints


---


### <a id="onprecleanup"></a> public \<UAct* Act\> OnPreCleanup
Invoked just before [Cleanup](#cleanup)() method is called.

> **Note:** Use `OnPreCleanupBP` in Blueprints


---


### <a id="onpostcleanup"></a> public \<UAct* Act\> OnPostCleanup
Invoked just after [Cleanup](#cleanup)() method has been called.

> **Note:** Use `OnPostCleanupBP` in Blueprints


---


### <a id="onenablechanged"></a> public \<UAct* Act, bool bNewIsEnabled\> OnEnableChanged
Invoked whenever the act has been enabled/disabled.

> **Note:** Use `OnEnableChangedBP` in Blueprints


---


### <a id="onblockchanged"></a> public \<UAct* Act, UAct* BlockingAct, [EActBlockType](#eactblocktype) BlockType, bool bDidBlock\> OnBlockChanged
Invoked whenever the act has been blocked/unblocked.

> **Note:** Use `OnBlockChangedBP` in Blueprints


---


### <a id="prologue"></a> public TFunction\<TArray\<UAct*\>(UAct*)\> Prologue
`Default: [](UAct*) { return TArray<UAct*>(); }`

Assign this with a function which returns a list of acts, All acts in that list will be performed in parallel before the main act is performed.  
If the list contains `nullptr` or if any act failed to perform it will be treated as prologuing failed & directly proceeed to [`Exit()`](#exit) with [`GetOutcome()`](#getoutcome) reflecting the failure.

```cpp
MyAct->Prologue = [](UAct* Act) -> TArray<UAct*> {

    if (bToFail) {
        return TArray<UAct*>{ nullptr };  // This will intentionally fail the act
    }

    return TArray<UAct*>{ MyAct1, MyAct2 };  // MyAct1 & MyAct2 will be performed in parallel
};
```

> **Note:** Use `PrologueBP` in Blueprints


---


### <a id="performconditions"></a> public TArray\<TFunction\<bool(UAct*)\>\> PerformConditions
`Default: {}`

Used when overriding [CanPerform](#canperform)() isn't sufficient and additional external conditions are required.
```cpp
void AMyPawn::BeginPlay()
{
    JumpAct->PerformConditions.Add([](UAct* Act) {
        return WasInputActionTriggered(TEXT("Jump"));  // Only jump when jump input triggered
    });
    JumpAct->Init(TEXT("Jump Act"), Theater);
}
```

> **Note:** Use `PerformConditionsBP` in Blueprints



---


### <a id="bisverbose"></a> public bool bIsVerbose
`Default: false`

Controls whether or not to print warnings. Set to `false` to silence them.


---


### <a id="name"></a> protected FString Name
`Default: ""`

Name of the act, Mainly useful for debugging purposes.


---


### <a id="bcanreperform"></a> protected bool bCanReperform
> **Note:** Should only be assigned inside the [`Setup()`](#setup) method.

`Default: false`

If `true` then calling `Perform()` on an ongoing act will abort the act interruptively and then perform.  
If `false` then current ongoing perform must be completed/aborted manually before calling `Perform()` again.


---


### <a id="tickflags"></a> protected EActTickFlags TickFlags
> **Note:** Should only be assigned inside the [`Setup()`](#setup)() method.

`Default: EActTickFlags::None`

Determines whether [`Tick()`](#tick) should be called. Look into [`Enter()`](#enter) & [`EActTickFlags`](#eacttickflags) to learn more.


---


### <a id="init"></a> public void Init(FString NewName = "", UTheater* NewTheater = nullptr, bool bInitiallyEnabled = true)
This method is used to initialize the act. Generally this will be called in [`BeginPlay()`][UE-BeginPlay] though it can be used elsewhere if required.
```cpp
void AMyPawn::BeginPlay()
{
    Super::BeginPlay();

    Theater = FindComponentByClass<UTheater>();
    MyAct->OnPostEnter.AddLambda([](UAct* Act) {
        UE_LOG(LogTemp, Log, TEXT("Before Entering"));
    });
    MyAct->Prologue = [this](UAct* Act) {
        return TArray<UAct*>{ SomeAct };
    };
    MyAct->MyVar = 10;
    MyAct->Init(TEXT("My Act"), Theater);
}
```
i.e. You should ideally set all Delegates, Prologues, Onetime Properties, etc before you call `Init()`.
Calling `Init()` will internally call your overridden `Setup()` method.


---


### <a id="deinit"></a> public void Deinit()
This method is used to deinitialize the act & it must be called before the act is destroyed.  
Generally this will be called in [`EndPlay()`][UE-EndPlay].
```cpp
void AMyPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    MyAct->Deinit();
    Super::EndPlay(EndPlayReason);
}
```
Calling `Deinit()` will internally call your overridden `Cleanup()` method.


---


### <a id="perform"></a> public void Perform()
Call this method when you want your defined act behaviour to run. This will start the perform lifecycle of the act.
```cpp
void AMyPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    MoveAct->Direction = GetDirection();
    MoveAct->Perform();
}
```


---


### <a id="performdeferred"></a> public void PerformDeferred([EActTickFlags](#eacttickflags) TickFlag = EActTickFlags::Tick)
> **Note:** Requires a `Theater` to be assigned.

This will delay off the [`Perform()`](#perform)until the next tick. Useful to avoid infinite recursion when trying to reperform an act.


---


### <a id="retry"></a> public void Retry()
If the act is performing then this function will finish the act with [EActOutcome::Retry](#eactoutcome) which will cause the act to reperform.  
If the act is not performing this will simply call [`Perform()`](#perform).


---


### <a id="abort"></a> public void Abort()
This will finish the act if it's performing with [EActOutcome::Interrupted](#eactoutcome).  
Won't do anything if the act was not performing.


---


### <a id="addtoblock"></a> public void AddToBlock(TArray\<UAct*\> Acts, [EActBlockType](#eactblocktype) BlockType = EActBlockType::Persistent)
Stores which other acts to block while performing.
```cpp
void AMyPawn::BeginPlay()
{
    Super::BeginPlay();

    Theater = FindComponentByClass<UTheater>();
    DamagedAct->AddToBlock({ WalkAct });  // Walking is blocked while player is taking damage
    DamagedAct->Init(TEXT("Damaged Act"), Theater);
}
```

Also look into [EActBlockType](#eactblocktype).


---


### <a id="removefromblock"></a> public void RemoveFromBlock(TArray\<UAct*\> Acts)
Removes given acts from being blocked.


---


### <a id="setenabled"></a> public void SetEnabled(bool bNewEnabled)
Disables/Enables the act i.e. If an act is disabled then it can no longer [`Perform()`](#perform) and any act that was ongoing will be interrupted.
```cpp
MyAct->SetEnabled(false);  // Disable act
MyAct->SetEnabled(true);  // Enable act
```


---


### <a id="didperform"></a> public bool DidPerform([EActTickFlags](#eacttickflags) TickFlag = EActTickFlags::Tick) const
Returns `true` if the act has performed atleast once in the span of the current tick.
```cpp
void AMyPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    UE_LOG(LogTemp, Log, TEXT("%d"), MyAct->DidPerform(EActTickFlags::Tick));  // false
    MyAct->Perform();
    UE_LOG(LogTemp, Log, TEXT("%d"), MyAct->DidPerform(EActTickFlags::Tick));  // true
}
```


---


### <a id="isongoing"></a> public bool IsOngoing() const
Returns `true` if the act is currently performing.


---


### <a id="isactive"></a> public bool IsActive() const
Returns `true` if the act is currently between entering & exiting.


---


### <a id="isenabled"></a> public bool IsEnabled() const
Returns `true` if the act is currently enabled.


---


### <a id="isblocked"></a> public bool IsBlocked() const
Returns `true` if the act is currently blocked by 1 or more other acts.


---


### <a id="cantick"></a> public bool CanTick([EActTickFlags](#eacttickflags) Type) const
Returns `true` if the act can tick on the given flag type(s).


---


### <a id="gettheater"></a> public UTheater* GetTheater() const
> **Note:** Requires a `Theater` to be assigned.

Returns the `UTheater` the act belongs to. returns nullptr if theater not assigned.


---


### <a id="getowner"></a> public AActor* GetOwner() const
> **Note:** Requires a `Theater` to be assigned.

Returns the [`AActor`][UE-Actor] the `Theater` component is attached to. Returns nullptr if theater not assigned.


---


### <a id="getblockedbyacts"></a> public TSet\<UAct*\> GetBlockedByActs() const
Returns a copy of the set of acts currently blocking this act.


---


### <a id="getactstoblock"></a> public TMap\<UAct*, [EActBlockType](#eactblocktype)\> GetActsToBlock() const
Returns a copy of the acts this act will block along with their [EActBlockType](#eactblocktype), As assigned via [AddToBlock](#addtoblock).


---


### <a id="getstatus"></a> public EActStatus GetStatus() const
Returns the current [EActStatus](#eactstatus) of the act.


---


### <a id="getoutcome"></a> public EActOutcome GetOutcome() const
Returns the outcome of [Enter](#enter)() or [`Tick()`](#tick).  
However this is only to be used inside the lifecycle methods since [`Exit()`](#exit) will internally reset the flag.


---


### <a id="getperformcount"></a> public int32 GetPerformCount() const
Returns how many times the act has performed since [initialized](#init).


---


### <a id="gettickcount"></a> public int32 GetTickCount() const
Returns how many times [`Tick()`](#tick) has been called since [initialized](#init).


---


### <a id="getdelta"></a> public float GetDelta() const
Returns [`FApp::GetDeltaTime()`][UE-GetDeltaTime]
(Kept for consistency sake)


---


### <a id="getname"></a> public FString GetName() const
Returns the name of the act as passed to [Init](#init)().
Mainly useful for debugging purposes.


---


### <a id="seq"></a> public static TArray\<UAct*\> Seq(TArray\<TArray\<UAct*\>\> PArrays)
This method is to be used **only** inside [Prologue](#prologue), It allows you to call prologue acts in sequence.
```cpp
MyAct->Prologue = [](UAct* Act) {
    return UAct::Seq({
        { MyActA1 },
        { MyActB1, MyActB2 },
        { MyActC1 },
    });
};
```
In the above example `MyActA1` will perform first,  
then `MyActB1` & `MyActB2` will perform in parallel,  
then `MyActC1` will perform last.  
And then after all prologue acts are complete would `MyAct` be performed.

This is how to do it without using `Seq()`:
```cpp
MyAct->Prologue = [](UAct* Act) {
    return TArray<UAct*>{ MyActC1 };
};

MyActC1->Prologue = [](UAct* Act) {
    return TArray<UAct*>{ MyActB1, MyActB2 };
};

MyActB1->Prologue = [](UAct* Act) {
    return TArray<UAct*>{ MyActA1 };
};

MyActB2->Prologue = [](UAct* Act) {
    return TArray<UAct*>{ MyActA1 };
};
```

> **Note:** Use `SeqBP()` in Blueprints


---


### <a id="setup"></a> protected void Setup()
> **Note:** Override `Setup_Implementation()` in C++ or override the `Setup` event node in Blueprint, Never invoke directly, Except when using `Super::Setup_Implementation()`.

This method is meant to be overridden and should contain your initialization logic inside it.
```cpp
void UMyAct::Setup_Implementation()
{
    bCanReperform = true;
    TickFlags = EActTickFlags::Tick;
    RigidBody = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
    // etc etc
}
```


---


### <a id="canperform"></a> protected bool CanPerform()
> **Note:** Override `CanPerform_Implementation()` in C++ or override the `CanPerform` event node in Blueprint, Never invoke directly, Except when using `Super::CanPerform_Implementation()`.

This method is meant to be overridden and should contain conditions on whether or not `Perform()` can be called.
```cpp
bool URunAct::CanPerform_Implementation()
{
    return IsOnGround();  // Cannot run if not on ground
}
```


---


### <a id="enter"></a> protected [EActOutcome](#eactoutcome) Enter()

> **Note:** Override `Enter_Implementation()` in C++ or override the `Enter` event node in Blueprint, Never invoke directly, Except when using `Super::Enter_Implementation()`.

This method is meant to be overridden and should contain the core behaviour of the act.
The return value dictates the outcome of the act. Possible return values are:
- `EActOutcome::Failure`
- `EActOutcome::Pending`
- `EActOutcome::Success`
- `EActOutcome::Retry`  
Do not return `EActOutcome::Interrupted` that is reserved for external cancellation.

```cpp
EActOutcome URunAct::Enter_Implementation()
{
    bool bDidMove;

    // Run logic here...

    return bDidMove ? EActOutcome::Success : EActOutcome::Failure;
}
```

If you want to use [`Tick()`](#tick) you must:
1. Assign [TickFlags](#tickflags) with something other than [EActTickFlags::None](#eacttickflags).
1. Return [EActOutcome::Pending](#eactoutcome) in `Enter()`, returning anything else will lead to [`Exit()`](#exit).

```cpp
void UGotoAct::Setup_Implementation()
{
    TickFlags = EActTickFlags::Tick;
}
EActOutcome UGotoAct::Enter_Implementation()
{
    if (AtDestination()) {  // Return as success if already at destination
        return EActOutcome::Success;
    }

    return EActOutcome::Pending;
}
EActOutcome UGotoAct::Tick_Implementation()
{
    // Move logic here...

    // Returning Pending continues ticking, returning anything else makes the act proceeed into Exit()
    return ReachedDestination() ? EActOutcome::Success : EActOutcome::Pending;
}
```

If `EActOutcome::Pending` is returned without the intent of ticking, [Finish](#finish)() must be called so the act can proceed to [`Exit()`](#exit).
```cpp
EActOutcome UEmoteAct::Enter_Implementation()
{
    PlayAnimation();

    OnAnimationEnded.AddLambda([this](bool bDidPlay) {
        Finish(bDidPlay ? EActOutcome::Success : EActOutcome::Failure);
    });

    return EActOutcome::Pending;
}
```
Return `EActOutcome::Retry` if you want the act to perform again without continuing with epilogue acts first.


---


### <a id="tick"></a> protected [EActOutcome](#eactoutcome) Tick()
> **Note:** Override `Tick_Implementation()` in C++ or override the `Tick` event node in Blueprint, Never invoke directly, Except when using `Super::Tick_Implementation()`.
> Also requires a `Theater` to be assigned.

This method is meant to be overridden and should contain the ticking logic of the act.
Look into [Enter](#enter)() to understand how the return value works.


---


### <a id="exit"></a> protected void Exit()
> **Note:** Override `Exit_Implementation()` in C++ or override the `Exit` event node in Blueprint, Never invoke directly, Except when using `Super::Exit_Implementation()`.

This method is meant to be overridden and should contain the finialization logic after [Entering](#enter).
```cpp
void UMoveAct::Exit_Implementation()
{
    Direction = FVector2D::ZeroVector;
}
```


---


### <a id="cleanup"></a> protected void Cleanup()
> **Note:** Override `Cleanup_Implementation()` in C++ or override the `Cleanup` event node in Blueprint, Never invoke directly, Except when using `Super::Cleanup_Implementation()`.

This method is meant to be overridden and should contain your deinitialization logic inside it.
```cpp
void UMyAct::Cleanup_Implementation()
{
    RigidBody = nullptr;
    // etc etc
}
```


---


### <a id="finish"></a> protected void Finish([EActOutcome](#eactoutcome) NewOutcome = EActOutcome::Success)
This method is only meant to be invoked in [Enter](#enter)() and should not be overridden.


---


### <a id="blockself"></a> protected void BlockSelf(UAct* ByAct, [EActBlockType](#eactblocktype) BlockType)
This method is used internally, Only kept incase some special functionality needs to be hooked when act is being blocked.
```cpp
void UMyAct::BlockSelf_Implementation(UAct* ByAct, EActBlockType BlockType)
{
    Super::BlockSelf_Implementation(ByAct, BlockType);

    // Custom functionality
}
```

---


### <a id="unblockself"></a> protected void UnblockSelf(UAct* ByAct)
This method is used internally, Only kept incase some special functionality needs to be hooked when act is being unblocked.
```cpp
void UMyAct::UnblockSelf_Implementation(UAct* ByAct)
{
    Super::UnblockSelf_Implementation(ByAct);

    // Custom functionality
}
```


---


### <a id="blockothers"></a> protected void BlockOthers()
This method is used internally, Only kept incase some special functionality needs to be hooked when act is blocking others.
```cpp
void UMyAct::BlockOthers_Implementation()
{
    Super::BlockOthers_Implementation();

    // Custom functionality
}
```


---


### <a id="unblockothers"></a> protected void UnblockOthers()
This method is used internally, Only kept incase some special functionality needs to be hooked when act is unblocking others.
```cpp
void UMyAct::UnblockOthers_Implementation()
{
    Super::UnblockOthers_Implementation();

    // Custom functionality
}
```


---


### <a id="writelog"></a> protected void WriteLog(FString Message, FString OverrideName = "")
This method is used internally to print warnings when [bIsVerbose](#bisverbose) is `true`, Only kept incase some special functionality needs to be hooked when a warning is logged.


<br/>


## 📖 UTheater Descriptions

### <a id="onenablechangedtheater"></a> public \<UTheater* Theater, bool bNewIsEnabled\> OnEnableChanged
Invoked whenever theater has been enabled/disabled.

> **Note:** Use `OnEnableChangedBP` in Blueprints


---


### <a id="onperformstart"></a> public \<UTheater* Theater, UAct* Act\> OnPerformStart
Invoked whenever any of acts assigned to the theater has started to performed.

> **Note:** Use `OnPerformStartBP` in Blueprints


---


### <a id="onperformend"></a> public \<UTheater* Theater, UAct* Act\> OnPerformEnd
Invoked whenever any of acts assigned to the theater has completed performing.

> **Note:** Use `OnPerformEndBP` in Blueprints


---


### <a id="onallperformend"></a> public \<UTheater* Theater\> OnAllPerformEnd
Invoked whenever all of the acts assigned to the theater have completed performing and none are ongoing anymore.

> **Note:** Use `OnAllPerformEndBP` in Blueprints
Useful for idle checking, etc.


---


### <a id="isenabledtheater"></a> public bool IsEnabled() const
Returns `true` if theater is currently enabled.

---


### <a id="setenabledtheater"></a> public void SetEnabled(bool bNewEnabled)
Disables/Enables theater i.e. If a theater is disabled then all acts assigned to it can no longer [`Perform()`](#perform) and any act that was ongoing will be interrupted.
```cpp
Theater->SetEnabled(false);  // Disable theater
Theater->SetEnabled(true);  // Enable theater
```


---


### <a id="abortall"></a> public void AbortAll()
Calls [Abort](#abort)() on all currently ongoing acts.


---


### <a id="areanyongoing"></a> public bool AreAnyOngoing() const
Returns `true` if any act is currently performing.


---


### <a id="getallacts"></a> public TSet\<UAct*\> GetAllActs() const
Returns a set of all the acts assigned to the theater.



[UE-DynamicMulticast]: https://docs.unrealengine.com/5.3/en-US/delegates-and-lambda-functions-in-unreal-engine/
[UE-Multicast]: https://docs.unrealengine.com/5.3/en-US/delegates-and-lambda-functions-in-unreal-engine/
[UE-TickComponent]: https://docs.unrealengine.com/5.3/en-US/API/Runtime/Engine/Components/UActorComponent/TickComponent/
[UE-BeginPlay]: https://docs.unrealengine.com/5.3/en-US/API/Runtime/Engine/GameFramework/AActor/BeginPlay/
[UE-EndPlay]: https://docs.unrealengine.com/5.3/en-US/API/Runtime/Engine/GameFramework/AActor/EndPlay/
[UE-Actor]: https://docs.unrealengine.com/5.3/en-US/API/Runtime/Engine/GameFramework/AActor/
[UE-GetDeltaTime]: https://docs.unrealengine.com/5.3/en-US/API/Runtime/Core/Misc/FApp/GetDeltaTime/
