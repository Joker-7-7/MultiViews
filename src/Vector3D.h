#ifndef COMMON_VECTOR3D_HPP
#define COMMON_VECTOR3D_HPP

    /**
     * 3D vector including three coordinates: x, y, z
     * <p>
     * Used to work with geometry in 3D space
     * <p>
     *
     * Usage example:
     * @code
     * double coordinates[3] = { 1.25, 2.33, 3.94 };
     * Vector3D vector(coordinates);
     * vector.round();
     * @endcode
     *
     * @author Ggevorgyan
     * @since  0.1.0
     */
    struct Vector3D final
    {
        /**
         * X coordinate of a vector in space
         */
        double x = 0;

        /**
         * Y coordinate of a vector in space
         */
        double y = 0;

        /**
         * Z coordinate of a vector in space
         */
        double z = 0;

        /**
         * Constructs the vector with X, Y, Z coordinates equal to <code>0.0</code>
         */
        Vector3D() = default;

        /**
         * Constructs the point with given coordinates X, Y, Z.
         *
         * @param x X coordinate
         * @param y Y coordinate
         * @param z Z coordinate
         */
        Vector3D(double x, double y, double z) noexcept;

        /**
         * Constructs the point from 3-element array [x, y, z].
         *
         * @param value pointer to an array
         *
         * @attention passing pointer to invalid or forbidden memory cause undefined behaviour
         */
        explicit Vector3D(const double* value) noexcept;

        /**
         * Copy constructor.
         *
         * @param vector vector to be copied
         */
        Vector3D(const Vector3D& vector) = default;

        /**
         * Move constructor.
         *
         * @param vector vector to be moved
         */
        Vector3D(Vector3D&& vector) noexcept = default;

        /**
         * Reinterprets the point as an array
         *
         * @return pointer to the 'x' coordinate
         */
        double* ptr() noexcept;

        /**
         * Reinterprets the point as a const array
         *
         * @return const pointer to the 'x' coordinate
         */
        const double* ptr() const noexcept;

        /**
         * Copy assignment operator.
         *
         * @param  value vector to be copied
         * @return       reference to the current instance
         */
        Vector3D& operator=(const Vector3D& value) = default;

        /**
         * Move assignment operator.
         *
         * @param  value vector to be moved
         * @return       reference to the current instance
         */
        Vector3D& operator=(Vector3D&& value) = default;

        /**
         * Assignment operator.
         *
         * @param  value pointer to a 3-element array with [x, y, z] coordinates.
         * @return       reference to the current instance
         *
         * @attention    passing pointer to invalid or forbidden memory cause undefined behaviour
         */
        Vector3D& operator=(const double* value) noexcept;

        /**
         * Subtraction operator
         *
         * @param other the point from which we subtract
         * @return      subtraction result
         */
        Vector3D operator-(const Vector3D& other) const noexcept;

        /**
         * Addition operator
         *
         * @param other the point from which we add
         * @return      addition result
         */
        Vector3D operator+(const Vector3D& other) const noexcept;

        /**
         * Multiplies the current vector by provided factor.
         *
         * @param   factor Factor to multiple by
         * @returns <code>(x * factor, y * factor, z * factor)</code> vector
         */
        Vector3D operator*(double factor) const noexcept;

        /**
         * Divides the current vector by provided factor.
         *
         * @param   factor Factor to divide by
         * @returns <code>(x / factor, y / factor, z / factor)</code> vector
         * @note    factor must not be zero
         */
        Vector3D operator/(double factor) const noexcept;

        /**
         * Rounds all coordinates to the nearest integer values
         */
        void round() noexcept;

        /**
         * Get's normalized vector from the current instance.
         *
         * @return <code>(0, 0, 0)</code> if the current instance is <code>(0, 0, 0)</code>,
         *         <code>(x / len, y / len, z / len)</code> otherwise, where the <code>len</code>
         *         is the current vector length.
         */
        Vector3D normalized() const noexcept;

        /**
         * Calculates a length (Eucledean norm) of a vector.
         * @return Length of a vector <code>sqrt(x*x + y*y + z*z)</code>
         */
        double length() const noexcept;
    };

#endif
