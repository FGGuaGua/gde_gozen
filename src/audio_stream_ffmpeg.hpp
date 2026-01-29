#pragma once

#include "ffmpeg.hpp"
#include "ffmpeg_helpers.hpp"

#include <godot_cpp/classes/audio_stream.hpp>
#include <godot_cpp/classes/audio_stream_playback.hpp>
#include <godot_cpp/classes/audio_stream_playback_resampled.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/mutex.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

using namespace godot;


class AudioStreamFFmpeg : public AudioStream {
	GDCLASS(AudioStreamFFmpeg, AudioStream);

  private:
	// FFmpeg classes.
	UniqueAVFormatCtxInput av_format_ctx;
	UniqueAVCodecCtx av_codec_ctx;
	UniqueAVIOContext avio_ctx;
	UniqueSwrCtx swr_ctx;
	AVStream* av_stream = nullptr;

	AVChannelLayout ch_layout;
	BufferData buffer_data; // Used for res://, user:// and memory buffer
	PackedByteArray file_buffer;

	bool loaded = false;
	bool stereo = true;
	int bytes_per_sample = 0;
	int sample_rate = 44100;
	double length = 0;
	bool use_icy = false;
	String icy_metadata;
	String icy_packet;

	Dictionary icy_headers_cache;
	Dictionary stream_title_cache;

	Mutex *mutex; // We need thread safety

	String file_path;
	bool from_memory_buffer = false; // Flag to indicate if loaded from memory buffer

	static inline void _log(String message) { UtilityFunctions::print("GoZenAudioStream: ", message, "."); }
	static inline bool _log_err(String message) {
		UtilityFunctions::printerr("GoZenAudioStream: ", message, "!");
		return false;
	}

  public:
	AudioStreamFFmpeg() = default;
	~AudioStreamFFmpeg();

	int open(const String& path, int stream_index = -1);
	int load_from_buffer(const PackedByteArray &data, int stream_index = -1);
	void close();
	inline bool is_open() const { return loaded; }

	double _get_length() const override { return length; }
	bool _is_monophonic() const override { return !stereo; }

	Ref<AudioStreamPlayback> _instantiate_playback() const override;
	void set_use_icy(bool value) { use_icy = value; }
	bool get_use_icy() const { return use_icy; }
	Dictionary get_icy_headers();
	String get_stream_title();
	Dictionary get_tags();

  protected:
	static void _bind_methods();
	friend class AudioStreamFFmpegPlayback;
	
  private:
	int _open_from_memory(const PackedByteArray &data, int stream_index);
};