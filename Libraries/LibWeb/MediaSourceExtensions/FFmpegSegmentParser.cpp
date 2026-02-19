#include <LibWeb/MediaSourceExtensions/FFmpegSegmentParser.h>

namespace Web::MediaSourceExtensions {

FFmpegSegmentParser::FFmpegSegmentParser()
{
    m_format_context = avformat_alloc_context();
    VERIFY(m_format_context);

    size_t const buffer_size = 4096;
    auto* avio_buffer = static_cast<unsigned char*>(av_malloc(buffer_size));
    VERIFY(avio_buffer);

    m_avio_context = avio_alloc_context(
        avio_buffer, buffer_size,
        0,
        this,
        &FFmpegSegmentParser::avio_read_packet,
        nullptr,
        nullptr);
    VERIFY(m_avio_context);

    m_format_context->pb = m_avio_context;
}

FFmpegSegmentParser::~FFmpegSegmentParser()
{
    if (m_format_context)
        avformat_close_input(&m_format_context);

    if (m_avio_context)
        avio_context_free(&m_avio_context);
}

void FFmpegSegmentParser::reset()
{
    if (m_format_context) {
        avformat_close_input(&m_format_context);
        m_format_context = avformat_alloc_context();
        VERIFY(m_format_context);
        m_format_context->pb = m_avio_context;
    }
    m_is_initialized = false;
    m_buffer.clear();
    m_read_offset = 0;
}

ErrorOr<Vector<SegmentParser::ParsedSegment>> FFmpegSegmentParser::append(ReadonlyBytes data)
{
    if (m_read_offset >= m_buffer.size()) {
        m_buffer.clear();
        m_read_offset = 0;
    } else if (m_read_offset > 0) {
        m_buffer.span().slice(m_read_offset).copy_to(m_buffer.span());
        m_buffer.resize(m_buffer.size() - m_read_offset);
        m_read_offset = 0;
    }
    m_buffer.append(data);

    Vector<ParsedSegment> segments;

    if (!m_is_initialized) {
        auto result = open_format_context();
        if (result.is_error()) {
            reset();
            return result.release_error();
        }
        
        int ff_result = avformat_find_stream_info(m_format_context, nullptr);
        if (ff_result < 0) {
            reset();
            return Vector<ParsedSegment> {};
        }
        m_is_initialized = true;

        ParsedSegment init_segment;
        init_segment.is_initialization_segment = true;
        for (unsigned i = 0; i < m_format_context->nb_streams; ++i) {
            AVStream* stream = m_format_context->streams[i];
            AVCodecParameters* codec_parameters = stream->codecpar;

            TrackInfo track_info;
            track_info.track_id = stream->index;

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
            track_info.codec_string = MUST(String::from_utf8(codec_name_buffer));
            
            if (codec_parameters->extradata_size > 0) {
                track_info.codec_private_data = MUST(ByteBuffer::copy(codec_parameters->extradata, codec_parameters->extradata_size));
            }
            init_segment.tracks.append(move(track_info));
        }
        segments.append(move(init_segment));
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

    if (!frames.is_empty()) {
        ParsedSegment media_segment;
        media_segment.is_initialization_segment = false;
        media_segment.coded_frames = move(frames);
        segments.append(move(media_segment));
    }

    return segments;
}

int FFmpegSegmentParser::avio_read_packet(void* opaque, uint8_t* buf, int buf_size)
{
    auto& parser = *static_cast<FFmpegSegmentParser*>(opaque);
    auto remaining_bytes = parser.m_buffer.size() - parser.m_read_offset;
    auto to_read = min((size_t)buf_size, remaining_bytes);
    if (to_read == 0)
        return AVERROR_EOF;

    memcpy(buf, parser.m_buffer.data() + parser.m_read_offset, to_read);
    parser.m_read_offset += to_read;
    return to_read;
}

}
