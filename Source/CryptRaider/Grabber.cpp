// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"

#include "PhysicsEngine/PhysicsHandleComponent.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle)
	{
		UE_LOG(LogTemp, Display, TEXT("Got Physics Handle: %s"), *PhysicsHandle->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Find Physics Handle"));
	}
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PhysicsHandle && PhysicsHandle->GetGrabbedComponent())
	{
		FVector TargetLocation = GetComponentLocation() + GetForwardVector() * HoldDistance;
		PhysicsHandle->SetTargetLocationAndRotation(TargetLocation, GetComponentRotation());
	}
}

void UGrabber::Grab()
{
	if (!PhysicsHandle)
		return;

	FHitResult HitResult;
	bool       HasHit = GetGrabbableInReach(HitResult);

	if (HasHit)
	{
		UPrimitiveComponent* HitComponent = HitResult.GetComponent();
		HitComponent->WakeAllRigidBodies();
		HitComponent->SetSimulatePhysics(true);
		AActor* HitActor = HitResult.GetActor();
		HitActor->Tags.Add("Grabbed");

		//HitComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		HitActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		//DrawDebugSphere(GetWorld(), HitResult.Location, GrabRadius, 30, FColor::Green, false, 5);
		//DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10, 10, FColor::Red, false, 5);
		//UE_LOG(LogTemp, Display, TEXT("Hitted Actor Name : %s"), *(HitResult.GetActor()->GetActorNameOrLabel()));

		PhysicsHandle->GrabComponentAtLocationWithRotation(HitComponent, NAME_None, HitResult.ImpactPoint, GetComponentRotation());
	}
}

void UGrabber::Release()
{
	//UE_LOG(LogTemp, Display, TEXT("Release Grabber"));

	if (!PhysicsHandle)
		return;

	if (UPrimitiveComponent* GrabbedComponent = PhysicsHandle->GetGrabbedComponent(); GrabbedComponent != nullptr)
	{
		GrabbedComponent->GetOwner()->Tags.Remove("Grabbed");
		PhysicsHandle->ReleaseComponent();
	}
}

bool UGrabber::GetGrabbableInReach(FHitResult& OutHitResult) const
{
	FVector Start = GetComponentLocation();
	FVector End = Start + GetForwardVector() * MaxGrabDistance;
	FColor  Color = FColor(255, 0, 0);

	// DrawDebugLine(GetWorld(), Start, End, Color);
	// DrawDebugSphere(GetWorld(), End, 10, 10, FColor::Blue, false, 5);

	// DrawDebugCapsule(GetWorld(), (Start + End) / 2, FVector::Distance(End, Start) / 2 + GrabRadius, GrabRadius, FRotationMatrix::MakeFromZ(End - Start).ToQuat(), FColor::Blue, false, 5);

	return GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, ECC_GameTraceChannel2, FCollisionShape::MakeSphere(GrabRadius));
}