#pragma once

#include <LibWeb/MediaSourceExtensions/FFmpegSegmentParser.h>

namespace Web::MediaSourceExtensions {

class MP4SegmentParser final : public FFmpegSegmentParser {
public:
    MP4SegmentParser();
    virtual ~MP4SegmentParser() override;

protected:
    virtual ErrorOr<void> open_format_context() override;
};

}
