#ifndef VECTOR_H
#define VECTOR_H

#include <memory>

class Vector3
{
    private:
        // PIMPL idiom
        class Impl;
        std::unique_ptr<Impl> pimpl;

    public:
        Vector3();
        Vector3(float dx, float dy);
        Vector3(const Vector3&);
        Vector3& operator=(const Vector3&);
        Vector3(Vector3&&) noexcept;
        Vector3& operator=(Vector3&&) noexcept;
        ~Vector3();

        float angle() const;

        float direction_x() const;
        void set_direction_x(float dx);
        float direction_y() const;
        void set_direction_y(float dy);

        //! Does not change the velocity
        void set_direction(float dx, float dy);

        float velocity() const;
        void set_velocity(float velocity);

        void set_vector(float dx, float dy) const;

        void set_zero() const;

        //! Vector3 must be normalized
        void sum(float dx, float dy, float velocity);

        float calc_dx_dt(float dt) const;
        float calc_dy_dt(float dt) const;
};


#endif
