

#pragma once

#include "GameFramework/Actor.h"
#include "TimeVolume.generated.h"

/**
 * 
 */
UCLASS()
class THEGAME_API ATimeVolume : public AActor
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Portal)
	TSubobjectPtr<class UBoxComponent> TriggerSphere;

	/** Event when this actor overlaps another actor. */
	virtual void ReceiveActorBeginOverlap(class AActor* OtherActor) override;
	/** Event when an actor no longer overlaps another actor, and they have separated. */
	virtual void ReceiveActorEndOverlap(class AActor* OtherActor) override;
};
