#include "MyPawn.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

AMyPawn::AMyPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
    RootComponent = BoxComp;
    BoxComp->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));

    SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComp"));
    SkeletalMeshComp->SetupAttachment(RootComponent);

    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
    SpringArmComp->SetupAttachment(RootComponent);
    SpringArmComp->TargetArmLength = 400.0f;
    SpringArmComp->bUsePawnControlRotation = false;

    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
    CameraComp->SetupAttachment(SpringArmComp);

    BoxComp->SetSimulatePhysics(false);
    SkeletalMeshComp->SetSimulatePhysics(false);
}

void AMyPawn::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }
}

void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyPawn::Move);
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyPawn::Look);
    }
}

void AMyPawn::Move(const FInputActionValue& Value) { MovementInput = Value.Get<FVector2D>(); }
void AMyPawn::Look(const FInputActionValue& Value) { LookInput = Value.Get<FVector2D>(); }

void AMyPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!MovementInput.IsZero())
    {
        FVector DeltaLocation = FVector(MovementInput.Y, MovementInput.X, 0.0f) * MoveSpeed * DeltaTime;
        AddActorLocalOffset(DeltaLocation, true);

        MovementInput = FVector2D::ZeroVector;
    }

    if (!LookInput.IsZero())
    {
        float NewYaw = LookInput.X * LookSpeed * DeltaTime;
        float NewPitch = -LookInput.Y * LookSpeed * DeltaTime;

        FRotator NewRotation = FRotator(NewPitch, NewYaw, 0.0f);
        AddActorLocalRotation(NewRotation);

        LookInput = FVector2D::ZeroVector;
    }
}