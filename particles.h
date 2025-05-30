#pragma once
#ifndef PARTICLES_H
#define PARTICLES_H

#include "common.h"

struct Particle {
    Point3D position;
    Point3D velocity;
    float lifetime;
    float size;
};

#endif