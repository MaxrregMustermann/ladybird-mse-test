#pragma once

#include <LibWeb/MediaSourceExtensions/SegmentParser.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
}

namespace Web::MediaSourceExtensions {

class MP4SegmentParser final : public SegmentParser {
public:
    MP4SegmentParser();
    virtual ~MP4SegmentParser() override;

    virtual ErrorOr<Vector<ParsedSegment>> append(ReadonlyBytes) override;
    virtual void reset() override;

private:
    static int avio_read_packet(void* opaque, uint8_t* buf, int buf_size);

    AVFormatContext* m_format_context { nullptr };
    AVIOContext* m_avio_context { nullptr };
    ByteBuffer m_buffer;
    bool m_initialized { false };
};

}
