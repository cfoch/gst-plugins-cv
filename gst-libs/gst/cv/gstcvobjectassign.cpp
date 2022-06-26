/*
 * GStreamer Plugins CV
 * Copyright (C) 2021 Cesar Fabian Orccon Chipana <cfoch.fabian@gmail.com>
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

#include "gstcvobjectassign.h"
#include "gst/cv/object/info/objectinfomapmeta.h"
#include "object/info/objectinfoutils.h"

static void gst_cv_object_assign_set_property (GObject *object, guint prop_id,
    const GValue *value, GParamSpec *pspec);

static void gst_cv_object_assign_get_property (GObject *object, guint prop_id,
    GValue *value, GParamSpec *pspec);

static double gst_cv_object_assign_cost (GstCVObjectAssign *self,
    graphene_point_t current_centroid, graphene_point_t previous_centroid);

struct _GstCVObjectAssignPrivate
{
  guint64 frame_number;
  GstStructure *sub_key;
  gchar *on_cache_id;
  guint on_last_n_frames;
};

struct _GstCVObjectAssignContext
{
  gint index;
  cv::Mat img;
  GstBuffer *buf;
};

GST_DEBUG_CATEGORY_STATIC (gst_cv_object_assign_debug);
#define GST_CAT_DEFAULT gst_cv_object_assign_debug

/* Default property values */
#define DEFAULT_ON_CACHE_ID         NULL
#define DEFAULT_ON_LAST_N_FRAMES    1

enum
{
  PROP_0,
  PROP_SUB_KEY,
  PROP_ON_CACHE_ID,
  PROP_ON_LAST_N_FRAMES
};

#define parent_class gst_cv_object_assign_parent_class
G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (GstCVObjectAssign, gst_cv_object_assign,
    GST_TYPE_OPENCV_VIDEO_FILTER);

static GstFlowReturn gst_cv_object_assign_transform_ip (
    GstOpencvVideoFilter *self, GstBuffer *buf, cv::Mat img);

static void
gst_cv_object_assign_finalize (GObject *obj)
{
  G_OBJECT_CLASS (parent_class)->finalize (obj);
}

static void
gst_cv_object_assign_class_init (GstCVObjectAssignClass *klass)
{
  GObjectClass *gobject_class;
  GstOpencvVideoFilterClass *gstopencvbasefilter_class;

  GST_DEBUG_CATEGORY_INIT (gst_cv_object_assign_debug, "cvobjectassign", 0,
      "cvobjectassign element");

  gobject_class = (GObjectClass *) klass;
  gobject_class->finalize = GST_DEBUG_FUNCPTR (gst_cv_object_assign_finalize);
  gstopencvbasefilter_class = (GstOpencvVideoFilterClass *) klass;

  gstopencvbasefilter_class->cv_trans_ip_func =
      gst_cv_object_assign_transform_ip;

  gobject_class->set_property = gst_cv_object_assign_set_property;
  gobject_class->get_property = gst_cv_object_assign_get_property;

  klass->assign = NULL;
  klass->cost = gst_cv_object_assign_cost;

  g_object_class_install_property (gobject_class, PROP_SUB_KEY,
      g_param_spec_boxed ("sub-key", "Sub-key", "A structured key useful to "
      "look up the output object info.", GST_TYPE_STRUCTURE,
      (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

  g_object_class_install_property (gobject_class, PROP_ON_CACHE_ID,
      g_param_spec_string ("on-cache-id", "On cache id", "Restrict drawing "
      "taking info from the cache with the given id.", DEFAULT_ON_CACHE_ID,
      (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

  g_object_class_install_property (gobject_class, PROP_ON_LAST_N_FRAMES,
      g_param_spec_uint ("on-last-n-frames", "On last N frames", "Use the last"
      "N frames. So the cost matrix will be built with the following size: "
      "m:(objects in current frame) x n:(objects in the current frame - N).", 1,
      G_MAXUINT, DEFAULT_ON_LAST_N_FRAMES, (GParamFlags) (G_PARAM_READWRITE |
        G_PARAM_STATIC_STRINGS)));
}

static void
gst_cv_object_assign_init (GstCVObjectAssign *self)
{
  self->priv =
      (GstCVObjectAssignPrivate *) gst_cv_object_assign_get_instance_private (
      self);

  self->priv->sub_key = NULL;
  self->priv->frame_number = 0;
  self->priv->on_last_n_frames = DEFAULT_ON_LAST_N_FRAMES;

  gst_opencv_video_filter_set_in_place (GST_OPENCV_VIDEO_FILTER_CAST (self),
      TRUE);
}

static void
gst_cv_object_assign_set_property (GObject *object, guint prop_id, const
    GValue *value, GParamSpec *pspec)
{
  GstCVObjectAssign *self = GST_CV_OBJECT_ASSIGN (object);

  switch (prop_id) {
    case PROP_ON_CACHE_ID:
      self->priv->on_cache_id = g_value_dup_string (value);
      break;
    case PROP_ON_LAST_N_FRAMES:
      self->priv->on_last_n_frames = g_value_get_uint (value);
      break;
    case PROP_SUB_KEY:
      if (self->priv->sub_key)
        gst_structure_free (self->priv->sub_key);
      self->priv->sub_key = GST_STRUCTURE (g_value_dup_boxed (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_cv_object_assign_get_property (GObject *object, guint prop_id,
    GValue *value, GParamSpec *pspec)
{
  GstCVObjectAssign *self = GST_CV_OBJECT_ASSIGN (object);

  switch (prop_id) {
    case PROP_ON_CACHE_ID:
      g_value_set_string (value, self->priv->on_cache_id);
      break;
    case PROP_ON_LAST_N_FRAMES:
      g_value_set_uint (value, self->priv->on_last_n_frames);
      break;
    case PROP_SUB_KEY:
      g_value_set_boxed (value, self->priv->sub_key);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static double
gst_cv_object_assign_cost (GstCVObjectAssign *self, graphene_point_t
    current_centroid, graphene_point_t previous_centroid)
{
  /* Euclidean distance */
  return graphene_point_distance (&current_centroid, &previous_centroid, NULL,
      NULL);
}

static void
gst_cv_object_assign_map_cost_matrix_map_keys_centroids (
    GstCVObjectAssign *self, GstCVObjectInfoMap *object_info_map,
    std::vector<GstStructure *> &keys, std::vector<graphene_point_t> &centroids)
{
  GstCVObjectInfoMapIter iter;
  GstStructure *key;
  GstCVObjectInfo *value;

  gst_cv_object_info_map_iter_init_with_sub_key (&iter, object_info_map,
      self->priv->sub_key);
  while (gst_cv_object_info_map_iter_next (&iter, &key, &value)) {
    graphene_point_t centroid;
    if (!gst_cv_object_info_get_centroid (value, &centroid))
      continue;

    keys.push_back (key);
    centroids.push_back (centroid);
  }
}

static void
gst_cv_object_assign_get_cost_matrix_info (GstCVObjectAssign *self,
    GstCVObjectInfoMap *cur_object_info_map,
    GstCVObjectInfoMap *prev_object_info_map,
    std::vector<std::vector<double> > &cost_matrix,
    std::vector<GstStructure *> &cur_keys,
    std::vector<GstStructure *> &prev_keys)
{
  std::vector<graphene_point_t> cur_centroids, prev_centroids;
  guint i, j;

  gst_cv_object_assign_map_cost_matrix_map_keys_centroids (self,
      cur_object_info_map, cur_keys, cur_centroids);
  gst_cv_object_assign_map_cost_matrix_map_keys_centroids (self,
      prev_object_info_map, prev_keys, prev_centroids);

  for (i = 0; i < (guint) cur_keys.size (); i++) {
    std::vector<double> row;

    for (j = 0; j < prev_keys.size (); j++) {
      double cost;

      cost = GST_CV_OBJECT_ASSIGN_CLASS_GET_CLASS (self)->cost (self,
          cur_centroids[i], prev_centroids[j]);
      row.push_back (cost);
    }

    cost_matrix.push_back (row);
  }
}

static GstCVObjectInfoMap *
gst_cv_object_assign_get_prev_object_info_map (GstCVObjectAssign *self,
    GstCVObjectInfoMapCache *cache)
{
  GstCVObjectInfoMap *map;
  guint i;

  if (self->priv->on_last_n_frames == 1) {
    if (!(map = gst_cv_object_info_map_cache_lookup (cache,
        self->priv->frame_number - 1)) || gst_cv_object_info_map_get_size (
        map) == 0)
      return NULL;
  }

  map = gst_cv_object_info_map_new_full (FALSE, FALSE);
  for (i = 1; i <= self->priv->on_last_n_frames; i++) {
    GstCVObjectInfoMap *i_map;
    GstCVObjectInfoMapIter iter;
    GstStructure *key;
    GstCVObjectInfo *value;

    if (!(i_map = gst_cv_object_info_map_cache_lookup (cache,
        self->priv->frame_number - i)))
      continue;

    gst_cv_object_info_map_iter_init (&iter, i_map);
    while (gst_cv_object_info_map_iter_next (&iter, &key, &value)) {
      if (gst_cv_object_info_map_lookup (map, key))
        continue;
      gst_cv_object_info_map_insert (map, key, value);
    }
  }

  if (gst_cv_object_info_map_get_size (map) == 0) {
    gst_cv_object_info_map_destroy (map);
    return NULL;
  }

  return map;
}

static GstFlowReturn
gst_cv_object_assign_transform_ip (GstOpencvVideoFilter *base, GstBuffer *buf,
    cv::Mat img)
{
  GstCVObjectAssign *self = GST_CV_OBJECT_ASSIGN (base);
  GstCVObjectAssignClass *klass = GST_CV_OBJECT_ASSIGN_CLASS_GET_CLASS (self);
  GstCVObjectInfoMapMeta *meta;
  GstCVObjectInfoMap *prev_object_info_map = NULL, *cur_object_info_map = NULL;
  GstCVObjectInfoMap *cur_object_info_map_copy = NULL;
  GstCVObjectInfoMapNamedCache *named_cache;
  GstCVObjectInfoMapCache *cache;
  std::vector<std::vector<double> > cost_matrix;
  std::vector<GstStructure *> cur_keys, prev_keys;
  std::vector<int> assignment;
  guint i;

  g_return_val_if_fail (klass->assign != NULL, GST_FLOW_ERROR);
  g_return_val_if_fail (self->priv->on_cache_id != NULL,
      GST_FLOW_NOT_SUPPORTED);

  if (!(meta = (GstCVObjectInfoMapMeta *) (gst_buffer_get_meta (buf,
      GST_CV_OBJECT_INFO_MAP_META_API_TYPE))))
    goto ok;

  if (!(cur_object_info_map = gst_cv_object_info_map_meta_get_object_info_map (
      meta)) || gst_cv_object_info_map_get_size (cur_object_info_map) == 0)
    goto ok;

  named_cache = gst_cv_object_info_map_named_cache_get_default ();
  if (!(cache = gst_cv_object_info_map_named_cache_lookup (named_cache,
      self->priv->on_cache_id)))
    goto ok;

  if (!(prev_object_info_map = gst_cv_object_assign_get_prev_object_info_map (
      self, cache)))
    goto ok;

  if (self->priv->sub_key)
    cur_object_info_map_copy = gst_cv_object_info_map_copy_with_sub_key (
        cur_object_info_map, self->priv->sub_key);
  else
    cur_object_info_map_copy = gst_cv_object_info_map_copy (
        cur_object_info_map);

  /**
   * Let be cur_keys the keys of of the objects in the current frame.
   * Let be prev_keys the keys of of the objects in the previous frame.
   * Cost matrix dimenion is m:rows:cur_keys.size() x n:cols:prev_keys.size()
   * Each assignment item maps to the index of the actual index. If m > n,
   * it means that more objects were detected in the current frame than in the
   * previous frame, meaning that only n objects can be assigned, but m - n
   * objects will not be assigned; these will be filled with -1.
   * The assignment array len will always have the size of cur_keys.
   *
   * When this function returns,
   *
   *  - cur_keys and prev_keys:
   *      (1) will have keys of objects with calculable centroid
   *      (2) will have keys containing an "index" field
   *  - cur_keys:
   *      (3) each key will point to the cur_object_info_map_copy's keys
   *  - prev_keys:
   *      (4) each key will point to the prev_object_info_map's keys
   */
  gst_cv_object_assign_get_cost_matrix_info (self, cur_object_info_map_copy,
      prev_object_info_map, cost_matrix, cur_keys, prev_keys);
  GST_CV_OBJECT_ASSIGN_CLASS_GET_CLASS (self)->assign (self, cost_matrix,
      assignment);
  g_assert (assignment.size () == cur_keys.size ());

  /* Remove current keys to avoid insert collisions. However the copy created
   * above will be used for reinsert later. */
  for (i = 0; i < (guint) cur_keys.size (); i++)
    g_assert (
        gst_cv_object_info_map_remove (cur_object_info_map, cur_keys[i]));

  /* Reassign */
  /* First insert reassigned indices. */
  for (i = 0; i < (guint) assignment.size (); i++) {
    GstStructure *prev_key, *cur_key, *new_cur_val_params;
    GstCVObjectInfo *cur_val, *new_cur_val;
    guint prev_index;

    if (assignment[i] == -1)
      continue;

    cur_key = cur_keys[i];
    cur_val = gst_cv_object_info_map_lookup (cur_object_info_map_copy, cur_key);
    g_assert (cur_val != NULL);

    new_cur_val = gst_cv_object_info_copy (cur_val);
    new_cur_val_params = gst_cv_object_info_get_params (new_cur_val);

    prev_key = prev_keys[assignment[i]];

    /* Update index if in params. */
    gst_structure_get (prev_key, GST_CV_OBJECT_INFO_SUB_KEY_INDEX,
        GST_TYPE_CV_OBJECT_INFO_SUB_KEY_INDEX, &prev_index, NULL);
    if (new_cur_val_params && gst_structure_has_field (new_cur_val_params,
        GST_CV_OBJECT_INFO_SUB_PARAM_INDEX)) {
      gst_structure_set (new_cur_val_params, GST_CV_OBJECT_INFO_SUB_PARAM_INDEX,
          GST_TYPE_CV_OBJECT_INFO_SUB_PARAM_INDEX, prev_index, NULL);
    }

    /* Reinsert deleted entry with the right (assigned) index. */
    g_assert (gst_cv_object_info_map_insert (cur_object_info_map,
        gst_structure_copy (
        prev_key), new_cur_val));
  }

  /* Next insert unasigned indices. */
  for (i = 0; i < (guint) assignment.size (); i++) {
    GstStructure *cur_key;
    GstCVObjectInfo *cur_val;

    if (assignment[i] != -1)
      continue;

    cur_key = gst_structure_copy (cur_keys[i]);
    cur_val = gst_cv_object_info_copy (gst_cv_object_info_map_lookup (
        cur_object_info_map_copy, cur_key));

    /* Previously, cur_keys were removed from cur_object_info_map. However, if
     * this key may happen to be AGAIN on this map, because in the reassign
     * process, an object may have been mapped to that key that was previously
     * removed. So, instead of just inserting it as it is, modify it
     * (generating a new index) to prevent colisions. */
    if (G_UNLIKELY (gst_cv_object_info_map_lookup (cur_object_info_map,
        cur_key))) {
      guint new_index = gst_cv_object_info_generate_id ();

      gst_structure_set (cur_key, GST_CV_OBJECT_INFO_SUB_KEY_INDEX,
          GST_TYPE_CV_OBJECT_INFO_SUB_KEY_INDEX, new_index, NULL);
      gst_structure_set (gst_cv_object_info_get_params (cur_val),
          GST_CV_OBJECT_INFO_SUB_PARAM_INDEX,
          GST_TYPE_CV_OBJECT_INFO_SUB_PARAM_INDEX, new_index, NULL);
    }

    gst_cv_object_info_map_insert (cur_object_info_map, cur_key, cur_val);
  }

  gst_cv_object_info_map_destroy (prev_object_info_map);
  gst_cv_object_info_map_destroy (cur_object_info_map_copy);

ok:
  self->priv->frame_number++;
  return GST_FLOW_OK;
}
