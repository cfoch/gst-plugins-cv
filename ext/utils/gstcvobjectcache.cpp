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

#include "gstcvobjectcache.h"

static GstCVObjectInfoMapNamedCache *_NAMED_CACHE = NULL;

struct _GstCVObjectCachePrivate
{
  GstStructure *sub_key;
  GstStructure *params;
  gboolean on_peer;
  guint max_cache_size;

  guint64 n_frame;
};

GST_DEBUG_CATEGORY_STATIC (gst_cv_object_cache_debug);
#define GST_CAT_DEFAULT gst_cv_object_cache_debug

#define DEFAULT_MAX_CACHE_SIZE      30

/* Default property values */
#define DEFAULT_ON_PEER             FALSE

enum
{
  PROP_0,
  PROP_SUB_KEY,
  PROP_ON_PEER,
  PROP_MAX_CACHE_SIZE,
};

#define parent_class gst_cv_object_cache_parent_class
G_DEFINE_TYPE_WITH_PRIVATE (GstCVObjectCache, gst_cv_object_cache,
    GST_TYPE_BASE_TRANSFORM);

static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK, GST_PAD_ALWAYS, GST_STATIC_CAPS_ANY);

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC, GST_PAD_ALWAYS, GST_STATIC_CAPS_ANY);

static void gst_cv_object_cache_set_property (GObject *object, guint prop_id,
    const GValue *value, GParamSpec *pspec);
static void gst_cv_object_cache_get_property (GObject *object, guint prop_id,
    GValue *value, GParamSpec *pspec);

static GstFlowReturn gst_cv_object_cache_transform_ip (GstBaseTransform *self,
    GstBuffer *buf);

static void
gst_cv_object_cache_finalize (GObject *obj)
{
  GstCVObjectCache *self = GST_CV_OBJECT_CACHE (obj);

  if (self->priv->sub_key)
    gst_structure_free (self->priv->sub_key);

  if (self->priv->params)
    gst_structure_free (self->priv->params);

  /* TODO: Mutex here. */
  if (_NAMED_CACHE) {
    gint prev_cache_refcount = GST_MINI_OBJECT_REFCOUNT_VALUE (_NAMED_CACHE);

    gst_mini_object_unref (GST_MINI_OBJECT_CAST (_NAMED_CACHE));
    if (prev_cache_refcount == 1)
      _NAMED_CACHE = NULL;
  }

  G_OBJECT_CLASS (parent_class)->finalize (obj);
}

static void
gst_cv_object_cache_class_init (GstCVObjectCacheClass *klass)
{
  GObjectClass *gobject_class;
  GstBaseTransformClass *base_trans_class = GST_BASE_TRANSFORM_CLASS (klass);
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

  GST_DEBUG_CATEGORY_INIT (gst_cv_object_cache_debug, "cvobjectcache", 0,
      "cvobjectcache element");

  gobject_class = (GObjectClass *) klass;
  gobject_class->finalize = GST_DEBUG_FUNCPTR (gst_cv_object_cache_finalize);
  gobject_class->set_property = gst_cv_object_cache_set_property;
  gobject_class->get_property = gst_cv_object_cache_get_property;

  g_object_class_install_property (gobject_class, PROP_SUB_KEY,
      g_param_spec_boxed ("sub-key", "Sub-key", "A structured key useful to "
      "look up the output object info.", GST_TYPE_STRUCTURE,
      (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

  g_object_class_install_property (gobject_class, PROP_MAX_CACHE_SIZE,
      g_param_spec_uint ("max-cache-size", "Max cache size",
      "Maximum cache size", 0, G_MAXUINT, DEFAULT_MAX_CACHE_SIZE,
      (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS |
        G_PARAM_CONSTRUCT)));

  base_trans_class->transform_ip = gst_cv_object_cache_transform_ip;

  gst_element_class_set_static_metadata (element_class, "cvobjectcache",
      "Filter/Effect/Video", "Caches the GstCVObjectInfoMap by frame number.",
      "Cesar Fabian Orccon Chipana <cfoch.fabian@gmail.com>");

  gst_element_class_add_static_pad_template (element_class, &src_factory);
  gst_element_class_add_static_pad_template (element_class, &sink_factory);
}

GstCVObjectInfoMapNamedCache *
gst_cv_object_cache_get_default_named_cache ()
{
  if (_NAMED_CACHE == NULL)
    _NAMED_CACHE = gst_cv_object_info_map_named_cache_new ();

  return _NAMED_CACHE;
}

static void
gst_cv_object_cache_init (GstCVObjectCache *self)
{
  self->priv =
      (GstCVObjectCachePrivate *) gst_cv_object_cache_get_instance_private (
      self);
  self->priv->on_peer = DEFAULT_ON_PEER;
  self->priv->n_frame = 0;
}

static void
gst_cv_object_cache_set_property (GObject *object, guint prop_id, const
    GValue *value, GParamSpec *pspec)
{
  GstCVObjectCache *self = GST_CV_OBJECT_CACHE (object);

  switch (prop_id) {
    case PROP_SUB_KEY:
      if (self->priv->sub_key)
        gst_structure_free (self->priv->sub_key);
      self->priv->sub_key = GST_STRUCTURE (g_value_dup_boxed (value));
      break;
    case PROP_ON_PEER:
      self->priv->on_peer = g_value_get_boolean (value);
      break;
    case PROP_MAX_CACHE_SIZE:
      self->priv->max_cache_size = g_value_get_uint (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_cv_object_cache_get_property (GObject *object, guint prop_id, GValue *value,
    GParamSpec *pspec)
{
  GstCVObjectCache *self = GST_CV_OBJECT_CACHE (object);

  switch (prop_id) {
    case PROP_SUB_KEY:
      g_value_set_boxed (value, self->priv->sub_key);
      break;
    case PROP_ON_PEER:
      g_value_set_boolean (value, self->priv->on_peer);
      break;
    case PROP_MAX_CACHE_SIZE:
      g_value_set_uint (value, self->priv->max_cache_size);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static GstFlowReturn
gst_cv_object_cache_transform_ip (GstBaseTransform *base, GstBuffer *buf)
{
  GstCVObjectCache *self = GST_CV_OBJECT_CACHE (base);
  GstCVObjectInfoMapMeta *meta;
  GstCVObjectInfoMap *map;
  GstCVObjectInfoMapNamedCache *named_cache;
  GstCVObjectInfoMapCache *cache;
  gchar *cache_id;

  meta = (GstCVObjectInfoMapMeta *) (gst_buffer_get_meta (buf,
      GST_CV_OBJECT_INFO_MAP_META_API_TYPE));
  if (!meta) {
    (self, "Nothing to do. GstCVObjectInfoMapMeta was not found.");
    self->priv->n_frame++;
    return GST_FLOW_OK;
  }

  named_cache = gst_cv_object_cache_get_default_named_cache ();
  cache_id = gst_element_get_name (GST_ELEMENT (self));
  map = gst_cv_object_info_map_meta_get_object_info_map (meta);

  if (!(cache = gst_cv_object_info_map_named_cache_lookup (named_cache,
      cache_id))) {
    cache = gst_cv_object_info_map_cache_new (self->priv->max_cache_size);
    gst_cv_object_info_map_named_cache_insert (named_cache, cache_id, cache);
  }
  /* TODO: Mutex here? */
  gst_cv_object_info_map_cache_extend_value_with_sub_key (cache,
      self->priv->n_frame, map, self->priv->sub_key);

  self->priv->n_frame++;
  return GST_FLOW_OK;
}
