#include "TheGame.h"
#include "Kismet/KismetMathLibrary.h"
#include "Portal.h"

APortal::APortal(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	this->PrimaryActorTick.bStartWithTickEnabled = true;
	this->PrimaryActorTick.bAllowTickOnDedicatedServer = true;
	this->PrimaryActorTick.bCanEverTick = true;

	this->SceneRoot = PCIP.CreateDefaultSubobject<USceneComponent>(this, _TEXT("Root"));
	this->RootComponent = this->SceneRoot;

	this->TriggerBox = PCIP.CreateDefaultSubobject<UBoxComponent>(this, _TEXT("TriggerBox"));
	this->TriggerBox->AttachTo(RootComponent);

	this->SceneCapture = PCIP.CreateDefaultSubobject<USceneCaptureComponent2D>(this, _TEXT("SceneCapture"));
	this->SceneCapture->AttachTo(RootComponent);

	this->StaticMesh = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, _TEXT("StaticMesh"));
	this->StaticMesh->AttachTo(RootComponent);
}

void
APortal::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	this->dynamicMaterial = this->StaticMesh->CreateAndSetMaterialInstanceDynamic(0);
}

void
APortal::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> outActors;
	UGameplayStatics::GetAllActorsOfClass(this, APortal::GetActorClass(), outActors);

	// find the other portal
	int32 i;
	for (i = 0; i < outActors.Num(); i++)
	{
		APortal* curPortal = Cast<APortal>(outActors[i]);
		if (curPortal && curPortal != this)
		{
			this->AssignOtherPortal(curPortal);
			break;
		}
	}
}

void
APortal::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);

	// handle only if portal is open
	if (!this->IsPortalOpen())
		return;

	TArray<AActor*> overlappingActors;
	this->GetOverlappingActors(overlappingActors);

	UE_CLOG(overlappingActors.Num() > 0, LogTemp, Warning, _TEXT("Tick() this=%s numActors=%i"), *(this->GetName()), overlappingActors.Num())

	const FVector& myForwardVector = this->GetActorForwardVector();
	const FTransform& myTransform = this->GetTransform();
	const FTransform& otherTransform = this->otherPortal->GetTransform();
	FPlane plane(this->GetActorLocation(), this->GetActorForwardVector());
	float T;
	FVector intersection;
	int32 i;
	for (i = 0; i < overlappingActors.Num(); i++)
	{
		AActor* curActor = overlappingActors[i];
		const FVector& otherVelocity = curActor->GetVelocity();
		
		// only handle overlaps moving in my forward direction
		float dotResult = FVector::DotProduct(otherVelocity, myForwardVector);
		if (dotResult < 0.01f)
		{
			continue;
		}

		const FVector& curLoc = curActor->GetActorLocation();
		FVector nextLoc = curActor->GetActorLocation() + curActor->GetVelocity() * deltaSeconds;

		UE_LOG( LogTemp, Warning, _TEXT("---CurLoc=%s NextLoc=%s"), *(curLoc.ToString()), *(nextLoc.ToString()))

		// check movement vector intersection with portal plane
		if (LinePlaneIntersection(curLoc, nextLoc, plane, T, intersection))
		{
			const FTransform& overlapTransform = curActor->GetTransform();
			
			FTransform myInverted = myTransform.InverseSafe();

			FTransform overlapNewTrasnform = overlapTransform;
			overlapNewTrasnform.SetLocation(nextLoc);
			overlapNewTrasnform = myInverted;
			overlapNewTrasnform = overlapTransform * myInverted;
			overlapNewTrasnform = overlapNewTrasnform * otherTransform;

			// HACK:
			/*TSubclassOf<UActorComponent> subclass(UMovementComponent::StaticClass());
			UMovementComponent* moveComponent = Cast<UMovementComponent>(curActor->GetComponentByClass(subclass));
			if (moveComponent)
			{
				moveComponent->StopMovementImmediately();
				moveComponent->Velocity = 
			}*/

			UE_LOG(LogTemp, Warning, _TEXT("---NewLoc=%s"), *(overlapNewTrasnform.GetLocation().ToString()))

			FQuat quat = overlapNewTrasnform.GetRotation();
			FRotator rotator = quat.Rotator();
			rotator.Yaw += 180;
			curActor->TeleportTo(overlapNewTrasnform.GetLocation(), rotator);
		}
	}
}

bool 
APortal::LinePlaneIntersection(const FVector& LineStart, const FVector& LineEnd, const FPlane& APlane, float& T, FVector& Intersection)
{
	FVector RayDir = LineEnd - LineStart;

	// Check ray is not parallel to plane
	if ((RayDir | APlane) == 0.0f)
	{
		T = -1.0f;
		Intersection = FVector::ZeroVector;
		return false;
	}

	T = ((APlane.W - (LineStart | APlane)) / (RayDir | APlane));

	// Check intersection is not outside line segment
	if (T < 0.0f || T > 1.0f)
	{
		Intersection = FVector::ZeroVector;
		return false;
	}

	// Calculate intersection point
	Intersection = LineStart + RayDir * T;

	return true;
}


void
APortal::AssignOtherPortal(APortal* other)
{
	if (this->otherPortal.IsValid())
		return;

	this->otherPortal = other;
	this->otherPortal->AssignOtherPortal(this);

	this->otherPortal->SceneCapture->TextureTarget = NewObject<UTextureRenderTarget2D>(this->otherPortal.Get());
	this->otherPortal->SceneCapture->TextureTarget->InitAutoFormat(1024, 1024);
	this->otherPortal->SceneCapture->TextureTarget->ClearColor = FLinearColor(0.f, 0.f, 0.f, 0.f);
	this->dynamicMaterial->SetTextureParameterValue("DiffuseColor", this->otherPortal->SceneCapture->TextureTarget);

}