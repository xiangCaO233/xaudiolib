#include "decoder.h"

#include <memory>
#include <vector>

#include "logger/logger.h"

XAudioDecoder::XAudioDecoder(AVCodecID id) {
    decoder = avcodec_find_decoder(id);
    if (!decoder)
        LOG_ERROR("创建解码器失败");
    else {
        LOG_INFO("成功创建解码器:[" + std::string(avcodec_get_name(id)) + "]");
        decoder_context = avcodec_alloc_context3(decoder);
    }
}

XAudioDecoder::~XAudioDecoder() {}

int XAudioDecoder::decode_audio(const std::shared_ptr<AVFormatContext> &format,
                                int streamIndex, std::vector<float> &pcm_data) {
    av_log_set_level(AV_LOG_ERROR);  // 只打印错误信息
    // 填充解码器上下文参数
    avcodec_parameters_to_context(decoder_context,
                                  format->streams[streamIndex]->codecpar);
    // 打开解码器上下文
    avcodec_open2(decoder_context, decoder, nullptr);
    auto packet = av_packet_alloc();
    auto frame = av_frame_alloc();
    // 输出通道布局
    const auto out_channel_layout = decoder_context->ch_layout;
    // 输出采样率
    int out_sample_rate = decoder_context->sample_rate;
    // 输出样本采样率(32bit)
    enum AVSampleFormat out_sample_format = AV_SAMPLE_FMT_FLTP;

    // 分配重采样器
    int sampler_allocat_ret = swr_alloc_set_opts2(
        &resampler, &out_channel_layout, out_sample_format, out_sample_rate,
        &out_channel_layout, decoder_context->sample_fmt,
        decoder_context->sample_rate, 0, nullptr);
    if (sampler_allocat_ret < 0 || !decoder_context ||
        swr_init(resampler) < 0) {
        LOG_ERROR("重采样器初始化失败");
        return -1;
    }
    while (av_read_frame(format.get(), packet) >= 0) {
        // 检查包是否属于音频流
        if (packet->stream_index == streamIndex) {
            // 发送包到解码器
            if (int ret = avcodec_send_packet(decoder_context, packet) < 0) {
                LOG_ERROR("发送包到解码器时出现问题");
                return -1;
            }
            // 接收解码后的音频帧
            while (avcodec_receive_frame(decoder_context, frame) == 0) {
                uint8_t **out_buffer = nullptr;
                int out_linesize = 0;
                int out_samples = av_rescale_rnd(
                    swr_get_delay(resampler, decoder_context->sample_rate) +
                        frame->nb_samples,
                    out_sample_rate, decoder_context->sample_rate, AV_ROUND_UP);
                if (av_samples_alloc_array_and_samples(
                        &out_buffer, &out_linesize,
                        decoder_context->ch_layout.nb_channels, out_samples,
                        out_sample_format, 0) < 0) {
                    LOG_ERROR("分配输出采样数组时出现问题");
                    return -1;
                }
                // 执行重采样
                int converted_samples = swr_convert(
                    resampler, out_buffer, out_samples,
                    (const uint8_t **)frame->data, frame->nb_samples);
                if (converted_samples < 0) {
                    av_freep(&out_buffer[0]);
                    av_freep(&out_buffer);
                    LOG_ERROR("重采样时出现问题");
                    return -1;
                }
                // 将浮点数据导入 pcm_data
                float *buffer_ptr = reinterpret_cast<float *>(out_buffer[0]);
                pcm_data.insert(
                    pcm_data.end(), buffer_ptr,
                    buffer_ptr + converted_samples *
                                     decoder_context->ch_layout.nb_channels);

                // 释放缓冲区
                av_freep(&out_buffer[0]);
                av_freep(&out_buffer);
            }
        }
        av_packet_unref(packet);
    }
    return 0;
}
