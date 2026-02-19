#include <LibWeb/MediaSourceExtensions/WebMSegmentParser.h>

namespace Web::MediaSourceExtensions {

WebMSegmentParser::WebMSegmentParser() = default;
WebMSegmentParser::~WebMSegmentParser() = default;

ErrorOr<void> WebMSegmentParser::open_format_context()
{
    AVInputFormat* input_format = av_find_input_format("matroska");
    if (!input_format)
        return Error::from_string_literal("Failed to find matroska input format");

    int result = avformat_open_input(&m_format_context, nullptr, input_format, nullptr);
    if (result < 0) {
        // FIXME: Propagate a proper error.
        return Error::from_string_literal("Failed to open input");
    }
    return {};
}

ErrorOr<Vector<SegmentParser::ParsedSegment>> WebMSegmentParser::parse_frames()
{
    // This is identical to the MP4 parser for now.
    // FIXME: This logic should be moved to the FFmpegSegmentParser base class.
    Vector<CodedFrame> frames;
    AVPacket* packet = av_packet_alloc();
    while (av_read_frame(m_format_context, packet) >= 0) {
        CodedFrame frame;
        frame.track_id = packet->stream_index;

        AVStream* stream = m_format_context->streams[packet->stream_index];
        double time_base = av_q2d(stream->time_base);
        frame.presentation_timestamp = packet->pts * time_base;
        frame.decode_timestamp = packet->dts * time_base;
        frame.duration = packet->duration * time_base;
        
        frame.is_key_frame = (packet->flags & AV_PKT_FLAG_KEY) != 0;
        frame.data = TRY(ByteBuffer::copy(packet->data, packet->size));

        frames.append(move(frame));
        av_packet_unref(packet);
    }
    av_packet_free(&packet);

    if (frames.is_empty())
        return Vector<ParsedSegment> {};

    ParsedSegment segment;
    segment.is_initialization_segment = false;
    segment.coded_frames = move(frames);

    Vector<ParsedSegment> segments;
    segments.append(move(segment));
    return segments;
}

}
