#include <LibWeb/MediaSourceExtensions/MediaSourceRegistry.h>

namespace Web::MediaSourceExtensions {

MediaSourceRegistry& MediaSourceRegistry::the()
{
    static MediaSourceRegistry instance;
    return instance;
}

void MediaSourceRegistry::register_url(String const& url, JS::GCPtr<MediaSource> media_source)
{
    m_registry.set(url, JS::make_handle(*media_source));
}

void MediaSourceRegistry::unregister_url(String const& url)
{
    m_registry.remove(url);
}

JS::GCPtr<MediaSource> MediaSourceRegistry::for_url(String const& url) const
{
    auto handle = m_registry.get(url);
    if (handle.has_value())
        return handle.value()->cell();
    return nullptr;
}

}
