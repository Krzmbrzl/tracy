#include <assert.h>

#include "TracyUtility.hpp"

namespace tracy
{

// Short list based on GetTypes() in TracySourceTokenizer.cpp
constexpr const char* TypesList[] = {
    "bool ", "char ", "double ", "float ", "int ", "long ", "short ",
    "signed ", "unsigned ", "void ", "wchar_t ", "size_t ", "int8_t ",
    "int16_t ", "int32_t ", "int64_t ", "intptr_t ", "uint8_t ", "uint16_t ",
    "uint32_t ", "uint64_t ", "ptrdiff_t ", nullptr
};

const char* ShortenZoneName( ShortenName type, const char* name, ImVec2& tsz, float zsz )
{
    assert( type != ShortenName::Never );
    if( name[0] == '<' ) return name;
    if( type == ShortenName::Always ) zsz = 0;

    static char buf[64*1024];
    char tmp[64*1024];

    auto end = name;
    while( *end ) end++;

    auto ptr = name;
    auto dst = tmp;
    int cnt = 0;
    for(;;)
    {
        auto start = ptr;
        while( ptr < end && *ptr != '<' ) ptr++;
        memcpy( dst, start, ptr - start + 1 );
        dst += ptr - start + 1;
        if( ptr == end ) break;
        cnt++;
        ptr++;
        while( cnt > 0 )
        {
            if( ptr == end ) break;
            if( *ptr == '<' ) cnt++;
            else if( *ptr == '>' ) cnt--;
            ptr++;
        }
        *dst++ = '>';
    }

    end = dst-1;
    ptr = tmp;
    dst = buf;
    cnt = 0;
    for(;;)
    {
        auto start = ptr;
        while( ptr < end && *ptr != '(' ) ptr++;
        memcpy( dst, start, ptr - start + 1 );
        dst += ptr - start + 1;
        if( ptr == end ) break;
        cnt++;
        ptr++;
        while( cnt > 0 )
        {
            if( ptr == end ) break;
            if( *ptr == '(' ) cnt++;
            else if( *ptr == ')' ) cnt--;
            ptr++;
        }
        *dst++ = ')';
    }

    end = dst-1;
    if( end - buf > 6 && memcmp( end-6, " const", 6 ) == 0 )
    {
        dst[-7] = '\0';
        end -= 6;
    }

    ptr = buf;
    for(;;)
    {
        auto match = TypesList;
        while( *match )
        {
            auto m = *match;
            auto p = ptr;
            while( *m )
            {
                if( *m != *p ) break;
                m++;
                p++;
            }
            if( !*m )
            {
                ptr = p;
                break;
            }
            match++;
        }
        if( !*match ) break;
    }

    tsz = ImGui::CalcTextSize( ptr, end );
    if( type == ShortenName::OnlyNormalize || tsz.x < zsz ) return ptr;

    for(;;)
    {
        auto p = ptr;
        while( p < end && *p != ':' ) p++;
        if( p == end ) return ptr;
        p++;
        while( p < end && *p == ':' ) p++;
        ptr = p;
        tsz = ImGui::CalcTextSize( ptr, end );
        if( tsz.x < zsz ) return ptr;
    }
}

void TooltipNormalizedName( const char* name, const char* normalized )
{
    if( ImGui::IsItemHovered() && normalized != name && strcmp( normalized, name ) != 0 )
    {
        const auto scale = ImGui::GetTextLineHeight() / 15.f;
        if( ImGui::CalcTextSize( name ).x > 1400 * scale )
        {
            ImGui::SetNextWindowSize( ImVec2( 1400 * scale, 0 ) );
            ImGui::BeginTooltip();
            ImGui::TextWrapped( "%s", name );
        }
        else
        {
            ImGui::BeginTooltip();
            ImGui::TextUnformatted( name );
        }
        ImGui::EndTooltip();
    }
}

}
