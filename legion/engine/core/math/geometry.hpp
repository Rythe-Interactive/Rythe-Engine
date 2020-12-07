#pragma once
#include <core/math/glm/glm_include.hpp>
#include <core/math/glm/gtx/string_cast.hpp>
#include <core/math/close_enough.hpp>

/**
 * @file geometry.hpp
 */

namespace legion::core::math
{
    /**@brief Calcualtes the shortest distance between a point and a line
     * @param point - The point
     * @param lineOrigin - The origin of the line
     * @param lineEnd - The end of the line
     */
    inline float pointToLine(const vec3& point, const vec3& lineOrigin, const vec3& lineEnd)
    {
        if (point == lineOrigin || point == lineEnd) return 0.0f;
        vec3 dir = lineEnd - lineOrigin;
        vec3 toLineOrigin = point - lineOrigin;

        if (dot(toLineOrigin, dir) <= 0.0)
        {
            // Projected point is before the start of the line
            // Use distance to start of the line
            return length(toLineOrigin);
        }
        vec3 toLineEnd = point - lineEnd;
        if (dot(toLineEnd, dir) >= 0.0f)
        {
            // Projected point is beyond end of line
            // Use distance toward end of line
            return length(toLineEnd);
        }
        return length(cross(dir, toLineOrigin)) / length(dir);
    }

    /**@class line_segment
     * @brief Data structure for a line segment
     */
    struct line_segment
    {
        line_segment(vec3 origin, vec3 end) :
            origin(origin), end(end)
        {
        }

        // Line origin, line start
        vec3 origin;
        // Line end
        vec3 end;

        inline vec3 direction() const
        {
            return end - origin;
        }

        /**@brief Calculates the closest distance between point p and this line
         */
        float distanceToPoint(const vec3& p) const
        {
            if (p == origin || p == end) return 0.0f;
            vec3 dir = direction();
            vec3 toLineOrigin = p - origin;

            if (dot(toLineOrigin, dir) <= 0.0)
            {
                // Projected point is before the start of the line
                // Use distance to start of the line
                return length(toLineOrigin);
            }
            vec3 toLineEnd = p - end;
            if (dot(toLineEnd, dir) >= 0.0f)
            {
                // Projected point is beyond end of line
                // Use distance toward end of line
                return length(toLineEnd);
            }
            return length(cross(dir, toLineOrigin)) / length(dir);
        }
    };

    /**@brief Calculates the size of a triangles surface area
     */
    inline float triangleSurface(const vec3& p0, const vec3& p1, const vec3& p2)
    {
        if (p0 == p1 || p0 == p2 || p1 == p2) return 0.0;
        // side lengths
        float a = abs(length(p0 - p1));
        float b = abs(length(p1 - p2));
        float c = abs(length(p2 - p0));
        float s = (a + b + c) * 0.5f;
        return sqrt(s * (s - a) * (s - b) * (s - c));
    }

    /**@brief Calculates the distance between a point and a triangle plane
     * @param p - The point
     * @param triPoint0 - The first triangle point
     * @param triPoint1 - The second triangle point
     * @param triPoint2 - The last triangle point
     * @param triNormal - The triangle plane normal
     */
    inline float pointToTriangle(const vec3& p, const vec3& triPoint0, const vec3& triPoint1, const vec3& triPoint2, const vec3& triNormal)
    {
        if (p == triPoint0 ||
            p == triPoint1 ||
            p == triPoint2) return 0.f;

        float cosAngle = dot(triNormal, p - triPoint0) / (distance(p, triPoint0) * length(triNormal));
        float projectionLength = length(p - triPoint0) * cosAngle;
        float positive = 1;
        if (projectionLength < 0) positive = -1;
        vec3 towardProjection = -projectionLength * (triNormal / length(triNormal));
        // Q is the projection of p onto the plane
        vec3 q = p + towardProjection;

         // Old way of finding if the point is in the triangle
        double q01Area = triangleSurface(q, triPoint0, triPoint1);
        double q02Area = triangleSurface(q, triPoint0, triPoint2);
        double q12Area = triangleSurface(q, triPoint1, triPoint2);

        // If the area of q to each set of two points is equal to the triangle surface area, q is on the triangle
        if (math::close_enough((q01Area + q02Area + q12Area), triangleSurface(triPoint0, triPoint1, triPoint2)))
        {
            return projectionLength;
        }

        //Point q is not on the triangle, check distance toward each edge of the triangle
        float distance01 = pointToLine(p, triPoint1, triPoint0);
        float distance02 = pointToLine(p, triPoint2, triPoint0);
        float distance12 = pointToLine(p, triPoint2, triPoint1);

        // Assume the shortest distance is sqDistance01
        // Then check if this is true
        float shortestDistance = distance01;
        if (distance02 < distance12)
        {
            if (distance02 < distance01) shortestDistance = distance02;
        }
        else if (distance12 < distance01) shortestDistance = distance12;

        return shortestDistance*positive;
    }

    /**@brief Calculates the distance between a point and a triangle plane
     * @brief Calculates the normal of the triangle plane and calls pointToTriangle with normal
     * @param p - The point
     * @param triPoint0 - The first triangle point
     * @param triPoint1 - The second triangle point
     * @param triPoint2 - The last triangle point
     */
    inline float pointToTriangle(const vec3& p, const vec3& triPoint0, const vec3& triPoint1, const vec3& triPoint2)
    {
        vec3 normal = normalize(cross(triPoint1 - triPoint0, triPoint2 - triPoint0));
        return pointToTriangle(p, triPoint0, triPoint1, triPoint2, normal);
    }

    /**@brief Calculates if a point can be projected onto a triangle
     * @param p - The point to be projected
     * @param triPoint0 - The first triangle point
     * @param triPoint1 - The second triangle point
     * @param triPoint2 - The last triangle point
     * @param triNormal - The normal of the triangle plane
     * @return whther the point can be projected onto the triangle
     */
    inline bool pointProjectionOntoTriangle(const vec3& p, const vec3& triPoint0, const vec3& triPoint1, const vec3& triPoint2, const vec3& triNormal)
    {
        if (p == triPoint0 ||
            p == triPoint1 ||
            p == triPoint2) return true;

        float cosAngle = dot(triNormal, p - triPoint0) / (distance(p, triPoint0) * length(triNormal));
        float projectionLength = length(p - triPoint0) * cosAngle;
        vec3 towardProjection = -projectionLength * (triNormal / length(triNormal));
        // Q is the projection of p onto the plane
        vec3 q = p + towardProjection;

        // Old way of finding if the point is in the triangle
        double q01Area = triangleSurface(q, triPoint0, triPoint1);
        double q02Area = triangleSurface(q, triPoint0, triPoint2);
        double q12Area = triangleSurface(q, triPoint1, triPoint2);

        // If the area of q to each set of two points is equal to the triangle surface area, q is on the triangle
        if (math::close_enough((q01Area + q02Area + q12Area), triangleSurface(triPoint0, triPoint1, triPoint2)))
        {
            return true;
        }
        return false;
    }

    /**@class triangle
     * @brief Data structure for a triangle
     */
    struct triangle
    {
        triangle(vec3 p0, vec3 p1, vec3 p2)
        {
            points[0] = p0;
            points[1] = p1;
            points[2] = p2;
            normal = normalize(cross(p1 - p0, p2 - p0));
        }

        triangle(vec3 p0, vec3 p1, vec3 p2, vec3 normal)
        {
            points[0] = p0;
            points[1] = p1;
            points[2] = p2;
            normal = normalize(normal);
        }

        // The three points of the triangle
        vec3 points[3];
        // The normalized normal of the triangle plane
        vec3 normal;

        /**@brief Calculates the closest distance between point p and this triangle
         */
        float distanceToPoint(const vec3& p) const
        {
            if (p == points[0] ||
                p == points[1] ||
                p == points[2]) return 0.f;

            float cosAngle = dot(normal, p - points[0]) / (distance(p, points[0]) * length(normal));
            float projectionLength = length(p - points[0]) * cosAngle;
            float positive = 1;
            if (projectionLength < 0) positive = -1;
            vec3 towardProjection = -projectionLength * (normal / length(normal));
            // Q is the projection of p onto the plane
            vec3 q = p + towardProjection;

            // Old way of finding if the point is in the triangle
            double q01Area = triangleSurface(q, points[0], points[1]);
            double q02Area = triangleSurface(q, points[0], points[2]);
            double q12Area = triangleSurface(q, points[1], points[2]);

            // If the area of q to each set of two points is equal to the triangle surface area, q is on the triangle
            if (math::close_enough((q01Area + q02Area + q12Area), surface()))
            {
                return projectionLength;
            }

            //Point q is not on the triangle, check distance toward each edge of the triangle
            float distance01 = pointToLine(p, points[1], points[0]);
            float distance02 = pointToLine(p, points[2], points[0]);
            float distance12 = pointToLine(p, points[2], points[1]);

            // Assume the shortest distance is sqDistance01
            // Then check if this is true
            float shortestDistance = distance01;
            if (distance02 < distance12)
            {
                if (distance02 < distance01) shortestDistance = distance02;
            }
            else if (distance12 < distance01) shortestDistance = distance12;

            return shortestDistance * positive;
        }

        /**@brief Calculates the size of the surface area
         */
        float surface() const
        {
            // side lengths
            float a = abs(length(points[0] - points[1]));
            float b = abs(length(points[1] - points[2]));
            float c = abs(length(points[2] - points[0]));
            float s = (a + b + c) * 0.5f;
            return sqrt(s * (s - a) * (s - b) * (s - c));
        }
    };

    /**@brief Calculates the closest distance between point p and a plane
     * @param point - The point
     * @param planePosition - A point on the plane
     * @param planeNormal - The plane normal
     */
    inline float pointToPlane(const vec3& point, const vec3& planePosition, const vec3& planeNormal)
    {
        return dot(normalize(planeNormal), point - planePosition);
    }

    /**@class plane
     * @brief 
     */
    struct plane
    {
        plane(vec3 position, vec3 normal) :
            position(position)
        {
            normal = normalize(normal);
        }

        /**@brief Constructs a plane from three points on the plane
         * @brief Uses p0 for the plane position 
         * @brief Calculates a normal using math::cross
         */
        plane(vec3 p0, vec3 p1, vec3 p2)
        {
            position = p0;
            normal = normalize(cross(p1 - p0, p2 - p0));
        }

        // Position on the plane, or a point on the plane
        vec3 position;
        // Normalized normal of the plane
        vec3 normal;

        /**@brief Calculates the closest distance between point p and this plane
         */
        float distanceToPoint(const vec3& p) const
        {
            return dot(normal, p-position);
        }
    };
}
