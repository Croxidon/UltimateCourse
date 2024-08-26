#pragma once
#include "DrawDebugHelpers.h"

#define DRAW_SPHERE(ActorLocation) if(GetWorld()) DrawDebugSphere(GetWorld(), ActorLocation, 32.f, 24, FColor::Blue, true);
#define DRAW_SPHERE_COLOR(ActorLocation, Color) if(GetWorld()) DrawDebugSphere(GetWorld(), ActorLocation, 16.f, 24, Color, false, 5.f);
#define DRAW_SPHERE_SingleFrame(ActorLocation) if(GetWorld()) DrawDebugSphere(GetWorld(), ActorLocation, 32.f, 24, FColor::Blue, false, -1.f);
#define DRAW_LINE(StartLocation, EndLocation)  if(GetWorld()) DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, true, -1.f, 0, 1.f);
#define DRAW_POINT(ActorLocation) if(GetWorld()) DrawDebugPoint(GetWorld(),ActorLocation, 15.f,FColor::Red,true);
#define DRAW_POINT_SingleFrame(ActorLocation) if(GetWorld()) DrawDebugPoint(GetWorld(),ActorLocation, 15.f,FColor::Red,false, -1.f);

#define DRAW_VECTOR(StartLocation, EndLocation) if(GetWorld()) \
{\
DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, true, -1.f, 0, 1.f); \
DrawDebugPoint(GetWorld(),EndLocation, 15.f,FColor::Red,true); \
}

#define DRAW_VECTOR_SingleFrame(StartLocation, EndLocation) if(GetWorld()) \
{\
DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, -1.f, 0, 1.f); \
DrawDebugPoint(GetWorld(),EndLocation, 15.f,FColor::Red,false); \
}