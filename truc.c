#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/dvb.h>
#include <signal.h>
#include <unistd.h>

/** To compile:
 *
 * gcc $(pkg-config --cflags libavformat libavcodec libswscale libavutil libavfilter libswresample) -O0 -ggdb3 -DDEBUG=0 -I/usr/local/include -I/usr/include -c truc.c -o truc.o
 * gcc truc.o -o truc $(pkg-config --libs libavformat libavcodec libswscale libavutil libavfilter libswresample)
 */

static AVCodecContext *dec_ctxs[10];
static int g_quit = 0;

static void sig_term(int signum) {
    g_quit = 1;
}

int open_codec_context(AVFormatContext *fmt_ctx, int index)
{
    AVCodec *dec;
    AVCodecContext *dec_ctx;
    AVStream *st = fmt_ctx->streams[index];

    if (!(dec = avcodec_find_decoder(st->codecpar->codec_id)))
        exit(1);

    if (!(dec_ctx = avcodec_alloc_context3(dec)))
        exit(1);

    if(avcodec_parameters_to_context(dec_ctx, st->codecpar) < 0)
        exit(1);

    dec_ctx->pkt_timebase = st->time_base;
    dec_ctx->framerate = st->avg_frame_rate;

    if (avcodec_open2(dec_ctx, dec, NULL) < 0)
        exit(1);

    dec_ctxs[index] = dec_ctx;
    return 0;
}

int main(int argc, char **argv, char **envp)
{
    char *path = argv[1];
    char error[128];
    AVDictionary *dict = NULL;
    AVFormatContext *fmt_ctx;
    AVPacket pkt;
    AVFrame *frame;
    int ret;

    signal(SIGTERM, sig_term);
    signal(SIGINT, sig_term);

    memset(dec_ctxs, 0, sizeof(dec_ctxs));

    //av_dict_set(&dict, "probesize", "5000000", 0);
    //av_dict_set(&dict, "analyzeduration", "10000000", 0);

    // Open format
    if (!(fmt_ctx = avformat_alloc_context()))
        exit(1);

    if (avformat_open_input(&fmt_ctx, path, 0, &dict) < 0)
        exit(1);

    if (avformat_find_stream_info(fmt_ctx, 0) < 0)
        exit(1);

    av_format_inject_global_side_data(fmt_ctx);
    av_dump_format(fmt_ctx, 0, path, 0);

    // Read
    frame = av_frame_alloc();
    do {
        AVStream *st;
        AVCodecContext *dec_ctx;

        ret = av_read_frame(fmt_ctx, &pkt);
        if (ret < 0) {
            av_strerror(ret, error, 128);
            av_log(NULL, AV_LOG_WARNING, "av_read_frame() error: %s\n", error);
            goto free_pkt;
        }

        st = fmt_ctx->streams[pkt.stream_index];
        dec_ctx = dec_ctxs[pkt.stream_index];
        if (!dec_ctx) {
            open_codec_context(fmt_ctx, pkt.stream_index);
            dec_ctx = dec_ctxs[pkt.stream_index];
        }

        if (st->codecpar->codec_id == AV_CODEC_ID_DVB_SUBTITLE)
            goto free_pkt;

        if (st->codecpar->codec_id != AV_CODEC_ID_EPG)
            goto free_pkt;

        ret = avcodec_send_packet(dec_ctx, &pkt);
        switch (ret) {
            case 0:
                break;
            case AVERROR(EINVAL):
                usleep(10000);
            default:
                goto free_pkt;
        }

        while (1) {
            ret = avcodec_receive_frame(dec_ctx, frame);
            if (ret < 0)
                break;

            for (int i = 0; i < frame->nb_side_data; i++) {
                AVFrameSideData *sd = frame->side_data[i];
                if (sd && sd->type == AV_FRAME_DATA_EPG_TABLE) {
                    EPGTable *table = sd->data;
                    av_epg_show_table(table, AV_LOG_WARNING);
                }
            }
            av_frame_unref(frame);
        }

free_pkt:
        av_packet_unref(&pkt);
    } while (!g_quit); //(ret >= 0 || ret == AVERROR(EAGAIN));
}
