#include <LibWeb/MediaSourceExtensions/MP4SegmentParser.h>

namespace Web::MediaSourceExtensions {

MP4SegmentParser::MP4SegmentParser() = default;
MP4SegmentParser::~MP4SegmentParser() = default;

ErrorOr<void> MP4SegmentParser::open_format_context()
{
    int result = avformat_open_input(&m_format_context, nullptr, nullptr, nullptr);
    if (result < 0) {
        // FIXME: Propagate a proper error.
        return Error::from_string_literal("Failed to open input");
    }
    return {};
}

}
