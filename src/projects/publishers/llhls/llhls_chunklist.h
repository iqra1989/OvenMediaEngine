//==============================================================================
//
//  OvenMediaEngine
//
//  Created by Getroot
//  Copyright (c) 2022 AirenSoft. All rights reserved.
//
//==============================================================================
#pragma once

#include <base/ovlibrary/ovlibrary.h>
#include <base/info/media_track.h>
#include <base/mediarouter/media_buffer.h>

class LLHlsChunklist
{
public:
	class SegmentInfo
	{
	public:
		SegmentInfo(uint32_t sequence)
			: _sequence(sequence)
		{
		}

		SegmentInfo(uint32_t sequence, uint64_t start_time, float_t duration, uint64_t size, ov::String url, ov::String next_url, bool is_independent)
			: _sequence(sequence)
			, _start_time(start_time)
			, _duration(duration)
			, _size(size)
			, _url(url)
			, _next_url(next_url)
			, _is_independent(is_independent)
		{
		}

		void UpdateInfo(uint64_t start_time, float_t duration, uint64_t size, ov::String url, bool is_independent)
		{
			this->_start_time = start_time;
			this->_duration = duration;
			this->_size = size;
			this->_url = url;
			this->_is_independent = is_independent;
		}

		uint64_t GetStartTime() const
		{
			if (_partial_segments.empty() == false)
			{
				return _partial_segments[0]->GetStartTime();
			}

			return _start_time;
		}

		int64_t GetSequence() const
		{
			return _sequence;
		}

		float_t GetDuration() const
		{
			return _duration;
		}

		uint64_t GetSize() const
		{
			return _size;
		}

		ov::String GetUrl() const
		{
			return _url;
		}

		ov::String GetNextUrl() const
		{
			return _next_url;
		}

		bool IsIndependent() const
		{
			return _is_independent;
		}

		// Insert partial segment info
		bool InsertPartialSegmentInfo(const std::shared_ptr<SegmentInfo> &partial_segment)
		{
			if (partial_segment == nullptr)
			{
				return false;
			}

			_partial_segments.push_back(partial_segment);

			return true;
		}

		// Get partial segments
		const std::deque<std::shared_ptr<SegmentInfo>> &GetPartialSegments() const
		{
			return _partial_segments;
		}

		void SetCompleted()
		{
			_completed = true;
		}

		bool IsCompleted() const
		{
			return _completed;
		}

	private:
		int64_t _sequence = -1;
		uint64_t _start_time; // milliseconds since epoce (1970-01-01 00:00:00)
		float_t _duration; // seconds
		uint64_t _size;
		ov::String _url;
		ov::String _next_url;
		bool _is_independent = false;
		bool _completed = false;

		std::deque<std::shared_ptr<SegmentInfo>> _partial_segments;
	}; // class SegmentInfo

	LLHlsChunklist(const std::shared_ptr<const MediaTrack> &track, uint32_t max_segments, uint32_t target_duration, float part_target_duration, const ov::String &map_uri);

	bool AppendSegmentInfo(const SegmentInfo &info);
	bool AppendPartialSegmentInfo(uint32_t segment_sequence, const SegmentInfo &info);

	ov::String ToString(bool skip=false) const;
	std::shared_ptr<const ov::Data> ToGzipData(bool skip=false) const;

	std::shared_ptr<SegmentInfo> GetSegmentInfo(uint32_t segment_sequence) const;
	bool GetLastSequenceNumber(int64_t &msn, int64_t &psn) const;

private:

	int64_t GetSegmentIndex(uint32_t segment_sequence) const;
	ov::String GetPlaylist(bool skip) const;

	std::shared_ptr<const MediaTrack> _track;

	uint32_t _max_segments;
	float_t _target_duration;
	float_t _part_target_duration;
	ov::String _map_uri;

	int64_t _last_segment_sequence = -1;
	int64_t _last_partial_segment_sequence = -1;

	mutable bool _need_playlist_updated = true;
	mutable ov::String _playlist_cache;
	mutable ov::String _playlist_skipped_cache;
	mutable std::shared_mutex _playlist_cache_guard;

	mutable bool _need_gzipped_playlist_updated = true;
	mutable std::shared_ptr<ov::Data> _gzipped_playlist_cache;
	mutable std::shared_ptr<ov::Data> _gzipped_playlist_skipped_cache;
	mutable std::shared_mutex _gzipped_playlist_cache_guard;

	// Segment number -> SegmentInfo
	std::deque<std::shared_ptr<SegmentInfo>> _segments;
	mutable std::shared_mutex _segments_guard;
	uint64_t _deleted_segments = 0;
};