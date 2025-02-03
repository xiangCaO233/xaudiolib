#include "decoder.h"

#include <vector>

#include "config/config.h"
#include "log/colorful-log.h"

XAudioDecoder::XAudioDecoder(AVCodecID id) : decoder_id_name(id) {
  decoder = avcodec_find_decoder(id);
  if (!decoder)
    XERROR("创建解码器失败");
  else {
    XINFO("成功创建解码器:[" + std::string(avcodec_get_name(id)) + "]");
    decoder_context = avcodec_alloc_context3(decoder);
  }
}

XAudioDecoder::~XAudioDecoder() {
  XTRACE("销毁解码器[" + std::string(avcodec_get_name(decoder_id_name)) + "]");
}

int XAudioDecoder::decode_audio(AVFormatContext *format, int streamIndex,
                                std::vector<float> &pcm_data) {
  av_log_set_level(AV_LOG_ERROR);
  // 填充解码器上下文参数
  avcodec_parameters_to_context(decoder_context,
                                format->streams[streamIndex]->codecpar);
  // 打开解码器上下文
  avcodec_open2(decoder_context, decoder, nullptr);
  auto packet = av_packet_alloc();
  auto frame = av_frame_alloc();
  AVChannelLayout out_channel_layout;
  av_channel_layout_default(&out_channel_layout,
                            static_cast<int>(Config::channel));
  auto out_sample_rate = static_cast<int>(Config::samplerate);
  enum AVSampleFormat out_sample_format = AV_SAMPLE_FMT_FLT;

  int sampler_allocat_ret = swr_alloc_set_opts2(
      &resampler, &out_channel_layout, out_sample_format, out_sample_rate,
      &decoder_context->ch_layout, decoder_context->sample_fmt,
      decoder_context->sample_rate, 0, nullptr);
  if (sampler_allocat_ret < 0 || !resampler || swr_init(resampler) < 0) {
    XCRITICAL("重采样器初始化失败");
    return -1;
  }

  while (av_read_frame(format, packet) >= 0) {
    if (packet->stream_index == streamIndex) {
      if (avcodec_send_packet(decoder_context, packet) < 0) {
        XCRITICAL("发送包到解码器时出现问题");
        return -1;
      }
      while (avcodec_receive_frame(decoder_context, frame) == 0) {
        uint8_t **out_buffer = nullptr;
        int out_linesize = 0;
        auto out_samples = av_rescale_rnd(
            swr_get_delay(resampler, out_sample_rate) + frame->nb_samples,
            out_sample_rate, decoder_context->sample_rate, AV_ROUND_UP);
        if (av_samples_alloc_array_and_samples(
                &out_buffer, &out_linesize, static_cast<int>(Config::channel),
                (int)out_samples, out_sample_format, 0) < 0) {
          XCRITICAL("分配输出采样数组时出现问题");
          return -1;
        }
        int converted_samples =
            swr_convert(resampler, out_buffer, (int)out_samples,
                        (const uint8_t **)frame->data, frame->nb_samples);
        if (converted_samples < 0) {
          XCRITICAL("重采样时出现问题");
          av_freep(&out_buffer[0]);
          av_freep(&out_buffer);
          return -1;
        }

        auto buffer_ptr = reinterpret_cast<float *>(out_buffer[0]);
        pcm_data.insert(
            pcm_data.end(), buffer_ptr,
            buffer_ptr + converted_samples * static_cast<int>(Config::channel));

        av_freep(&out_buffer[0]);
        av_freep(&out_buffer);
      }
    }
    av_packet_unref(packet);
  }

  av_packet_free(&packet);
  av_frame_free(&frame);
  return 0;
}
