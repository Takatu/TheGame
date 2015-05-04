

#pragma once

#include "GameFramework/Actor.h"
#include "Portal.generated.h"

/**
 * 
 */
UCLASS()
class THEGAME_API APortal : public AActor
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Portal)
	TSubobjectPtr<class USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Portal)
	TSubobjectPtr<class UBoxComponent> TriggerBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Portal)
	TSubobjectPtr<class UStaticMeshComponent> StaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Portal)
	TSubobjectPtr<class USceneCaptureComponent2D> SceneCapture;

	/** Called from main actor tick function to implement custom code at the appropriate point in the tick */
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

private:
	// return true if portal is open
	bool IsPortalOpen() const;
	//
	static bool LinePlaneIntersection(const FVector& LineStart, const FVector& LineEnd, const FPlane& APlane, float& T, FVector& Intersection);
	//
	void AssignOtherPortal(APortal* otherPortal);

	TWeakObjectPtr<APortal> otherPortal;
	TWeakObjectPtr<UMaterialInstanceDynamic> dynamicMaterial;
};


inline
bool APortal::IsPortalOpen() const
{
	return this->otherPortal.IsValid();
}