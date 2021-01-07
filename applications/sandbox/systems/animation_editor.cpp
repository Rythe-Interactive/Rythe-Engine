#include "animation_editor.hpp"

namespace ext
{
    int detail::AnimationSequencer::GetFrameMin() const
    {
        return m_frameMin;
    }

    int detail::AnimationSequencer::GetItemCount() const
    {
        return static_cast<int>(m_currentAnimation.data.size());
    }

    int detail::AnimationSequencer::GetItemTypeCount() const
    {
        return 3;
    }

    int detail::AnimationSequencer::GetFrameMax() const
    {
        return m_frameMax;
    }

    const char* detail::AnimationSequencer::GetItemTypeName(int type) const
    {
        return type == 0 ? "Position" : type == 1 ? "Rotation" : "Scale";
    }

    const char* detail::AnimationSequencer::GetItemLabel(int index) const
    {
        static char tmps[512];
        sprintf_s(tmps, "[%02d] %s", index, make_label(std::get<2>(m_currentAnimation.data[index])));
        return tmps;
    }

    void detail::AnimationSequencer::Get(int index, int** start, int** end, int* type, unsigned* color)
    {
        //index of the rotation key frames


        if (start) *start = &std::get<0>(m_currentAnimation.data[index]);
        if (end) *end = &std::get<1>(m_currentAnimation.data[index]);
        if (type)
        {
            if (std::holds_alternative<position>(std::get<2>(m_currentAnimation.data[index])))
            {
                if (color) *color = 0xA3BE8CFF;
                *type = 0;
            }
            if (std::holds_alternative<rotation>(std::get<2>(m_currentAnimation.data[index])))
            {
                if (color) *color = 0xd08770FF;
                *type = 1;
            }
            if (std::holds_alternative<scale>(std::get<2>(m_currentAnimation.data[index])))
            {
                if (color) *color = 0xb48eadFF;
                *type = 2;
            }
        }
    }

    animation detail::AnimationSequencer::GetAnimation() const
    {
        animation anim;

        for (const auto& [start, end, variant] : m_currentAnimation.data)
        {
            if (std::holds_alternative<position>(variant))
            {
                anim.position_key_frames.emplace_back(end - start, std::get<position>(variant));
            }
            if (std::holds_alternative<rotation>(variant))
            {
                anim.rotation_key_frames.emplace_back(end - start, std::get<rotation>(variant));
            }
            if (std::holds_alternative<scale>(variant))
            {
                anim.scale_key_frames.emplace_back(end - start, std::get<scale>(variant));
            }
        }

        anim.p_accumulator = m_savedAnimationData.pa;
        anim.r_accumulator = m_savedAnimationData.ra;
        anim.s_accumulator = m_savedAnimationData.sa;
        anim.p_index = m_savedAnimationData.pi;
        anim.r_index = m_savedAnimationData.ri;
        anim.s_index = m_savedAnimationData.si;
        anim.looping = m_savedAnimationData.looping;
        anim.running = m_savedAnimationData.running;

        return anim;
    }

    void detail::AnimationSequencer::Add(int type)
    {
        if (type == 0)
            m_currentAnimation.data.emplace_back(0, 0, position());
        if (type == 1)
            m_currentAnimation.data.emplace_back(0, 0, rotation());
        if (type == 2)
            m_currentAnimation.data.emplace_back(0, 0, scale());
    }

    void detail::AnimationSequencer::Del(int index)
    {
        m_currentAnimation.data.erase(m_currentAnimation.data.begin() + index);
    }

    void detail::AnimationSequencer::Duplicate(int index)
    {
        auto& [start, end, anim_data] = m_currentAnimation.data[index];
        m_currentAnimation.data.emplace_back(end, 10, anim_data);
    }

    void detail::AnimationSequencer::SetAnimation(animation* anim)
    {
        m_currentAnimation.data.clear();


        if (anim)
        {
            m_savedAnimationData.pa = anim->p_accumulator;
            m_savedAnimationData.ra = anim->r_accumulator;
            m_savedAnimationData.sa = anim->s_accumulator;
            m_savedAnimationData.pi = anim->p_index;
            m_savedAnimationData.ri = anim->r_index;
            m_savedAnimationData.si = anim->s_index;
            m_savedAnimationData.looping = anim->looping;
            m_savedAnimationData.running = anim->running;

            int start = 0;
            for (auto& entry : anim->position_key_frames)
            {
                m_currentAnimation.data.emplace_back(start, entry.first + start, entry.second);
                start += entry.first + 1;
            }

            start = 0;
            for (auto& entry : anim->rotation_key_frames)
            {
                m_currentAnimation.data.emplace_back(start, entry.first + start, entry.second);
                start += entry.first + 1;
            }

            start = 0;
            for (auto& entry : anim->scale_key_frames)
            {
                m_currentAnimation.data.emplace_back(start, entry.first + start, entry.second);
                start += entry.first + 1;
            }
        }
    }

    std::variant<position, rotation, scale>& detail::AnimationSequencer::GetDataPointAt(int index)
    {
        return std::get<2>(m_currentAnimation.data.at(index));
    }
}
