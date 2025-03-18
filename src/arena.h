#ifndef ARENA_H
#define ARENA_H

#include <list>
#include <vector>

#include "boundary.h"
#include "platform.h"
#include "character.h"
#include "bullet.h"

#include "tinyxml2.h"

class Arena {
    private:
        // pimpl idiom
        class Impl;
        std::unique_ptr<Impl> pimpl;

    public:
        const float &height() const;
        const float &width() const;
        const float &depth() const;

        const Boundary &boundaries() const;
        const std::vector<Platform> &platforms() const;
        std::list<Character> &foes();
        const Character &player() const;
        const std::vector<Character> &players() const; //! only one player
        std::list<Bullet> &bullets();

        void addBullet(Bullet &&bullet);

        Arena(float height);
        ~Arena();

        float loadFrom(const char *doc);

        void draw(bool draw_axes = false) const;
};

#endif
