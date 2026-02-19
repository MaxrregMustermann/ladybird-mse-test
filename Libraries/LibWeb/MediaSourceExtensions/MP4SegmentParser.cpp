#include <LibWeb/MediaSourceExtensions/MP4SegmentParser.h>
#include <AK/MemoryStream.h>

namespace Web::MediaSourceExtensions {

MP4SegmentParser::MP4SegmentParser()
{
    m_format_context = avformat_alloc_context();
    VERIFY(m_format_context);

    // This buffer is used by the AVIOContext for reading.
    // We must provide a buffer, but FFmpeg will manage its use.
    // The size can be a typical page size or a custom value.
    size_t const buffer_size = 4096;
    auto* avio_buffer = static_cast<unsigned char*>(av_malloc(buffer_size));
    VERIFY(avio_buffer);

    m_avio_context = avio_alloc_context(
        avio_buffer, buffer_size,
        0, // write_flag (0 for read-only)
        this, // opaque pointer
        &MP4SegmentParser::avio_read_packet,
        nullptr, // write_packet
        nullptr  // seek
    );
    VERIFY(m_avio_context);

    m_format_context->pb = m_avio_context;
}

MP4SegmentParser::~MP4SegmentParser()
{
    avformat_close_input(&m_format_context);
    // The AVIOContext buffer is freed by avio_context_free.
    avio_context_free(&m_avio_context);
}

ErrorOr<Vector<SegmentParser::ParsedSegment>> MP4SegmentParser::append(ReadonlyBytes data)
{
    m_buffer.append(data);

    if (!m_initialized) {
        // avformat_open_input is a blocking call. It will call our avio_read_packet
        // function until it has enough data to parse the container header.
        int result = avformat_open_input(&m_format_context, nullptr, nullptr, nullptr);
        if (result < 0) {
            // Not enough data has been appended yet to parse the header.
            // Return an empty vector and wait for more data.
            if (result == AVERROR_EOF)
                return Vector<ParsedSegment> {};

            // A real error occurred.
            // FIXME: Propagate a proper error.
            return Vector<ParsedSegment> {};
        }

        result = avformat_find_stream_info(m_format_context, nullptr);
        if (result < 0) {
            // Not enough data to determine stream information. This can happen with fragmented media.
            // We'll try again on the next append.
            // To do this, we must "undo" the initialized state.
            avformat_close_input(&m_format_context);
            m_format_context = nullptr; // avformat_close_input frees the context
            // We need to re-allocate the contexts for the next attempt.
            reset();
            return Vector<ParsedSegment> {};
        }

        m_initialized = true;

        Vector<TrackInfo> tracks;
        for (unsigned i = 0; i < m_format_context->nb_streams; ++i) {
            AVStream* stream = m_format_context->streams[i];
            AVCodecParameters* codec_parameters = stream->codecpar;

            TrackInfo track_info;
            track_info.track_id = stream->id;

            if (codec_parameters->codec_type == AVMEDIA_TYPE_VIDEO) {
                track_info.is_video = true;
                track_info.width = codec_parameters->width;
                track_info.height = codec_parameters->height;
            } else if (codec_parameters->codec_type == AVMEDIA_TYPE_AUDIO) {
                track_info.is_audio = true;
                track_info.sample_rate = codec_parameters->sample_rate;
                track_info.channels = codec_parameters->ch_layout.nb_channels;
            } else {
                continue;
            }

            char codec_name_buffer[AV_CODEC_MAX_NAME_SIZE];
            avcodec_string(codec_name_buffer, sizeof(codec_name_buffer), codec_parameters, 0);
            track_info.codec_string = String::from_utf8(codec_name_buffer).release_value_but_fixme_should_propagate_errors();
            
            if (codec_parameters->extradata_size > 0) {
                track_info.codec_private_data = MUST(ByteBuffer::copy(codec_parameters->extradata, codec_parameters->extradata_size));
            }

            tracks.append(move(track_info));
        }

        ParsedSegment segment;
        segment.is_initialization_segment = true;
        segment.tracks = move(tracks);

        Vector<ParsedSegment> segments;
        segments.append(move(segment));
        return segments;
    }

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
        frame.data = MUST(ByteBuffer::copy(packet->data, packet->size));

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

    // FIXME: For media segments, read frames with av_read_frame and return them.
    return Vector<ParsedSegment> {};
}

void MP4SegmentParser::reset()
{
    if (m_format_context)
        avformat_close_input(&m_format_context);

    if (m_avio_context)
        avio_context_free(&m_avio_context);

    m_initialized = false;
    m_buffer.clear();

    // Re-initialize contexts for the next use.
    m_format_context = avformat_alloc_context();
    VERIFY(m_format_context);

    size_t const buffer_size = 4096;
    auto* avio_buffer = static_cast<unsigned char*>(av_malloc(buffer_size));
    VERIFY(avio_buffer);

    m_avio_context = avio_alloc_context(
        avio_buffer, buffer_size,
        0,
        this,
        &MP4SegmentParser::avio_read_packet,
        nullptr,
        nullptr
    );
    VERIFY(m_avio_context);

    m_format_context->pb = m_avio_context;
}

int MP4SegmentParser::avio_read_packet(void* opaque, uint8_t* buf, int buf_size)
{
    auto& parser = *static_cast<MP4SegmentParser*>(opaque);
    auto to_read = min((size_t)buf_size, parser.m_buffer.size());
    if (to_read == 0)
        return AVERROR_EOF;

    memcpy(buf, parser.m_buffer.data(), to_read);
    // FIXME: This is inefficient. We should use a circular buffer or a different approach
    // to avoid creating a new ByteBuffer on every read.
    parser.m_buffer = parser.m_buffer.slice(to_read);
    return to_read;
}

}
