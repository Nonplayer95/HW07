// MyAnimInstance.h

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyAnimInstance.generated.h"

UCLASS()
class TASKS_API UMyAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    UMyAnimInstance();

    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
    // Reference to the owning pawn
    UPROPERTY(BlueprintReadOnly, Category = "Character")
    class AMyPawn* OwnerPawn;

    // Movement properties
    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    float Speed;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    float Direction;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    bool bIsMoving;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    FVector Velocity;

    // For blending between idle and movement
    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    float MovementBlendWeight;

private:
    // Helper functions
    void UpdateMovementProperties();
};