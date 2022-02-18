#pragma once
#include <core/core.hpp>

namespace legion::physics
{
    struct EdgeLabel
    {
        //first pairing states face id and second pair states edge id
        std::pair<int, int> firstEdge = std::make_pair(-1, -1);
        std::pair<int, int> nextEdge = std::make_pair(-1, -1);

        void Log() const
        {
            log::debug("- This EdgeLabel has ");
            log::debug("firstEdge {},{} and secondEdge {},{}",
                firstEdge.first,firstEdge.second,nextEdge.first,nextEdge.second);
        }

        EdgeLabel()
        {

        }

        EdgeLabel(std::pair<int, int> pFirstEdge, std::pair<int, int> pNextEdge) 
        {
            firstEdge = std::make_pair(pFirstEdge.first, pFirstEdge.second);
            nextEdge = std::make_pair(pNextEdge.first, pNextEdge.second);

        }

        EdgeLabel(const EdgeLabel& rhs)
        {
            firstEdge = rhs.firstEdge;
            nextEdge = rhs.nextEdge;
        }

        bool operator==(const EdgeLabel& rhs)
        {
            return firstEdge == rhs.firstEdge && nextEdge == rhs.nextEdge;
        }

        bool IsSet() const
        {
            return ((firstEdge.first != -1 && firstEdge.second != -1)
                && (nextEdge.first != -1 && nextEdge.second != -1));
        }

    };
}
