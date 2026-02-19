#include <LibWeb/MediaSourceExtensions/MP4SegmentParser.h>
#include <LibWeb/MediaSourceExtensions/SegmentParser.h>
#include <LibWeb/MimeSniff/MimeType.h>

// FIXME: Include the WebM parser implementation once it exists.
// #include <LibWeb/MediaSourceExtensions/WebMSegmentParser.h>

namespace Web::MediaSourceExtensions {

OwnPtr<SegmentParser> SegmentParser::create_for_mime_type(StringView mime_type_string)
{
    auto mime_type = MimeSniff::MimeType::parse(mime_type_string);
    if (!mime_type.has_value())
        return nullptr;

    if (mime_type->essence() == "video/mp4"sv || mime_type->essence() == "audio/mp4"sv) {
        return make<MP4SegmentParser>();
    }

    if (mime_type->essence() == "video/webm"sv || mime_type->essence() == "audio/webm"sv) {
        // FIXME: Return an instance of WebMSegmentParser when it's implemented.
        return nullptr;
    }

    return nullptr;
}

} // namespace Web::MediaSourceExtensions
