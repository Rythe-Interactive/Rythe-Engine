#pragma once
#include <core/math/vector/vector.hpp>
#include <core/math/geometric/geometric.hpp>
#include <core/math/exponential/exponential.hpp>

/**
 * @file geometry.hpp
 */

namespace legion::core::math
{
    /**
     * @brief Calcualtes shortest distance from point to line segment
     */
    inline float pointToLineSegment2D(const float2& point, const float2& lineOrigin, const float2& lineEnd)
    {
        if (point == lineOrigin || point == lineEnd) return 0.0f;
        float2 lineDirection = lineEnd - lineOrigin;
        float2 lineNormal = float2(-lineDirection.y, lineDirection.x);

        float2 toLineOrigin = point - lineOrigin;

        if (dot(toLineOrigin, lineDirection) <= 0.0)
        {
            // Point is before the start of the line
            // Return to line start
            return length(toLineOrigin);
        }
        float2 toLineEnd = point - lineEnd;

        if (dot(toLineEnd, lineDirection) >= 0.0)
        {
            // Point is after the end of the line
            // Return to line start
            return length(toLineEnd);
        }

        // Calculate and return point to line projection length
        return dot(point - lineOrigin, normalize(lineNormal));
    }

    /**
     * @brief Calculate shortest distance from point to infinite line
     */
    inline float pointToLine2D(const float2& point, const float2& lineOrigin, const float2& lineEnd)
    {
        if (point == lineOrigin || point == lineEnd) return 0.0f;
        float2 lineDirection = lineEnd - lineOrigin;
        float2 lineNormal = float2(-lineDirection.y, lineDirection.x);

        return dot(point - lineOrigin, normalize(lineNormal));
    }

    /**@brief Calculates the shortest distance between a point and a line
     * @param point - The point
     * @param lineOrigin - The origin of the line
     * @param lineEnd - The end of the line
     */
    inline float pointToLineSegment(const float3& point, const float3& lineOrigin, const float3& lineEnd)
    {
        // Check if the point is equal to the start or end of the line
        if (point == lineOrigin || point == lineEnd) return 0.0f;
        float3 dir = lineEnd - lineOrigin;
        float3 toLineOrigin = point - lineOrigin;

        if (dot(toLineOrigin, dir) <= 0.0)
        {
            // Projected point is before the start of the line
            // Use distance to start of the line
            return length(toLineOrigin);
        }
        float3 toLineEnd = point - lineEnd;
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
        line_segment(float3 origin, float3 end) :
            origin(origin), end(end)
        {
        }

        // Line origin, line start
        float3 origin;
        // Line end
        float3 end;

        inline float3 direction() const
        {
            return end - origin;
        }

        /**@brief Calculates the closest distance between point p and this line
         */
        float distanceToPoint(const float3& p) const
        {
            if (p == origin || p == end) return 0.0f;
            float3 dir = direction();
            float3 toLineOrigin = p - origin;

            if (dot(toLineOrigin, dir) <= 0.0)
            {
                // Projected point is before the start of the line
                // Use distance to start of the line
                return length(toLineOrigin);
            }
            float3 toLineEnd = p - end;
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
    inline float triangleSurface(const float3& p0, const float3& p1, const float3& p2)
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
    inline float pointToTriangle(const float3& p, const float3& triPoint0, const float3& triPoint1, const float3& triPoint2, const float3& triNormal,bool debug = false)
    {
        if (p == triPoint0 ||
            p == triPoint1 ||
            p == triPoint2) return 0.f;

        // Get the angle between the triangle normal and p to triPoint0
        float cosAngle = dot(triNormal, p - triPoint0) / (distance(p, triPoint0) * length(triNormal));
        // Get the length of the projection of point p onto the triangle
        float projectionLength = length(p - triPoint0) * cosAngle;
        // From the projectionLength we can determine if the point is above or under the triangle plane
        float positive = 1;
        if (projectionLength < 0) positive = -1;
        // A vector when added to p gives the projected point
        float3 towardProjection = (triNormal / length(triNormal)) * -projectionLength;
        // Q is the projection of p onto the plane
        float3 q = p + towardProjection;

        // We need to determine if projected p is inside the triangle.
        // By putting point q (projected point p) on the triangle, three new triangles are created
        // If point q is in fact inside the triangle, the areas of the three triangles will add up to same area as the original triangle

        double q01Area = triangleSurface(q, triPoint0, triPoint1);
        double q02Area = triangleSurface(q, triPoint0, triPoint2);
        double q12Area = triangleSurface(q, triPoint1, triPoint2);

        // If the area of q to each set of two points is equal to the triangle surface area, q is on the triangle
        if (abs(q01Area + q02Area + q12Area) - triangleSurface(triPoint0, triPoint1, triPoint2) < epsilon_v<float>)
        {
            return projectionLength;
        }
        //Point q is not inside the triangle, check distance toward each edge of the triangle
        // therefore the smallest distance is distance toward a side or end point
        float distance01 = pointToLineSegment(p, triPoint1, triPoint0);
        float distance02 = pointToLineSegment(p, triPoint2, triPoint0);
        float distance12 = pointToLineSegment(p, triPoint2, triPoint1);

        // Assume the shortest distance is sqDistance01
        // Then check if this is true
        float shortestDistance = distance01;
        if (distance02 < distance12)
        {
            if (distance02 < distance01) shortestDistance = distance02;
        }
        else if (distance12 < distance01) shortestDistance = distance12;

        // Return the shortest distance toward one of the sides, either positive or negative, which was determined before
        return shortestDistance*positive;
    }

    /**@brief Calculates the distance between a point and a triangle plane
     * @brief Calculates the normal of the triangle plane and calls pointToTriangle with normal
     * @param p - The point
     * @param triPoint0 - The first triangle point
     * @param triPoint1 - The second triangle point
     * @param triPoint2 - The last triangle point
     */
    inline float pointToTriangle(const float3& p, const float3& triPoint0, const float3& triPoint1, const float3& triPoint2)
    {
        float3 normal = normalize(cross(triPoint1 - triPoint0, triPoint2 - triPoint0));
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
    inline bool pointProjectionOntoTriangle(const float3& p, const float3& triPoint0, const float3& triPoint1, const float3& triPoint2, const float3& triNormal)
    {
        if (p == triPoint0 ||
            p == triPoint1 ||
            p == triPoint2) return true;

        float cosAngle = dot(triNormal, p - triPoint0) / (distance(p, triPoint0) * length(triNormal));
        float projectionLength = length(p - triPoint0) * cosAngle;
        float3 towardProjection = (triNormal / length(triNormal)) * -projectionLength;
        // Q is the projection of p onto the plane
        float3 q = p + towardProjection;

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
        triangle(float3 p0, float3 p1, float3 p2)
        {
            points[0] = p0;
            points[1] = p1;
            points[2] = p2;
            normal = normalize(cross(p1 - p0, p2 - p0));
        }

        triangle(float3 p0, float3 p1, float3 p2, float3 normal)
        {
            points[0] = p0;
            points[1] = p1;
            points[2] = p2;
            normal = normalize(normal);
        }

        // The three points of the triangle
        float3 points[3];
        // The normalized normal of the triangle plane
        float3 normal;

        /**@brief Calculates the closest distance between point p and this triangle
         */
        float distanceToPoint(const float3& p) const
        {
            if (p == points[0] ||
                p == points[1] ||
                p == points[2]) return 0.f;

            float cosAngle = dot(normal, p - points[0]) / (distance(p, points[0]) * length(normal));
            float projectionLength = length(p - points[0]) * cosAngle;
            float positive = 1;
            if (projectionLength < 0) positive = -1;
            float3 towardProjection = (normal / length(normal)) * -projectionLength;
            // Q is the projection of p onto the plane
            float3 q = p + towardProjection;

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
            float distance01 = pointToLineSegment(p, points[1], points[0]);
            float distance02 = pointToLineSegment(p, points[2], points[0]);
            float distance12 = pointToLineSegment(p, points[2], points[1]);

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
    inline float pointToPlane(const float3& point, const float3& planePosition, const float3& planeNormal)
    {
        return dot(normalize(planeNormal), point - planePosition);
    }

    inline float angleToPlane(const float3& point, const float3& planePosition, const float3& planeNormal)
    {
        return normalize_dot(planeNormal, point - planePosition);
    }

    /**@class plane
     * @brief 
     */
    struct plane
    {
        plane(float3 position, float3 normal) :
            position(position)
        {
            normal = normalize(normal);
        }

        /**@brief Constructs a plane from three points on the plane
         * @brief Uses p0 for the plane position 
         * @brief Calculates a normal using math::cross
         */
        plane(float3 p0, float3 p1, float3 p2)
        {
            position = p0;
            normal = normalize(cross(p1 - p0, p2 - p0));
        }

        // Position on the plane, or a point on the plane
        float3 position;
        // Normalized normal of the plane
        float3 normal;

        /**@brief Calculates the closest distance between point p and this plane
         */
        float distanceToPoint(const float3& p) const
        {
            return dot(normal, p-position);
        }
    };

    /**@brief Calculates a matrix for a plane
     *     p1---------- 
     *     /          /
     *    /          /
     *   /          /
     *  /          /
     * p0---------p2 
     * @param normal The normal of the plane
     * @param centroid The center of the the plane
     */
    /*inline float4x4 planeMatrix(const float3& p0, const float3& p1, const float3& p2, const float3& centroid)
    {
        float3 xAxis = p2 - p0;
        float3 zAxis = p1 - p0;

        float4x4 scale(
            length(xAxis), 0, 0, 0,
            0, length(zAxis), 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
            );

        float3 rotX = xAxis / length(xAxis);
        float3 rotZ = zAxis / length(zAxis);
        float3 rotY = cross(rotX, rotZ);

        float4x4 rot(
            rotX.x, rotX.y, rotX.z, 0,
            rotY.x, rotY.y, rotY.z, 0,
            rotZ.x, rotZ.y, rotZ.z, 0,
            0, 0, 0, 1
        );

        float4x4 translation(
            1, 0, 0, centroid.x,
            0, 1, 0, centroid.y,
            0, 0, 1, centroid.z,
            0, 0, 0, 1
        );

        return translation * rot * scale;
    }*/
}
