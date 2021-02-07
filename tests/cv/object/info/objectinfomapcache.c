#include <gst/cv/object/info/objectinfomapcache.h>

static GstCVObjectInfo *
create_object_info (gint internal_value)
{
  GstCVObjectInfo *info;
  GValue value = G_VALUE_INIT;

  g_value_init (&value, G_TYPE_INT);
  g_value_set_int (&value, internal_value);

  info = gst_cv_object_info_new (&value);

  return info;
}


gboolean
object_info_map_is_equal (GstCVObjectInfoMap * map1, GstCVObjectInfoMap * map2)
{
  GstCVObjectInfoMapIter iter;
  GstStructure *key1;
  GstCVObjectInfo *value1, *value2;

  if (gst_cv_object_info_map_get_size (map1) !=
      gst_cv_object_info_map_get_size (map2))
    return FALSE;

  gst_cv_object_info_map_iter_init (&iter, map1);
  while (gst_cv_object_info_map_iter_next (&iter, &key1, &value1)) {
    const GValue *value1_value, *value2_value;

    value2 = gst_cv_object_info_map_lookup (map2, key1);

    value1_value = gst_cv_object_info_get_value (value1);
    value2_value = gst_cv_object_info_get_value (value2);

    if (g_value_get_int (value1_value) != g_value_get_int (value2_value))
      return FALSE;
  }

  return TRUE;
}

static void
test_new ()
{
  GstCVObjectInfoMapCache *cache;

  cache = gst_cv_object_info_map_cache_new (5);
  g_assert_cmpint (0, ==, gst_cv_object_info_map_cache_get_size (cache));
  g_assert_cmpint (5, ==, gst_cv_object_info_map_cache_get_max_size (cache));
  gst_cv_object_info_map_cache_destroy (cache);
}

static void
test_insert ()
{
  GstCVObjectInfoMapCache *cache;
  /* map1 */
  GstCVObjectInfoMap *map1;
  GstStructure *map1_key1, *map1_key2;
  GstCVObjectInfo *map1_val1, *map1_val2;
  /* map2 */
  GstCVObjectInfoMap *map2;
  GstStructure *map2_key1, *map2_key2;
  GstCVObjectInfo *map2_val1, *map2_val2;
  /* map3 */
  GstCVObjectInfoMap *map3;
  GstStructure *map3_key1, *map3_key2;
  GstCVObjectInfo *map3_val1, *map3_val2;
  /* map4 */
  GstCVObjectInfoMap *map4;
  GstStructure *map4_key1, *map4_key2;
  GstCVObjectInfo *map4_val1, *map4_val2;

  cache = gst_cv_object_info_map_cache_new (3);

  /* Insert map at frame 100 */
  map1 = gst_cv_object_info_map_new ();
  map1_key1 = gst_structure_new_from_string ("whatever, index=(int)1, "
      "label=(string)car, element=(string)element0, type=(string)roi;");
  map1_val1 = create_object_info (11);
  gst_cv_object_info_map_insert (map1, map1_key1, map1_val1);

  map1_key2 = gst_structure_new_from_string ("whatever, index=(int)2, "
      "label=(string)car, element=(string)element0, type=(string)roi;");
  map1_val2 = create_object_info (12);
  gst_cv_object_info_map_insert (map1, map1_key2, map1_val2);

  g_assert_true (gst_cv_object_info_map_cache_insert (cache, 100, map1));

  g_assert_true (object_info_map_is_equal (map1, (GstCVObjectInfoMap *)
          gst_cv_object_info_map_cache_lookup (cache, 100)));

  /* Insert map at frame 101 */
  map2 = gst_cv_object_info_map_new ();
  map2_key1 = gst_structure_new_from_string ("whatever, index=(int)1, "
      "label=(string)car, element=(string)element0, type=(string)roi;");
  map2_val1 = create_object_info (21);
  gst_cv_object_info_map_insert (map2, map2_key1, map2_val1);

  map2_key2 = gst_structure_new_from_string ("whatever, index=(int)2, "
      "label=(string)car, element=(string)element0, type=(string)roi;");
  map2_val2 = create_object_info (22);
  gst_cv_object_info_map_insert (map2, map2_key2, map2_val2);

  g_assert_true (gst_cv_object_info_map_cache_insert (cache, 101, map2));

  g_assert_true (object_info_map_is_equal (map1, (GstCVObjectInfoMap *)
          gst_cv_object_info_map_cache_lookup (cache, 100)));
  g_assert_true (object_info_map_is_equal (map2, (GstCVObjectInfoMap *)
          gst_cv_object_info_map_cache_lookup (cache, 101)));

  /* Insert map at frame 102 */
  map3 = gst_cv_object_info_map_new ();
  map3_key1 = gst_structure_new_from_string ("whatever, index=(int)1, "
      "label=(string)car, element=(string)element0, type=(string)roi;");
  map3_val1 = create_object_info (31);
  gst_cv_object_info_map_insert (map3, map3_key1, map3_val1);

  map3_key2 = gst_structure_new_from_string ("whatever, index=(int)2, "
      "label=(string)car, element=(string)element0, type=(string)roi;");
  map3_val2 = create_object_info (32);
  gst_cv_object_info_map_insert (map3, map3_key2, map3_val2);

  g_assert_true (gst_cv_object_info_map_cache_insert (cache, 102, map3));

  g_assert_true (object_info_map_is_equal (map1, (GstCVObjectInfoMap *)
          gst_cv_object_info_map_cache_lookup (cache, 100)));
  g_assert_true (object_info_map_is_equal (map2, (GstCVObjectInfoMap *)
          gst_cv_object_info_map_cache_lookup (cache, 101)));
  g_assert_true (object_info_map_is_equal (map3, (GstCVObjectInfoMap *)
          gst_cv_object_info_map_cache_lookup (cache, 102)));

  /* Insert map at frame 103 */
  map4 = gst_cv_object_info_map_new ();
  map4_key1 = gst_structure_new_from_string ("whatever, index=(int)1, "
      "label=(string)car, element=(string)element0, type=(string)roi;");
  map4_val1 = create_object_info (41);
  gst_cv_object_info_map_insert (map4, map4_key1, map4_val1);

  map4_key2 = gst_structure_new_from_string ("whatever, index=(int)2, "
      "label=(string)car, element=(string)element0, type=(string)roi;");
  map4_val2 = create_object_info (42);
  gst_cv_object_info_map_insert (map4, map4_key2, map4_val2);

  g_assert_true (gst_cv_object_info_map_cache_insert (cache, 103, map4));
  // Cache size reached. Map at frame 100 should have been removed internally.
  g_assert_null (gst_cv_object_info_map_cache_lookup (cache, 100));

  g_assert_true (object_info_map_is_equal (map2, (GstCVObjectInfoMap *)
          gst_cv_object_info_map_cache_lookup (cache, 101)));
  g_assert_true (object_info_map_is_equal (map3, (GstCVObjectInfoMap *)
          gst_cv_object_info_map_cache_lookup (cache, 102)));
  g_assert_true (object_info_map_is_equal (map4, (GstCVObjectInfoMap *)
          gst_cv_object_info_map_cache_lookup (cache, 103)));

  gst_cv_object_info_map_destroy (map1);
  gst_cv_object_info_map_destroy (map2);
  gst_cv_object_info_map_destroy (map3);
  gst_cv_object_info_map_destroy (map4);
  gst_cv_object_info_map_cache_destroy (cache);
}

int
main (int argc, char *argv[])
{
  gst_init (&argc, &argv);
  g_test_init (&argc, &argv, NULL);
  g_test_add_func ("/cv/object/info/test_info_map_cache_new", test_new);
  g_test_add_func ("/cv/object/info/test_info_map_cache_insert", test_insert);

  return g_test_run ();
}
