// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"

#define DEBUG_ERROR(e) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, (e));
#define DEBUG_WARNING(e) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, (e));
#define DEBUG_INFO(e) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, (e));
