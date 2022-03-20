// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MainUI.h"
#include "MinimapActor.generated.h"
class ATestMinimapCharacter;
class USceneCaptureComponent2D;
class UTextureRenderTarget2D;
UCLASS()
class TESTMINIMAP_API AMinimapActor : public AActor
{
	GENERATED_BODY()
	
	USceneCaptureComponent2D* MinimapCaptureComponent;
public:	
	// Sets default values for this actor's properties
	AMinimapActor();

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Vision)
	int CheckVisionStepsNum;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void ProcessVision();
	void EstimateVisionArea(ATestMinimapCharacter* Character, TArray<FLineSegment> &AreaLines);
	bool Multitrace(TArray<FHitResult>& Hits, FVector Start, FVector End,  FCollisionQueryParams CollisionQueryParams);
public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	FLineSegments Vision;
	UMainUI* UI;
	void CheckLinesIntersection(FLineSegment &L1, FLineSegment &L2);

};
