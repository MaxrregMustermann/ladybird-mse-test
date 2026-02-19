#include <LibWeb/MediaSourceExtensions/MediaSourceRegistry.h>

namespace Web::MediaSourceExtensions {

MediaSourceRegistry& MediaSourceRegistry::the()
{
    static MediaSourceRegistry* s_the = nullptr;
    if (!s_the)
        s_the = new MediaSourceRegistry;
    return *s_the;
}

void MediaSourceRegistry::register_url(String const& url, JS::GCPtr<MediaSource> media_source)
{
    m_registry.set(url, media_source);
}

void MediaSourceRegistry::unregister_url(String const& url)
{
    m_registry.remove(url);
}

JS::GCPtr<MediaSource> MediaSourceRegistry::for_url(String const& url) const
{
    return m_registry.get(url).value_or(nullptr);
}

}
