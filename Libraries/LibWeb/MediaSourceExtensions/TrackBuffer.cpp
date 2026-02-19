#include <LibWeb/MediaSourceExtensions/TrackBuffer.h>
#include <AK/BinarySearch.h>

namespace Web::MediaSourceExtensions {

void TrackBuffer::insert_coded_frame(Frame frame)
{
    // Find the correct insertion point to maintain PTS order.
    auto it = m_frames.find_if([&](auto const& other) {
        return other.pts > frame.pts;
    });
    size_t index = m_frames.insert(it - m_frames.begin(), move(frame));

    // If the new frame is a keyframe, update our keyframe index.
    if (frame.is_keyframe) {
        auto keyframe_it = m_keyframe_indices.find_if([&](auto const& other_index) {
            return m_frames[other_index].pts > frame.pts;
        });
        m_keyframe_indices.insert(keyframe_it - m_keyframe_indices.begin(), index);
    }
}

void TrackBuffer::remove_frames_in_range(double start, double end)
{
    // This is complex because we need to rebuild the keyframe index after removal.
    // A simpler approach for now is to just rebuild it from scratch.
    m_frames.remove_all_matching([&](auto const& frame) {
        return frame.pts >= start && frame.pts < end;
    });

    m_keyframe_indices.clear();
    for (size_t i = 0; i < m_frames.size(); ++i) {
        if (m_frames[i].is_keyframe) {
            m_keyframe_indices.append(i);
        }
    }
}

TrackBuffer::Frame const* TrackBuffer::get_frame_at(double presentation_time)
{
    if (m_frames.is_empty())
        return nullptr;

    auto it = AK::binary_search(m_frames, presentation_time, nullptr, [](auto const& time, auto const& frame) {
        if (time < frame.pts)
            return -1;
        if (time > frame.pts)
            return 1;
        return 0;
    });

    if (it == m_frames.end()) {
        // No exact match, find the closest frame before the time.
        // `binary_search` doesn't give us the lower bound, so we have to do it manually.
        // FIXME: This is a linear scan, which is inefficient.
        for (ssize_t i = m_frames.size() - 1; i >= 0; --i) {
            if (m_frames[i].pts <= presentation_time)
                return &m_frames[i];
        }
        return nullptr;
    }

    return &*it;
}

JS::NonnullGCPtr<HTML::TimeRanges> TrackBuffer::compute_buffered_ranges(JS::Realm& realm) const
{
    auto time_ranges = HTML::TimeRanges::create(realm);
    if (m_frames.is_empty())
        return time_ranges;

    double range_start = m_frames.first().pts;
    double range_end = m_frames.first().pts + m_frames.first().duration;

    // A small tolerance to merge adjacent ranges.
    constexpr double tolerance = 0.1;

    for (size_t i = 1; i < m_frames.size(); ++i) {
        auto const& frame = m_frames[i];
        // If the gap between the last frame and this one is too large, start a new range.
        if (frame.pts > range_end + tolerance) {
            time_ranges->add(range_start, range_end);
            range_start = frame.pts;
            range_end = frame.pts + frame.duration;
        } else {
            // Otherwise, extend the current range.
            range_end = frame.pts + frame.duration;
        }
    }

    // Add the last range.
    time_ranges->add(range_start, range_end);

    return time_ranges;
}

void TrackBuffer::reset_parser_state()
{
    m_frames.clear();
    m_keyframe_indices.clear();
}

void TrackBuffer::evict_coded_frames(double current_playback_time, size_t needed_bytes)
{
    size_t freed_bytes = 0;

    // Pass 1: Remove frames that have already been played.
    // We'll keep a 10-second buffer behind the current time.
    m_frames.remove_all_matching([&](auto const& frame) {
        if (frame.pts < current_playback_time - 10.0) {
            freed_bytes += frame.encoded_data.size();
            return true;
        }
        return false;
    });

    if (freed_bytes >= needed_bytes) {
        rebuild_keyframe_index();
        return;
    }

    // Pass 2: Remove frames far in the future.
    // We'll keep a 30-second buffer ahead of the current time.
    m_frames.remove_all_matching([&](auto const& frame) {
        if (frame.pts > current_playback_time + 30.0) {
            freed_bytes += frame.encoded_data.size();
            return true;
        }
        return false;
    });

    rebuild_keyframe_index();
}

void TrackBuffer::rebuild_keyframe_index()
{
    m_keyframe_indices.clear();
    for (size_t i = 0; i < m_frames.size(); ++i) {
        if (m_frames[i].is_keyframe) {
            m_keyframe_indices.append(i);
        }
    }
}


}
