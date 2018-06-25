// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Murdock.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONPROJECT_API AMurdock : public ABaseCharacter
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	class UObjectPoolComponent* ObjPoolComp;
	
private:
	virtual void StartAttack() override;
};
