// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerComponent.h"

#include "Mover.h"

UTriggerComponent::UTriggerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTriggerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTriggerComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* AcceptableActor = GetAcceptableActor();
	if (AcceptableActor != nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("Unlocking"));

		UPrimitiveComponent* Component = Cast<UPrimitiveComponent>(AcceptableActor->GetRootComponent());
		if (Component != nullptr)
		{
			Component->SetSimulatePhysics(false);
		}
		Component->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
		Mover->SetShouldMove(true);
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Relocking"));
		Mover->SetShouldMove(false);
	}
}

void UTriggerComponent::SetMover(UMover* NewMover)
{
	Mover = NewMover;

}

AActor* UTriggerComponent::GetAcceptableActor() const
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);

	for (const auto& OverlappingActor : OverlappingActors)
	{
		bool isAcceptable = OverlappingActor->ActorHasTag(AcceptableActorTag);
		bool isGrabbed = OverlappingActor->ActorHasTag("Grabbed");
		if (isAcceptable && !isGrabbed)
			return OverlappingActor;
	}

	return nullptr;
}