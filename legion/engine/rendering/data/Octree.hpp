#pragma once
#include<core/core.hpp>
#include <rendering/debugrendering.hpp>
namespace legion::rendering
{
    template <typename ValueType>
    class Octree
    {
    private:
        enum Subsection : uint8
        {
            TopLeftFront,
            TopRightFront,
            BotRightFront,
            BotLeftFront,
            TopLeftBack,
            TopRightBack,
            BotRightBack,
            BotLeftBack
        };

    public:

        Octree() = default;
        Octree(size_type capacity, math::vec3 min, math::vec3 max, math::vec3 position)
            : m_capacity(capacity), m_min(min), m_max(max), m_position(position) {};

        Octree(size_type capacity, math::vec3 min, math::vec3 max)
            : m_capacity(capacity), m_min(min), m_max(max)
        {
            m_position = (min + max) * 0.5f;
        };

        void insertNode(ValueType item, const math::vec3& pos)
        {
            //exit if node does not fit inside tree
            if (!fitsNode(pos)) return;

            if (!m_children)
            {
                //store item if possible
                if (m_items.size() < m_capacity)
                {
                    m_items.push_back(std::make_pair(pos, item));
                    return;
                }
                //initialize children
                m_children = std::make_unique<std::array<Octree, 8>>();
                m_children->at(Subsection::TopLeftFront) = Octree<ValueType>(m_capacity, math::vec3(m_min.x, m_position.y, m_min.z), math::vec3(m_position.x, m_max.y, m_position.z));
                m_children->at(Subsection::TopRightFront) = Octree<ValueType>(m_capacity, math::vec3(m_position.x, m_position.y, m_min.z), math::vec3(m_max.x, m_max.y, m_position.z));
                m_children->at(Subsection::BotRightFront) = Octree<ValueType>(m_capacity, math::vec3(m_position.x, m_min.y, m_min.z), math::vec3(m_max.x, m_position.y, m_position.z));
                m_children->at(Subsection::BotLeftFront) = Octree<ValueType>(m_capacity, math::vec3(m_min.x, m_min.y, m_min.z), math::vec3(m_position.x, m_position.y, m_position.z));
                m_children->at(Subsection::TopLeftBack) = Octree<ValueType>(m_capacity, math::vec3(m_min.x, m_position.y, m_position.z), math::vec3(m_position.x, m_max.y, m_max.z));
                m_children->at(Subsection::TopRightBack) = Octree<ValueType>(m_capacity, math::vec3(m_position.x, m_position.y, m_position.z), math::vec3(m_max.x, m_max.y, m_max.z));
                m_children->at(Subsection::BotRightBack) = Octree<ValueType>(m_capacity, math::vec3(m_position.x, m_min.y, m_position.z), math::vec3(m_max.x, m_position.y, m_max.z));
                m_children->at(Subsection::BotLeftBack) = Octree<ValueType>(m_capacity, math::vec3(m_min.x, m_min.y, m_position.z), math::vec3(m_position.x, m_position.y, m_max.z));
                //pass in current hold data into newly created children
                for (auto& [itemPos, item] : m_items)
                {
                    int childIndex = GetChildIndex(itemPos);
                    m_children->at(childIndex).insertNode(item, itemPos);
                }
            }
            int childIndex = GetChildIndex(pos);
            m_children->at(childIndex).insertNode(item, pos);
        };
        void DrawTree()
        {
            debug::drawCube(math::vec3(m_min.x, m_min.y, m_min.z), math::vec3(m_max.x, m_max.y, m_max.z), math::colors::black);
            if (m_children)
            {
                //for all children draw tree if its not a leaf
                for (int i = 0; i < 8; i++)
                {
                    m_children->at(i).DrawTree();
                }
            }
        };
        math::vec3 GetAverage()
        {
            return m_averagePos;
        }

        std::unique_ptr<std::array<Octree, 8>>  m_children;
        std::vector<math::vec3> GetData(int depth)
        {
            std::vector<math::vec3> Data;
            //check if this tree has children
            if (m_children)
            {
                //if depth 0 has not been reached go deeper and decrease depth
                if (depth > 0)
                {
                    for (int i = 0; i < 8; i++)
                    {
                        std::vector<math::vec3> newData = m_children->at(i).GetData(depth - 1);

                        Data.insert(Data.end(), newData.begin(), newData.end());
                    }
                }
                //if depth 0 has been reached get the average of all the left children
                else
                {
                    for (int i = 0; i < 8; i++)
                    {
                        Data.push_back(std::get<0>(m_items.at(i)));
                    }
                }
            }
            //if there are no children get all data items
            else
            {
                for (auto& [itemPos, item] : m_items)
                {
                    Data.push_back(itemPos);
                }
            }
            return Data;
        }
        void GenerateAverage()
        {
            //check if the octant has more child octants
            if (m_children)
            {
                //iterate child octants and generate their average
                for (int i = 0; i < 8; i++)
                {
                    m_children->at(i).GenerateAverage();
                }
                //accumulate child values
                math::vec3 cummulatedPos = math::vec3(0, 0, 0);
                int weight = 0;
                for (int i = 0; i < 8; i++)
                {
                    cummulatedPos += m_children->at(i).GetAverage();
                    //increment weight if position is not (0,0,0) -> empty
                    if (math::length(m_children->at(i).GetAverage()) >= math::epsilon<float>())
                        weight++;

                }
                if (weight == 0) m_averagePos = math::vec3(0, 0, 0);
                else m_averagePos = cummulatedPos / (float)weight;
            }
            else
            {
                //if there are no child octants get average all stored items together
                math::vec3 cummulatedPos = math::vec3(0, 0, 0);
                for (auto& [itemPos, item] : m_items)
                {
                    cummulatedPos += itemPos;
                }
                m_averagePos = cummulatedPos / m_items.capacity();
            }
        }
    private:
        int GetChildIndex(const math::vec3& pos)
        {
            if (pos.x < m_position.x)
            {
                if (pos.y > m_position.y)
                {
                    if (pos.z < m_position.z)
                        return Subsection::TopLeftFront;
                    else
                        return Subsection::TopLeftBack;
                }
                else
                {
                    if (pos.z < m_position.z)
                        return Subsection::BotLeftFront;
                    else
                        return Subsection::BotLeftBack;
                }
            }
            else
            {
                if (pos.y > m_position.y)
                {
                    if (pos.z < m_position.z)
                        return Subsection::TopRightFront;
                    else
                        return Subsection::TopRightBack;
                }
                else
                {
                    if (pos.z < m_position.z)
                        return Subsection::BotRightFront;
                    else
                        return Subsection::BotRightBack;
                }

            }
        }
        bool fitsNode(const math::vec3& position)
        {
            return
                !(
                    position.x < m_min.x
                    || position.y < m_min.y
                    || position.z < m_min.z
                    || position.x > m_max.x
                    || position.y > m_max.y
                    || position.z > m_max.z
                    );
        }

        std::vector <std::pair<math::vec3, ValueType>>m_items;
        size_type m_capacity;

        math::vec3 m_min;
        math::vec3 m_max;
        math::vec3 m_position;
        math::vec3 m_averagePos;
    };
}
