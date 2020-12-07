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
            //std::cout << "\t\t\t\t\t\t\tUsing to line origin" << std::endl;
            return length(toLineOrigin);
        }
        vec3 toLineEnd = point - lineEnd;
        if (dot(toLineEnd, dir) >= 0.0f)
        {
            // Projected point is beyond end of line
            // Use distance toward end of line
            //std::cout << "\t\t\t\t\t\t\tUsing to line end" << std::endl;
            return length(toLineEnd);
        }
        //float distOnLine = dot(toLineOrigin, normalize(dir));
        //vec3 closestPointOnLine = lineOrigin + (normalize(dir) * distOnLine);
        //vec3 difference = closestPointOnLine - point;
        //std::cout << "\t\t\t\t\t\t\tUsing line projection: " << " ; " << length(difference) << std::endl;
        //return length(difference);
        //float dist = length(cross(dir, toLineOrigin)) / length(dir);
        //std::cout << "\t\t\t\t\t\t\tUsing line projection: " << dist << " ; " << length(difference) << ", diff: " << abs(dist-length(difference)) << std::endl;
        //std::cout << "\t\t\t\t\t\t\tUsing line projection: " << dist << std::endl;
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

        inline vec3 direction()
        {
            return end - origin;
        }

        /**@brief Calculates the closest distance between point p and this line
         */
        float distanceToPoint(const vec3& p) const
        {
            vec3 dir = normalize(end - origin);
            vec3 toLineOrigin = origin - p;
            float distOnLine = dot(toLineOrigin, dir);
            vec3 closestPointOnLine = origin + (dir * distOnLine);
            return distance(closestPointOnLine, p);
        }

        /**@brief Calculates the closest squared distance between point p and this line
         * @brief Does not require a sqrt
         */
        float squaredDistanceToPoint(const vec3& p) const
        {
            vec3 dir = normalize(end - origin);
            vec3 toLineOrigin = origin - p;
            float distOnLine = dot(toLineOrigin, dir);
            vec3 closestPointOnLine = origin + (dir * distOnLine);
            return (closestPointOnLine.x * closestPointOnLine.x) + (closestPointOnLine.y * closestPointOnLine.y) + (closestPointOnLine.z * closestPointOnLine.z);
        }
    };

    /**@brief Calculates the size of a triangles surface area
     */
    inline float triangleSurface(const vec3& p0, const vec3& p1, const vec3& p2)
    {
        if (p0 == p1 || p0 == p2 || p1 == p2) return 0.0;
        /*double dot = normalizeDot(p1-p0,p2-p0);
        return 0.5f * dot;*/
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
        //std::cout << "\t\t\t\t\t\tTriangle: " << to_string(triPoint0) << " ; " << to_string(triPoint1) << " ; " << to_string(triPoint2) << "\n";
        // Distance to plane
        //float distanceToPlane = dot(triNormal, p - triPoint0); // If this is negative, the point is behiond the plane
        //float positive = 1; // positive by default
        //if(distanceToPlane != 0) positive = distanceToPlane / abs(distanceToPlane); // Returns 1 if distanceToPlane is positive, returns -1 if distanceToPlane is negative

        //// calculate point q (projection of p on the triangle)
        //vec3 q = p - (distanceToPlane * triNormal);

        //// Calculate the area of q to each set of two points
        //float q01Area = triangleSurface(q, triPoint0, triPoint1);
        //float q02Area = triangleSurface(q, triPoint0, triPoint2);
        //float q12Area = triangleSurface(q, triPoint1, triPoint2);

        float cosAngle = dot(triNormal, p - triPoint0) / (distance(p, triPoint0) * length(triNormal));
        float projectionLength = length(p - triPoint0) * cosAngle;
        float positive = 1;
        if (projectionLength < 0) positive = -1;
        //std::cout << "\t\t\t\t\t\tPositive: " << positive << std::endl;
        vec3 towardProjection = -projectionLength * (triNormal / length(triNormal));
        // Q is the projection of p onto the plane
        vec3 q = p + towardProjection;

        //vec3 v1 = ((triPoint0 - triPoint1) / length(triPoint0 - triPoint1)) + ((triPoint0 - triPoint2) / length(triPoint0 - triPoint2));
        //vec3 v2 = ((triPoint1 - triPoint2) / length(triPoint1 - triPoint2)) + ((triPoint1 - triPoint0) / length(triPoint1 - triPoint0));
        ////vec3 v3 = ((triPoint2 - triPoint0) / length(triPoint2 - triPoint0)) + ((triPoint2 - triPoint1) / length(triPoint2 - triPoint1));

        //float f1 = dot( v1 * (q-triPoint0), triNormal); // if > 0 -> q is anticlockwise of V1 (applies to other f's as well)
        //float f2 = dot( v2 * (q - triPoint1), triNormal);

        //
        //if (!(f1 < 0 && f2 > 0 && dot((q - triPoint0) * (q - triPoint1), triNormal) < 0))
        //{
        //    // q is in triangle
        //    std::cout << "\t\t\t\t\t\tReturning distance: " << projectionLength << std::endl;
        //    //return projectionLength;
        //}

         // Old way of finding if the point is in the triangle
        double q01Area = triangleSurface(q, triPoint0, triPoint1);
        double q02Area = triangleSurface(q, triPoint0, triPoint2);
        double q12Area = triangleSurface(q, triPoint1, triPoint2);
        double triArea = triangleSurface(triPoint0, triPoint1, triPoint2);
        double area = (q01Area + q02Area + q12Area);
        //std::cout << "\t\t\t\t\t\tAreas: " << q01Area << " + " << q02Area << " + " << q12Area << " = " << area << " == " << triArea << std::endl;

        // If the area of q to each set of two points is equal to the triangle surface area, q is on the triangle
        if (math::close_enough(area, triArea))
        {
            //std::cout << "\t\t\t\t\t\tclose enough!" << std::endl;
            // The distance is simply the distance between the original point and the projected point of p (q)
            //std::cout << "\t\t\t\t\t\tReturning distance to q: " << (distance(p, q) * positive) << std::endl;
            //return distance(p, q)*positive;
            //std::cout << "\t\t\t\t\t\tReturning distance to q, diff: " << abs(distance(p, q)*positive-projectionLength) << std::endl;
            //std::cout << "\t\t\t\t\t\tReturning distance: " << projectionLength << std::endl;
            //std::cout << "\t\t\t\t\t\tTo point: " << length(p - q) << std::endl;
            return projectionLength;
        }

        //std::cout << "\t\t\t\t\t\tCalculating line distances: " << std::endl;

        //Point q is not on the triangle, check distance toward each edge of the triangle
        float distance01 = pointToLine(p, triPoint1, triPoint0);
        float distance02 = pointToLine(p, triPoint2, triPoint0);
        float distance12 = pointToLine(p, triPoint2, triPoint1);
        //std::cout << "\t\t\t\t\t\t\t01: " << distance01 << "\n";
        //std::cout << "\t\t\t\t\t\t\t02: " << distance02 << "\n";
        //std::cout << "\t\t\t\t\t\t\t12: " << distance12 << "\n";

        // Assume the shortest distance is sqDistance01
        // Then check if this is true
        float shortestDistance = distance01;
        if (distance02 < distance12)
        {
            if (distance02 < distance01) shortestDistance = distance02;
        }
        else if (distance12 < distance01) shortestDistance = distance12;

        //std::cout << "\t\t\t\t\t\tReturning shortest distance: " << shortestDistance << std::endl;

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
        float cosAngle = dot(triNormal, p - triPoint0) / (distance(p, triPoint0) * length(triNormal));
        float projectionLength = length(p - triPoint0) * cosAngle;
        vec3 towardProjection = -projectionLength * (triNormal / length(triNormal));
        // Q is the projection of p onto the plane
        vec3 q = p + towardProjection;
        float q01Area = triangleSurface(q, triPoint0, triPoint1);
        float q02Area = triangleSurface(q, triPoint0, triPoint2);
        float q12Area = triangleSurface(q, triPoint1, triPoint2);

        // If the area of q to each set of two points is equal to the triangle surface area, q is on the triangle
        if (math::close_enough(q01Area + q02Area + q12Area, triangleSurface(triPoint0, triPoint1, triPoint2)))
        {
            //std::cout << "\t\t\t\t\t\t Close enough!" << std::endl;
            return true;
        }
        //std::cout << "\t\t\t\t\t\t Not Close enough!" << std::endl;
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
            // Distance to plane
            float distanceToPlane = dot(normal, p - points[0]);
            // calculate point q (projection of p on the triangle)
            vec3 q = p - (distanceToPlane*normal);

            // Calculate the area of q to each set of two points
            float q01Area = triangleSurface(q, points[0], points[1]);
            float q02Area = triangleSurface(q, points[0], points[2]);
            float q12Area = triangleSurface(q, points[1], points[2]);

            // If the area of q to each set of two points is equal to the triangle surface area, q is on the triangle
            if (q01Area + q02Area + q12Area == surface())
            {
                // The distance is simply the distance between the original point and the projected point of p (q)
                return distance(p, q);
            }

            //Point q is not on the triangle, check distance toward each edge of the triangle
            float distance01 = pointToLine(p, points[0], points[1]);
            float distance02 = pointToLine(p, points[0], points[2]);
            float distance12 = pointToLine(p, points[1], points[2]);

            // Assume the shortest distance is sqDistance01
            // Then check if this is true
            float shortestDistance = distance01;
            if (distance02 < distance12)
            {
                if (distance02 < distance01) shortestDistance = distance02;
            }
            else if (distance12 < distance01) shortestDistance = distance12;

            return sqrt(shortestDistance);
        }

        /**@brief Calculates the size of the surface area
         */
        float surface() const
        {
            return 0.5f * (normalizeDot(points[0], points[1]) * normalizeDot(points[0], points[2])) * fastSin(0.f);
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
