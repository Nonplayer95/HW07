// MyAnimInstance.cpp

#include "MyAnimInstance.h"
#include "MyPawn.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UMyAnimInstance::UMyAnimInstance()
{
    Speed = 0.0f;
    Direction = 0.0f;
    bIsMoving = false;
    MovementBlendWeight = 0.0f;
}

void UMyAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    // Get the owning pawn
    OwnerPawn = Cast<AMyPawn>(TryGetPawnOwner());
}

void UMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    // Make sure we have a valid pawn
    if (!OwnerPawn)
    {
        OwnerPawn = Cast<AMyPawn>(TryGetPawnOwner());
        if (!OwnerPawn)
        {
            return;
        }
    }

    UpdateMovementProperties();

    // Smooth blend weight for animation transitions
    float TargetBlendWeight = bIsMoving ? 1.0f : 0.0f;
    MovementBlendWeight = FMath::FInterpTo(MovementBlendWeight, TargetBlendWeight, DeltaSeconds, 10.0f);
}

void UMyAnimInstance::UpdateMovementProperties()
{
    if (!OwnerPawn)
    {
        return;
    }

    // Get velocity
    Velocity = OwnerPawn->GetVelocity();

    // Calculate speed (magnitude of horizontal velocity)
    FVector HorizontalVelocity = Velocity;
    HorizontalVelocity.Z = 0.0f;
    Speed = HorizontalVelocity.Size();

    // Check if moving
    bIsMoving = Speed > 5.0f; // Threshold to consider as moving

    // Calculate direction (angle between forward vector and velocity)
    if (bIsMoving)
    {
        FRotator PawnRotation = OwnerPawn->GetActorRotation();
        FRotator VelocityRotation = Velocity.Rotation();

        // Calculate the direction angle
        Direction = UKismetMathLibrary::NormalizedDeltaRotator(VelocityRotation, PawnRotation).Yaw;
    }
    else
    {
        Direction = 0.0f;
    }
}