#pragma once
#include <ctype.h>
#include <stdlib.h>

// Bit masks: zero disables a track, AL accepts either stable game context.
enum CarbonPlaybackMode { CarbonOff = 0, CarbonFE = 1, CarbonIG = 2, CarbonAll = 3 };
static const unsigned int CarbonNoTrack = ~0u;

inline unsigned char ResolveCarbonPlaybackContext(unsigned int state, unsigned char previous)
{
    if (state == 3) return CarbonFE;
    if (state == 6) return CarbonIG;
    return previous;
}

inline unsigned char ParseCarbonPlaybackMode(const char* text, bool& valid)
{
    valid = true;
    if (!text) return CarbonAll;
    while (*text && isspace((unsigned char)*text)) ++text;
    if (!*text) return CarbonAll;
    char token[3] = {};
    unsigned int length = 0;
    while (*text && !isspace((unsigned char)*text) && *text != ';' && *text != '/')
    {
        if (length >= 2) { valid = false; return CarbonAll; }
        token[length++] = (char)toupper((unsigned char)*text++);
    }
    if (token[0] == 'F' && token[1] == 'E') return CarbonFE;
    if (token[0] == 'I' && token[1] == 'G') return CarbonIG;
    if (token[0] == 'A' && token[1] == 'L') return CarbonAll;
    if (token[0] == 'O' && token[1] == 'F') return CarbonOff;
    valid = false;
    return CarbonAll;
}

inline bool CarbonTrackAllowed(bool enabled, unsigned char mode, unsigned char context)
{
    return enabled && (mode & context) != 0;
}

// A bounded scan also handles empty playlists and contexts with no eligible songs.
inline unsigned int SelectCarbonTrack(const bool* enabled, const unsigned char* modes,
    unsigned int count, unsigned char context, bool sequenced, unsigned int& cursor)
{
    if (!count) return CarbonNoTrack;
    if (sequenced)
    {
        for (unsigned int visited = 0; visited < count; ++visited)
        {
            const unsigned int index = cursor % count;
            cursor = (index + 1) % count;
            if (CarbonTrackAllowed(enabled[index], modes[index], context)) return index;
        }
        return CarbonNoTrack;
    }
    unsigned int eligible = 0;
    for (unsigned int i = 0; i < count; ++i)
        if (CarbonTrackAllowed(enabled[i], modes[i], context)) ++eligible;
    if (!eligible) return CarbonNoTrack;
    unsigned int rank = (unsigned int)rand() % eligible;
    for (unsigned int i = 0; i < count; ++i)
        if (CarbonTrackAllowed(enabled[i], modes[i], context) && rank-- == 0) return i;
    return CarbonNoTrack;
}
