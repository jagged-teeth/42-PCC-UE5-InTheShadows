// Fill out your copyright notice in the Description page of Project Settings.


#include "TestActor.h"

// Sets default values
ATestActor::ATestActor()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATestActor::BeginPlay()
{
	Super::BeginPlay();

	InteractableData = InstanceInteractableData;
}

// Called every frame
void ATestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATestActor::BeginFocus()
{
	if (Mesh)
		Mesh->SetRenderCustomDepth(true);
}

void ATestActor::EndFocus()
{
	if (Mesh)
		Mesh->SetRenderCustomDepth(false);
	if (IsInteracting)
		EndInteract();
}

void ATestActor::BeginInteract()
{
	IsInteracting = true;
	UE_LOG(LogTemp, Warning, TEXT("Calling BeginInteract override on interface test actor"));
}

void ATestActor::EndInteract()
{
	IsInteracting = false;
	UE_LOG(LogTemp, Warning, TEXT("Calling EndInteract override on interface test actor"));
}

void ATestActor::Interact(APlayerCharacter* PC)
{
	UE_LOG(LogTemp, Warning, TEXT("Calling Interact override on interface test actor"));
}
