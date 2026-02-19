#include <LibWeb/MediaSourceExtensions/MediaSource.h>

namespace Web::MediaSourceExtensions {

MediaSource::MediaSource(JS::Realm& realm)
    : DOM::EventTarget(realm)
{
}

MediaSource::~MediaSource() = default;

} // namespace Web::MediaSourceExtensions
