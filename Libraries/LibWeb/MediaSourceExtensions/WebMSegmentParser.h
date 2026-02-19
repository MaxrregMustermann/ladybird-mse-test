#pragma once

#include <LibWeb/MediaSourceExtensions/FFmpegSegmentParser.h>

namespace Web::MediaSourceExtensions {

class WebMSegmentParser final : public FFmpegSegmentParser {
public:
    WebMSegmentParser();
    virtual ~WebMSegmentParser() override;

protected:
    virtual ErrorOr<void> open_format_context() override;
    virtual ErrorOr<Vector<ParsedSegment>> parse_frames() override;
};

}
