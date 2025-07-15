// MyPawn.cpp

#include "MyPawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Engine/LocalPlayer.h"

AMyPawn::AMyPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create and setup Capsule Component as root
    CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
    RootComponent = CapsuleComponent;
    CapsuleComponent->InitCapsuleSize(42.0f, 96.0f);
    CapsuleComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 96.0f)); // Half height to place on ground

    // IMPORTANT: Disable physics simulation for root component
    CapsuleComponent->SetSimulatePhysics(false);
    CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CapsuleComponent->SetCollisionResponseToAllChannels(ECR_Block);

    // Create and setup Skeletal Mesh Component
    SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
    SkeletalMeshComponent->SetupAttachment(RootComponent);
    SkeletalMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
    SkeletalMeshComponent->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

    // IMPORTANT: Disable physics simulation for mesh component
    SkeletalMeshComponent->SetSimulatePhysics(false);
    SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Create and setup Spring Arm Component
    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
    SpringArmComponent->SetupAttachment(RootComponent);
    SpringArmComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 40.0f));
    SpringArmComponent->TargetArmLength = 400.0f;
    SpringArmComponent->bUsePawnControlRotation = false; // We'll control rotation manually
    SpringArmComponent->bInheritPitch = true;
    SpringArmComponent->bInheritYaw = true;
    SpringArmComponent->bInheritRoll = true;

    // Create and setup Camera Component
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
    CameraComponent->bUsePawnControlRotation = false;

    // Set this pawn to be controlled by the lowest-numbered player
    AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void AMyPawn::BeginPlay()
{
    Super::BeginPlay();

    // Add Input Mapping Context
    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    // Initialize current rotation
    CurrentYaw = GetActorRotation().Yaw;
    CurrentPitch = SpringArmComponent->GetRelativeRotation().Pitch;

    // Initialize velocity tracking
    LastLocation = GetActorLocation();
}

void AMyPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Calculate velocity
    FVector CurrentLocation = GetActorLocation();
    CurrentVelocity = (CurrentLocation - LastLocation) / DeltaTime;
    LastLocation = CurrentLocation;
}

void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Make sure that we are using a UEnhancedInputComponent
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // Bind the actions
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyPawn::Move);
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyPawn::Look);
    }
}

void AMyPawn::Move(const FInputActionValue& Value)
{
    // Get the input as a Vector2D
    FVector2D MovementVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        // Get forward and right vectors based on the pawn's rotation
        const FRotator Rotation = GetActorRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // Get forward and right directions
        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        // Calculate movement
        FVector MovementDelta = (ForwardDirection * MovementVector.Y + RightDirection * MovementVector.X) * MoveSpeed * GetWorld()->GetDeltaSeconds();

        // Apply movement using AddActorWorldOffset
        AddActorWorldOffset(MovementDelta, true);
    }
}

void AMyPawn::Look(const FInputActionValue& Value)
{
    // Get the input as a Vector2D
    FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        // Calculate new rotation values
        CurrentYaw += LookAxisVector.X * TurnRate * GetWorld()->GetDeltaSeconds();
        CurrentPitch += LookAxisVector.Y * LookUpRate * GetWorld()->GetDeltaSeconds();

        // Clamp pitch to prevent over-rotation
        CurrentPitch = FMath::Clamp(CurrentPitch, -80.0f, 80.0f);

        // Apply Yaw rotation to the entire pawn
        SetActorRotation(FRotator(0.0f, CurrentYaw, 0.0f));

        // Apply Pitch rotation to the spring arm
        SpringArmComponent->SetRelativeRotation(FRotator(CurrentPitch, 0.0f, 0.0f));
    }
}