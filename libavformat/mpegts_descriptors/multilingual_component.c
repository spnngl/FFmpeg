#include "libavutil/mem.h"
#include "../mpegts.h"
#include "multilingual_component.h"
#include "common.h"

static int multilingual_component_parse(MpegTSDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    MpegTSMultilingualComponentDescriptor *mcd;

    if (desc->priv)
        desc->free(desc);

    desc->priv = av_mallocz(sizeof(MpegTSMultilingualComponentDescriptor));
    mcd = desc->priv;

    val = mpegts_get8(&p, p_end);
    if (val < 0)
        return val;
    mcd->component_tag = val;

    while(p < p_end) {
        SimpleLinkedList *cur;
        MpegTSMultilingualComponentDescription *comp =
            av_mallocz(sizeof(MpegTSMultilingualComponentDescription));

        memcpy(comp->iso_639_language_code, p, 3);
        p += 3;

        val = mpegts_get8(&p, p_end);
        if (val < 0)
            return val;
        comp->text_description_len = val;

        comp->text = av_malloc(comp->text_description_len + 1);
        memcpy(comp->text, p, comp->text_description_len);
        comp->text[comp->text_description_len] = '\0';
        p += comp->text_description_len;

        cur = av_mallocz(sizeof(SimpleLinkedList));
        cur->data = comp;
        cur->next = mcd->descriptions;
        mcd->descriptions = cur;
    }

    return 0;
}

static int multilingual_component_free(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        MpegTSMultilingualComponentDescriptor *mcd = desc->priv;
        SimpleLinkedList *head = mcd->descriptions, *next;
        while (head) {
            MpegTSMultilingualComponentDescription *d = head->data;

            next = head->next;
            av_free(d->text);
            av_free(d);
            av_free(head);
            head = next;
        }
        av_free(mcd);
        desc->priv = NULL;
    }
    return 0;
}

static void multilingual_component_debug(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        int count = 0;
        MpegTSMultilingualComponentDescriptor *mcd = desc->priv;
        SimpleLinkedList *head = mcd->descriptions;

        av_log(NULL, MPEGTS_DESCRIPTORS_LOG, "Multi-lingual component descriptor: "
               "component_tag(%i)\n", mcd->component_tag);

        while (head) {
            MpegTSMultilingualComponentDescription *d = head->data;
            av_log(NULL, MPEGTS_DESCRIPTORS_LOG, "Multi-lingual description #%i: "
                   "iso_639_language_code(%.3s), text(%s)\n", count++,
                   d->iso_639_language_code, d->text);
            head = head->next;
        }
    }
}

MpegTSDescriptor multilingual_component_descriptor = {
    .priv = NULL,
    .parse = multilingual_component_parse,
    .free = multilingual_component_free,
    .debug = multilingual_component_debug
};
