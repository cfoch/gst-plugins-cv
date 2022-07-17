/*
 * GStreamer Plugins CV
 * Copyright (C) 2019 Cesar Fabian Orccon Chipana <cfoch.fabian@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "gstcvdraw.h"
#include "../utils/gstcvobjectcache.h"

struct _GstCVDrawPrivate
{
  GstPad *sinkpad;
  GstElement *peer_element;
  gdouble unscale_factor;
  cv::Mat *img;
  GstStructure *sub_key;
  GstStructure *params;
  gboolean on_peer;
  gchar *on_cache_id;
  guint64 last_n_frames;

  guint64 n_frame;
};

GST_DEBUG_CATEGORY_STATIC (gst_cv_draw_debug);
#define GST_CAT_DEFAULT gst_cv_draw_debug

/* Default property values */
#define DEFAULT_COLOR               cv::Scalar (0, 255, 0)
#define DEFAULT_CACHE_COLOR         cv::Scalar (255, 0, 0)
#define DEFAULT_ON_PEER             FALSE
#define DEFAULT_ON_CACHE_ID         NULL
#define DEFAULT_LAST_N_FRAMES       10

enum
{
  PROP_0,
  PROP_SUB_KEY,
  PROP_PARAMS,
  PROP_ON_PEER,
  PROP_ON_CACHE_ID,
  PROP_LAST_N_FRAMES,
};

#define parent_class gst_cv_draw_parent_class
G_DEFINE_TYPE_WITH_PRIVATE (GstCVDraw, gst_cv_draw,
    GST_TYPE_OPENCV_VIDEO_FILTER);

static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK, GST_PAD_ALWAYS, GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE (
        "RGB")));

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC, GST_PAD_ALWAYS, GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE ("RGB")));

static void gst_cv_draw_set_property (GObject *object, guint prop_id, const
    GValue *value, GParamSpec *pspec);
static void gst_cv_draw_get_property (GObject *object, guint prop_id,
    GValue *value, GParamSpec *pspec);

static GstFlowReturn gst_cv_draw_transform_ip (GstOpencvVideoFilter *self,
    GstBuffer *buf, cv::Mat img);

static void
gst_cv_draw_finalize (GObject *obj)
{
  GstCVDraw *self = GST_CV_DRAW (obj);

  if (self->priv->sub_key)
    gst_structure_free (self->priv->sub_key);

  if (self->priv->params)
    gst_structure_free (self->priv->params);

  if (self->priv->on_cache_id)
    g_free (self->priv->on_cache_id);

  G_OBJECT_CLASS (parent_class)->finalize (obj);
}

static void
gst_cv_draw_class_init (GstCVDrawClass *klass)
{
  GObjectClass *gobject_class;
  GstOpencvVideoFilterClass *gstopencvbasefilter_class;
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

  GST_DEBUG_CATEGORY_INIT (gst_cv_draw_debug, "cvdraw", 0, "cvdraw element");

  gobject_class = (GObjectClass *) klass;
  gobject_class->finalize = GST_DEBUG_FUNCPTR (gst_cv_draw_finalize);
  gstopencvbasefilter_class = (GstOpencvVideoFilterClass *) klass;

  gstopencvbasefilter_class->cv_trans_ip_func = gst_cv_draw_transform_ip;

  gobject_class->set_property = gst_cv_draw_set_property;
  gobject_class->get_property = gst_cv_draw_get_property;

  g_object_class_install_property (gobject_class, PROP_SUB_KEY,
      g_param_spec_boxed ("sub-key", "Sub-key", "A structured key useful to "
      "look up the output object info.", GST_TYPE_STRUCTURE,
      (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

  g_object_class_install_property (gobject_class, PROP_PARAMS,
      g_param_spec_boxed ("params", "Parameters", "A structure to draw "
      "info from parameters contained in the object info. The structure "
      "should have the following format: 'cv,params=<param1,param2,...>' "
      "where param1, param2... should be strings.", GST_TYPE_STRUCTURE,
      (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

  g_object_class_install_property (gobject_class, PROP_ON_PEER,
      g_param_spec_boolean ("on-peer", "On peer element",
      "Extends the 'params' "
      "property with element=<peer_element_name>. This is useful to filter "
      "objects info by the peer element name.", DEFAULT_ON_PEER,
      (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

  g_object_class_install_property (gobject_class, PROP_ON_CACHE_ID,
      g_param_spec_string ("on-cache-id", "On cache id", "Restrict drawing "
      "taking info from the cache with the given id.", DEFAULT_ON_CACHE_ID,
      (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

  g_object_class_install_property (gobject_class, PROP_LAST_N_FRAMES,
      g_param_spec_uint64 ("last-n-frames", "Last N Frames",
      "Draw the last n-frames.", 0, G_MAXUINT64, DEFAULT_LAST_N_FRAMES,
      (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

  gst_element_class_set_static_metadata (element_class, "cvdraw",
      "Filter/Effect/Video",
      "Draws the info contained in the GstObjectInfoMapMeta structure.",
      "Cesar Fabian Orccon Chipana <cfoch.fabian@gmail.com>");

  gst_element_class_add_static_pad_template (element_class, &src_factory);
  gst_element_class_add_static_pad_template (element_class, &sink_factory);
}

static void
gst_cv_draw_init (GstCVDraw *self)
{
  self->priv = (GstCVDrawPrivate *) gst_cv_draw_get_instance_private (self);
  self->priv->sinkpad = gst_element_get_static_pad (GST_ELEMENT (self), "sink");
  self->priv->peer_element = NULL;
  self->priv->on_peer = DEFAULT_ON_PEER;
  self->priv->on_cache_id = DEFAULT_ON_CACHE_ID;
  self->priv->last_n_frames = DEFAULT_LAST_N_FRAMES;

  self->priv->n_frame = 0;

  gst_opencv_video_filter_set_in_place (GST_OPENCV_VIDEO_FILTER_CAST (self),
      TRUE);
}

static void
gst_cv_draw_set_property (GObject *object, guint prop_id, const GValue *value,
    GParamSpec *pspec)
{
  GstCVDraw *self = GST_CV_DRAW (object);

  switch (prop_id) {
    case PROP_SUB_KEY:
      if (self->priv->sub_key)
        gst_structure_free (self->priv->sub_key);
      self->priv->sub_key = GST_STRUCTURE (g_value_dup_boxed (value));
      break;
    case PROP_PARAMS:
      if (self->priv->params)
        gst_structure_free (self->priv->params);
      self->priv->params = GST_STRUCTURE (g_value_dup_boxed (value));
      break;
    case PROP_ON_PEER:
      self->priv->on_peer = g_value_get_boolean (value);
      break;
    case PROP_ON_CACHE_ID:
      self->priv->on_cache_id = g_value_dup_string (value);
      break;
    case PROP_LAST_N_FRAMES:
      self->priv->last_n_frames = g_value_get_uint64 (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_cv_draw_get_property (GObject *object, guint prop_id, GValue *value,
    GParamSpec *pspec)
{
  GstCVDraw *self = GST_CV_DRAW (object);

  switch (prop_id) {
    case PROP_SUB_KEY:
      g_value_set_boxed (value, self->priv->sub_key);
      break;
    case PROP_PARAMS:
      g_value_set_boxed (value, self->priv->params);
      break;
    case PROP_ON_PEER:
      g_value_set_boolean (value, self->priv->on_peer);
      break;
    case PROP_ON_CACHE_ID:
      g_value_set_string (value, self->priv->on_cache_id);
      break;
    case PROP_LAST_N_FRAMES:
      g_value_set_uint64 (value, self->priv->last_n_frames);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static cv::Scalar
gst_cv_draw_get_default_color (GstCVDraw *self)
{
  return !self->priv->on_cache_id ? DEFAULT_COLOR : DEFAULT_CACHE_COLOR;
}

static void
gst_cv_draw_draw_object_info (GstCVDraw *self, GstCVObjectInfo *info)
{
  gst_cv_object_info_draw (info, self->priv->unscale_factor, *self->priv->img,
      gst_cv_draw_get_default_color (self));
}

static void
gst_cv_draw_draw_id (GstCVDraw *self, GstCVObjectInfo *reference_object_info,
    guint index)
{
  graphene_point_t scaled_centroid;
  gdouble unscale_factor = self->priv->unscale_factor;

  gst_cv_object_info_get_centroid (reference_object_info, &scaled_centroid);
  if (GST_CV_OBJECT_INFO_CENTROID_IS_UNDEFINED (scaled_centroid))
    return;

  cv::Point unscaled_centroid = cv::Point (scaled_centroid.x,
      scaled_centroid.y) * unscale_factor;
  cv::putText (*self->priv->img, std::to_string (index), unscaled_centroid,
      cv::FONT_HERSHEY_SIMPLEX, 1.0, gst_cv_draw_get_default_color (self));
}

static void
_draw (GstStructure *key, GstCVObjectInfo *value, GstCVDraw *self)
{
  GValueArray *params_value;
  GstStructure *object_info_params;

  gst_cv_draw_draw_object_info (self, value);

  object_info_params = gst_cv_object_info_get_params (value);
  /* Draw params. */
  if (self->priv->params && object_info_params && gst_structure_get_array (
      self->priv->params, "params", &params_value)) {
    gchar *str;
    guint i;

    for (i = 0; i < params_value->n_values; i++) {
      GValue *param_val;
      const gchar *field_name;

      param_val = g_value_array_get_nth (params_value, i);

      if (G_VALUE_TYPE (param_val) != G_TYPE_STRING) {
        str = gst_structure_to_string (key);
        GST_WARNING_OBJECT (self, "Skipping param at index=%d at object with "
            "key='%s'. Param is not a string.", i, str);
        g_free (str);
        continue;
      }

      field_name = g_value_get_string (param_val);
      if (!gst_structure_has_field (object_info_params, field_name)) {
        str = gst_structure_to_string (key);
        GST_WARNING_OBJECT (self, "Skipping param at index=%d at object with "
            "key='%s'. Object does not contain such param.", i, str);
        g_free (str);
        continue;
      }

      /* TODO: Support other type of params. Only index param support now. */
      if (g_strcmp0 (GST_CV_OBJECT_INFO_SUB_PARAM_INDEX, field_name) == 0) {
        guint index;
        if (gst_structure_get_uint (object_info_params,
            GST_CV_OBJECT_INFO_SUB_PARAM_INDEX, &index))
          gst_cv_draw_draw_id (self, value, index);
      }
    }
  }
}

static void
_get_scaled_frame_and_unscale_factor (GstBuffer *buf, cv::Mat input_frame,
    cv::Mat &scaled_frame, gdouble *unscale_factor)
{
  GstCVDupScaleMeta *meta;

  meta = (GstCVDupScaleMeta *) gst_buffer_get_meta (buf,
      GST_CV_DUP_SCALE_META_API_TYPE);

  if (meta) {
    scaled_frame = gst_cv_dup_scale_meta_get_scaled_frame (meta);
    *unscale_factor = gst_cv_dup_scale_meta_get_unscale_factor (meta);
  } else {
    scaled_frame = input_frame;
    *unscale_factor = 1.0;
  }
}

static void
gst_cv_draw_map (GstCVDraw *self, GstCVObjectInfoMap *map,
    GstCVObjectInfoMapFunc func)
{
  if (self->priv->sub_key) {
    gst_cv_object_info_map_foreach_with_sub_key (map, self->priv->sub_key,
        (GstCVObjectInfoMapFunc) _draw, self);
  } else {
    gst_cv_object_info_map_foreach (map, (GstCVObjectInfoMapFunc) _draw, self);
  }
}

static void
_foreach_cache_func (guint64 key, GstCVObjectInfoMap *value, gpointer user_data)
{
  GstCVDraw *self = GST_CV_DRAW (user_data);
  gst_cv_draw_map (self, value, (GstCVObjectInfoMapFunc) _draw);
}

static GstFlowReturn
_cv_draw_transform_ip (GstOpencvVideoFilter *base, GstBuffer *buf, cv::Mat img)
{
  GstCVDraw *self = GST_CV_DRAW (base);
  GstCVObjectInfoMapMeta *meta;
  GstCVObjectInfoMap *map;
  cv::Mat scaled_frame;

  self->priv->img = NULL;

  if (self->priv->on_peer && !self->priv->peer_element) {
    GstPad *peerpad;
    gchar *peer_element_name;

    peerpad = gst_pad_get_peer (self->priv->sinkpad);
    if (!peerpad)
      return GST_FLOW_ERROR;
    self->priv->peer_element = GST_ELEMENT (gst_pad_get_parent (peerpad));
    if (!self->priv->peer_element)
      return GST_FLOW_ERROR;

    if (!self->priv->sub_key)
      self->priv->sub_key = gst_structure_new_empty ("cv");

    peer_element_name = gst_element_get_name (self->priv->peer_element);
    gst_structure_set (self->priv->sub_key, GST_CV_OBJECT_INFO_SUB_KEY_ELEMENT,
        GST_TYPE_CV_OBJECT_INFO_SUB_KEY_ELEMENT, peer_element_name, NULL);

    g_free (peer_element_name);
  }

  self->priv->img = &img;
  _get_scaled_frame_and_unscale_factor (buf, img, scaled_frame,
      &self->priv->unscale_factor);

  if (!self->priv->on_cache_id) {
    meta = (GstCVObjectInfoMapMeta *) (gst_buffer_get_meta (buf,
        GST_CV_OBJECT_INFO_MAP_META_API_TYPE));
    if (!meta) {
      GST_WARNING_OBJECT (self,
          "Nothing to do. GstCVObjectInfoMapMeta was not found.");
      return GST_FLOW_OK;
    }

    map = gst_cv_object_info_map_meta_get_object_info_map (meta);
    gst_cv_draw_map (self, map, (GstCVObjectInfoMapFunc) _draw);
  } else {
    GstCVObjectInfoMapNamedCache *named_cache;
    GstCVObjectInfoMapCache *cache;

    named_cache = gst_cv_object_cache_get_default_named_cache ();
    cache = gst_cv_object_info_map_named_cache_lookup (named_cache,
        self->priv->on_cache_id);

    if (cache)
      gst_cv_object_info_map_cache_foreach (cache, _foreach_cache_func, self);
  }

  return GST_FLOW_OK;
}

static GstFlowReturn
gst_cv_draw_transform_ip (GstOpencvVideoFilter *base, GstBuffer *buf, cv::Mat
    img)
{
  GstCVDraw *self = GST_CV_DRAW (base);

  self->priv->n_frame++;
  return _cv_draw_transform_ip (base, buf, img);
}
