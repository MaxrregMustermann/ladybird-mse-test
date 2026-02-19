#pragma once

#include <LibWeb/MediaSourceExtensions/SegmentParser.h>

namespace Web::MediaSourceExtensions {

class SourceBuffer;
class TrackBuffer;

class CodedFrameProcessor {
public:
    CodedFrameProcessor(SourceBuffer&);

    void process(TrackBuffer& track_buffer, SegmentParser::CodedFrame const& frame);

private:
    SourceBuffer& m_source_buffer;
    bool m_need_random_access_point { true };
};

}
