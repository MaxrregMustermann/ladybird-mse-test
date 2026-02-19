#include <LibWeb/MediaSourceExtensions/SourceBuffer.h>

namespace Web::MediaSourceExtensions {

SourceBuffer::SourceBuffer(JS::Realm& realm, MediaSource& media_source, String mime_type)
    : DOM::EventTarget(realm)
    , m_parent_media_source(media_source)
    , m_mime_type(move(mime_type))
{
}

SourceBuffer::~SourceBuffer() = default;

} // namespace Web::MediaSourceExtensions
