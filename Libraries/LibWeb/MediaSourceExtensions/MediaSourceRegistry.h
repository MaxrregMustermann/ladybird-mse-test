#pragma once

#include <AK/HashMap.h>
#include <AK/String.h>
#include <LibWeb/Forward.h>

namespace Web::MediaSourceExtensions {

class MediaSource;

class MediaSourceRegistry {
public:
    static MediaSourceRegistry& the();

    void register_url(String const&, JS::NonnullGCPtr<MediaSource>);
    void unregister_url(String const&);
    JS::GCPtr<MediaSource> for_url(String const&) const;

private:
    MediaSourceRegistry() = default;
    HashMap<String, JS::Handle<MediaSource>> m_registry;
};

}
