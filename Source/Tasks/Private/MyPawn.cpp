#include "MyPawn.h"
#include "Engine/Engine.h"

AMyPawn::AMyPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	RootComponent = CapsuleComponent;

	CapsuleComponent->SetCapsuleHalfHeight(88.0f);
	CapsuleComponent->SetCapsuleRadius(34.0f);
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CapsuleComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	// Physics Simulation 비활성화하기
	CapsuleComponent->SetSimulatePhysics(false);

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(CapsuleComponent);

	SkeletalMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));
	SkeletalMeshComponent->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	
	SkeletalMeshComponent->SetSimulatePhysics(false);
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(CapsuleComponent);

	SpringArmComponent->TargetArmLength = 400.0f;
	SpringArmComponent->bUsePawnControlRotation = false;
	SpringArmComponent->bInheritPitch = false;
	SpringArmComponent->bInheritYaw = false;
	SpringArmComponent->bInheritRoll = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	CurrentYaw = 0.0f;
	CurrentPitch = 0.0f;

}

void AMyPawn::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
	
}

void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//// 임시 테스트: T키를 누르면 앞으로 이동
	//if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::T))
	//{
	//	TestMovement();
	//}

}

void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyPawn::Move);
		}

		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyPawn::Look);
		}
	}
}

void AMyPawn::Move(const FInputActionValue& Value)
{
	//Vector2D 입력값 가져오기 (X-좌우, Y-앞뒤)
	FVector2D MovementVector = Value.Get < FVector2D>();
	// 디버깅 로그 추가
	UE_LOG(LogTemp, Warning, TEXT("Move Input: X=%f, Y=%f"), MovementVector.X, MovementVector.Y);

	if (Controller != nullptr)
	{
		const FVector ForwardDirection = GetActorForwardVector();
		const FVector RightDirection = GetActorRightVector();

		FVector MovementDirection = (ForwardDirection * MovementVector.Y) + (RightDirection * MovementVector.X);

		MovementDirection.Z = 0.0f;
		MovementDirection.Normalize();

		AddActorLocalOffset(MovementDirection * MovementSpeed * GetWorld()->GetDeltaSeconds(), true);


	}
}

void AMyPawn::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		CurrentYaw += LookAxisVector.X * LookSensitivity;
		CurrentPitch += LookAxisVector.Y * LookSensitivity;

		CurrentPitch = FMath::Clamp(CurrentPitch, -90.0f, 90.0f);

		FRotator NewRotation = FRotator(0.0f, CurrentYaw, 0.0f);
		SetActorRotation(NewRotation);
		
		if (SpringArmComponent)
		{
			FRotator SpringArmRotation = FRotator(CurrentPitch, CurrentYaw, 0.0f);
			SpringArmComponent->SetWorldRotation(SpringArmRotation);

		}
	}
}

//void AMyPawn::TestMovement()
//{
//	// 간단한 앞으로 이동 테스트
//	FVector ForwardDirection = GetActorForwardVector();
//	AddActorWorldOffset(ForwardDirection * 100.0f);
//	UE_LOG(LogTemp, Warning, TEXT("Test Movement Called!"));
//}
