#ifndef __TRACYTIMELINECONTROLLER_HPP__
#define __TRACYTIMELINECONTROLLER_HPP__

#include "../public/common/TracyForceInline.hpp"
#include "tracy_robin_hood.h"

namespace tracy
{

class TimelineController
{
    struct VisData
    {
        bool visible = true;
        bool showFull = true;
        bool ghost = false;
        int offset = 0;
        int height = 0;
    };

public:
    TimelineController();

    void FirstFrameExpired();
    void End( float offset );

    float GetHeight() const { return m_height; }
    const unordered_flat_map<const void*, VisData>& GetVisData() const { return m_visData; }

    void AdjustThreadHeight( VisData& vis, int oldOffset, int& offset );
    float AdjustThreadPosition( VisData& vis, float wy, int& offset );

    tracy_force_inline VisData& Vis( const void* ptr )
    {
        auto it = m_visData.find( ptr );
        if( it == m_visData.end() )
        {
            it = m_visData.emplace( ptr, VisData {} ).first;
        }
        return it->second;
    }

private:
    unordered_flat_map<const void*, VisData> m_visData;

    float m_height;
    float m_offset;
    float m_scroll;

    bool m_firstFrame;
};

}

#endif
