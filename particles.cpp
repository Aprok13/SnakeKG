//#include "particles.h"
//
//// В данном случае particles.h содержит только определение структуры Particle,
//// поэтому particles.cpp может быть пустым или содержать вспомогательные функции,
//// если они понадобятся в будущем.
//
//// Если нужно добавить функции для работы с частицами, они будут выглядеть так:
//
//void initializeParticle(Particle& p, Point3D position) {
//    p.position = position;
//    p.velocity = Point3D((rand() % 100 - 50) / 100.0f,
//        (rand() % 100 - 50) / 100.0f,
//        (rand() % 100 - 50) / 100.0f);
//    p.lifetime = 1.0f + (rand() % 100) / 100.0f;
//    p.size = 0.05f + (rand() % 100) / 1000.0f;
//}