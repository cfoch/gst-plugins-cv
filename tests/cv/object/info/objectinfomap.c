#include <gst/cv/object/info/objectinfomap.h>

static GstCVObjectInfo *
create_object_info (gint internal_value)
{
  GstCVObjectInfo *info;
  GValue value = G_VALUE_INIT;

  g_value_init (&value, G_TYPE_INT);
  g_value_set_int (&value, internal_value);

  info = gst_cv_object_info_new (&value);

  g_value_unset (&value);

  return info;
}

static void
test_insert_invalid_args ()
{
  GstCVObjectInfoMap *map;
  GstStructure *valid_key, *invalid_key;
  GstCVObjectInfo *valid_value;

  valid_value = create_object_info (100);
  valid_key =
      gst_structure_new_from_string ("valid, foo=(int)1, bar=(string)baz;");

  map = gst_cv_object_info_map_new ();
  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL,
      "*assertion*G_IS_VALUE*failed");
  g_assert_false (gst_cv_object_info_map_insert (map, valid_key, NULL));

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL,
      "*assertion*GST_IS_STRUCTURE*failed");
  g_assert_false (gst_cv_object_info_map_insert (map, NULL, valid_value));

  invalid_key = gst_structure_new_from_string ("invalid, foo=(fraction)1;");
  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL,
      "*gst_cv_object_info_map_check_key*");
  gst_cv_object_info_map_insert (map, invalid_key, valid_value);
  gst_structure_free (invalid_key);

  invalid_key =
      gst_structure_new_from_string ("invalid, foo=(date)1994-01-17;");
  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL,
      "*gst_cv_object_info_map_check_key*");
  gst_cv_object_info_map_insert (map, invalid_key, valid_value);
  gst_structure_free (invalid_key);

  gst_cv_object_info_free (valid_value);
}

static void
test_insert_lookup ()
{
  GstCVObjectInfoMap *map;
  GstStructure *key1, *key2, *key3, *key4, *key5;
  GstCVObjectInfo *val1, *val2, *val3, *val4, *val5;

  key1 = gst_structure_new_from_string ("whatever, index=(int)1, "
      "label=(string)car, element=(string)element0, type=(string)roi;");
  val1 = create_object_info (1);

  map = gst_cv_object_info_map_new ();
  g_assert_true (gst_cv_object_info_map_insert (map, key1, val1));
  g_assert_true (val1 == gst_cv_object_info_map_lookup (map, key1));

  key2 = gst_structure_new_from_string ("whatever, index=(int)2, "
      "label=(string)car, element=(string)element0, type=(string)roi;");
  val2 = create_object_info (2);
  g_assert_true (gst_cv_object_info_map_insert (map, key2, val2));
  g_assert_true (val1 == gst_cv_object_info_map_lookup (map, key1));
  g_assert_true (val2 == gst_cv_object_info_map_lookup (map, key2));

  key3 = gst_structure_new_from_string ("whatever, index=(int)1, "
      "label=(string)face, element=(string)element1, type=(string)roi;");
  val3 = create_object_info (3);
  g_assert_true (gst_cv_object_info_map_insert (map, key3, val3));
  g_assert_true (val1 == gst_cv_object_info_map_lookup (map, key1));
  g_assert_true (val2 == gst_cv_object_info_map_lookup (map, key2));
  g_assert_true (val3 == gst_cv_object_info_map_lookup (map, key3));

  key4 = gst_structure_new_from_string ("whatever, index=(int)2, "
      "label=(string)face, element=(string)element1, type=(string)roi;");
  val4 = create_object_info (4);
  g_assert_true (gst_cv_object_info_map_insert (map, key4, val4));
  g_assert_true (val1 == gst_cv_object_info_map_lookup (map, key1));
  g_assert_true (val2 == gst_cv_object_info_map_lookup (map, key2));
  g_assert_true (val3 == gst_cv_object_info_map_lookup (map, key3));
  g_assert_true (val4 == gst_cv_object_info_map_lookup (map, key4));

  key5 = gst_structure_new_from_string ("whatever, banana=(string)yellow;");
  val5 = create_object_info (100);
  g_assert_true (gst_cv_object_info_map_insert (map, key5, val5));
  g_assert_true (val1 == gst_cv_object_info_map_lookup (map, key1));
  g_assert_true (val2 == gst_cv_object_info_map_lookup (map, key2));
  g_assert_true (val3 == gst_cv_object_info_map_lookup (map, key3));
  g_assert_true (val4 == gst_cv_object_info_map_lookup (map, key4));
  g_assert_true (val5 == gst_cv_object_info_map_lookup (map, key5));

  gst_cv_object_info_map_destroy (map);
}

static void
test_iter ()
{
  GstCVObjectInfoMap *map;
  GstStructure *subkey;
  GstStructure *key1, *key2, *key3, *key4, *key5;
  GstCVObjectInfo *val1, *val2, *val3, *val4, *val5;
  /* iter vars */
  GSList *list = NULL;
  GstCVObjectInfoMapIter iter;
  GstStructure *key;
  GstCVObjectInfo *value;

  val1 = create_object_info (1);
  key1 = gst_structure_new_from_string ("whatever, index=(int)1, "
      "label=(string)car, element=(string)element0, type=(string)roi;");

  key2 = gst_structure_new_from_string ("whatever, index=(int)2, "
      "label=(string)car, element=(string)element0, type=(string)roi;");
  val2 = create_object_info (2);

  key3 = gst_structure_new_from_string ("whatever, index=(int)1, "
      "label=(string)face, element=(string)element1, type=(string)roi;");
  val3 = create_object_info (3);

  key4 = gst_structure_new_from_string ("whatever, index=(int)2, "
      "label=(string)face, element=(string)element1, type=(string)roi;");
  val4 = create_object_info (4);

  key5 = gst_structure_new_from_string ("whatever, banana=(string)yellow;");
  val5 = create_object_info (100);

  map = gst_cv_object_info_map_new ();
  gst_cv_object_info_map_insert (map, key1, val1);
  gst_cv_object_info_map_insert (map, key2, val2);
  gst_cv_object_info_map_insert (map, key3, val3);
  gst_cv_object_info_map_insert (map, key4, val4);
  gst_cv_object_info_map_insert (map, key5, val5);

  list = NULL;
  gst_cv_object_info_map_iter_init (&iter, map);
  while (gst_cv_object_info_map_iter_next (&iter, &key, &value)) {
    g_assert_null (g_slist_find (list, key));
    g_assert_true ((key == key1 && value == val1) ||
        (key == key2 && value == val2) ||
        (key == key3 && value == val3) ||
        (key == key4 && value == val4) || (key == key5 && value == val5));
  }
  g_slist_free (list);

  list = NULL;
  subkey = gst_structure_new_from_string ("whatever, label=(string)face, "
      "element=(string)element1;");
  gst_cv_object_info_map_iter_init_with_sub_key (&iter, map, subkey);
  while (gst_cv_object_info_map_iter_next_with_sub_key (&iter, &key, &value)) {
    g_assert_null (g_slist_find (list, key));
    g_assert_true ((key == key3 && value == val3) || (key == key4
            && value == val4));
  }
  gst_structure_free (subkey);
  g_slist_free (list);

  gst_cv_object_info_map_destroy (map);
}

int
main (int argc, char *argv[])
{
  gst_init (&argc, &argv);
  g_test_init (&argc, &argv, NULL);
  g_test_add_func ("/cv/object/info/test_info_map_insert_invalid_args",
      test_insert_invalid_args);
  g_test_add_func ("/cv/object/info/test_insert_lookup", test_insert_lookup);
  g_test_add_func ("/cv/object/info/test_iter", test_iter);

  return g_test_run ();
}
