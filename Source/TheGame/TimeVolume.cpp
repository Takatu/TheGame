

#include "TheGame.h"
#include "TimeVolume.h"

ATimeVolume::ATimeVolume(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	this->TriggerSphere = PCIP.CreateDefaultSubobject<UBoxComponent>(this, _TEXT("TriggerSphere"));
	RootComponent = this->TriggerSphere;
}

void
ATimeVolume::ReceiveActorBeginOverlap(class AActor* OtherActor)
{
	Super::ReceiveActorBeginOverlap(OtherActor);

	OtherActor->GetComponents();
	UActorComponent* component;
	component->ComponentVelo
}
