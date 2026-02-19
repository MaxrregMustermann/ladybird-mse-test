#pragma once

#include <LibWeb/MediaSourceExtensions/SegmentParser.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
}

namespace Web::MediaSourceExtensions {

// This is a base class for segment parsers that use FFmpeg for demuxing.
// It handles the common logic of setting up a custom AVIOContext for reading
// from an in-memory buffer.
class FFmpegSegmentParser : public SegmentParser {
public:
    virtual ~FFmpegSegmentParser() override;

    ErrorOr<Vector<ParsedSegment>> append(ReadonlyBytes data) override;
    virtual void reset() override;

protected:
    FFmpegSegmentParser();
    virtual ErrorOr<void> open_format_context() = 0;

    AVFormatContext* m_format_context { nullptr };
    AVIOContext* m_avio_context { nullptr };
    bool m_is_initialized { false };

private:
    static int avio_read_packet(void* opaque, uint8_t* buf, int buf_size);
    
    ByteBuffer m_buffer;
    size_t m_read_offset { 0 };
};

}
