#ifndef _CVI_OBJECT_TYPES_H_
#define _CVI_OBJECT_TYPES_H_
#include <stdbool.h>
#include "core/core/cvai_core_types.h"

/** @enum cvai_obj_class_id_e
 *  @ingroup core_cviaicore
 *  @brief classes id of object detection output
 *
 *  A List of class id and it's group:
 *
 * ID  | Class Name       | Group
 * --- | -------------    | -------------
 * 0   | person           | person
 * 1   | bicycle          | vehicle
 * 2   | car              | vehicle
 * 3   | motorcycle       | vehicle
 * 4   | airplane         | vehicle
 * 5   | bus              | vehicle
 * 6   | train            | vehicle
 * 7   | truck            | vehicle
 * 8   | boat             | vehicle
 * 9   | traffic light    | outdoor
 * 10  | fire hydrant     | outdoor
 * 11  | street sign      | outdoor
 * 12  | stop sign        | outdoor
 * 13  | parking meter    | outdoor
 * 14  | bench            | outdoor
 * 15  | bird             | animal
 * 16  | cat              | animal
 * 17  | dog              | animal
 * 18  | horse            | animal
 * 19  | sheep            | animal
 * 20  | cow              | animal
 * 21  | elephant         | animal
 * 22  | bear             | animal
 * 23  | zebra            | animal
 * 24  | giraffe          | animal
 * 25  | hat              | accessory
 * 26  | backpack         | accessory
 * 27  | umbrella         | accessory
 * 28  | shoe             | accessory
 * 29  | eye glasses      | accessory
 * 30  | handbag          | accessory
 * 31  | tie              | accessory
 * 32  | suitcase         | accessory
 * 33  | frisbee          | sports
 * 34  | skis             | sports
 * 35  | snowboard        | sports
 * 36  | sports ball      | sports
 * 37  | kite             | sports
 * 38  | baseball bat     | sports
 * 39  | baseball glove   | sports
 * 40  | skateboard       | sports
 * 41  | surfboard        | sports
 * 42  | tennis racket    | sports
 * 43  | bottle           | kitchen
 * 44  | plate            | kitchen
 * 45  | wine glass       | kitchen
 * 46  | cup              | kitchen
 * 47  | fork             | kitchen
 * 48  | knife            | kitchen
 * 49  | spoon            | kitchen
 * 50  | bowl             | kitchen
 * 51  | banana           | food
 * 52  | apple            | food
 * 53  | sandwich         | food
 * 54  | orange           | food
 * 55  | broccoli         | food
 * 56  | carrot           | food
 * 57  | hot dog          | food
 * 58  | pizza            | food
 * 59  | donut            | food
 * 60  | cake             | food
 * 61  | chair            | furniture
 * 62  | couch            | furniture
 * 63  | potted plant     | furniture
 * 64  | bed              | furniture
 * 65  | mirror           | furniture
 * 66  | dining table     | furniture
 * 67  | window           | furniture
 * 68  | desk             | furniture
 * 69  | toilet           | furniture
 * 70  | door             | furniture
 * 71  | tv               | electronic
 * 72  | laptop           | electronic
 * 73  | mouse            | electronic
 * 74  | remote           | electronic
 * 75  | keyboard         | electronic
 * 76  | cell phone       | electronic
 * 77  | microwave        | appliance
 * 78  | oven             | appliance
 * 79  | toaster          | appliance
 * 80  | sink             | appliance
 * 81  | refrigerator     | appliance
 * 82  | blender          | appliance
 * 83  | book             | indoor
 * 84  | clock            | indoor
 * 85  | vase             | indoor
 * 86  | scissors         | indoor
 * 87  | teddy bear       | indoor
 * 88  | hair drier       | indoor
 * 89  | toothbrush       | indoor
 * 90  | hair brush       | indoor
 */
typedef enum {
  CVI_AI_DET_TYPE_PERSON,
  CVI_AI_DET_TYPE_BICYCLE,
  CVI_AI_DET_TYPE_CAR,
  CVI_AI_DET_TYPE_MOTORBIKE,
  CVI_AI_DET_TYPE_AEROPLANE,
  CVI_AI_DET_TYPE_BUS,
  CVI_AI_DET_TYPE_TRAIN,
  CVI_AI_DET_TYPE_TRUCK,
  CVI_AI_DET_TYPE_BOAT,
  CVI_AI_DET_TYPE_TRAFFIC_LIGHT,
  CVI_AI_DET_TYPE_FIRE_HYDRANT,
  CVI_AI_DET_TYPE_STREET_SIGN,
  CVI_AI_DET_TYPE_STOP_SIGN,
  CVI_AI_DET_TYPE_PARKING_METER,
  CVI_AI_DET_TYPE_BENCH,
  CVI_AI_DET_TYPE_BIRD,
  CVI_AI_DET_TYPE_CAT,
  CVI_AI_DET_TYPE_DOG,
  CVI_AI_DET_TYPE_HORSE,
  CVI_AI_DET_TYPE_SHEEP,
  CVI_AI_DET_TYPE_COW,
  CVI_AI_DET_TYPE_ELEPHANT,
  CVI_AI_DET_TYPE_BEAR,
  CVI_AI_DET_TYPE_ZEBRA,
  CVI_AI_DET_TYPE_GIRAFFE,
  CVI_AI_DET_TYPE_HAT,
  CVI_AI_DET_TYPE_BACKPACK,
  CVI_AI_DET_TYPE_UMBRELLA,
  CVI_AI_DET_TYPE_SHOE,
  CVI_AI_DET_TYPE_EYE_GLASSES,
  CVI_AI_DET_TYPE_HANDBAG,
  CVI_AI_DET_TYPE_TIE,
  CVI_AI_DET_TYPE_SUITCASE,
  CVI_AI_DET_TYPE_FRISBEE,
  CVI_AI_DET_TYPE_SKIS,
  CVI_AI_DET_TYPE_SNOWBOARD,
  CVI_AI_DET_TYPE_SPORTS_BALL,
  CVI_AI_DET_TYPE_KITE,
  CVI_AI_DET_TYPE_BASEBALL_BAT,
  CVI_AI_DET_TYPE_BASEBALL_GLOVE,
  CVI_AI_DET_TYPE_SKATEBOARD,
  CVI_AI_DET_TYPE_SURFBOARD,
  CVI_AI_DET_TYPE_TENNIS_RACKET,
  CVI_AI_DET_TYPE_BOTTLE,
  CVI_AI_DET_TYPE_PLATE,
  CVI_AI_DET_TYPE_WINE_GLASS,
  CVI_AI_DET_TYPE_CUP,
  CVI_AI_DET_TYPE_FORK,
  CVI_AI_DET_TYPE_KNIFE,
  CVI_AI_DET_TYPE_SPOON,
  CVI_AI_DET_TYPE_BOWL,
  CVI_AI_DET_TYPE_BANANA,
  CVI_AI_DET_TYPE_APPLE,
  CVI_AI_DET_TYPE_SANDWICH,
  CVI_AI_DET_TYPE_ORANGE,
  CVI_AI_DET_TYPE_BROCCOLI,
  CVI_AI_DET_TYPE_CARROT,
  CVI_AI_DET_TYPE_HOT_DOG,
  CVI_AI_DET_TYPE_PIZZA,
  CVI_AI_DET_TYPE_DONUT,
  CVI_AI_DET_TYPE_CAKE,
  CVI_AI_DET_TYPE_CHAIR,
  CVI_AI_DET_TYPE_SOFA,
  CVI_AI_DET_TYPE_POTTED_PLANT,
  CVI_AI_DET_TYPE_BED,
  CVI_AI_DET_TYPE_MIRROR,
  CVI_AI_DET_TYPE_DINING_TABLE,
  CVI_AI_DET_TYPE_WINDOW,
  CVI_AI_DET_TYPE_DESK,
  CVI_AI_DET_TYPE_TOILET,
  CVI_AI_DET_TYPE_DOOR,
  CVI_AI_DET_TYPE_TV_MONITOR,
  CVI_AI_DET_TYPE_LAPTOP,
  CVI_AI_DET_TYPE_MOUSE,
  CVI_AI_DET_TYPE_REMOTE,
  CVI_AI_DET_TYPE_KEYBOARD,
  CVI_AI_DET_TYPE_CELL_PHONE,
  CVI_AI_DET_TYPE_MICROWAVE,
  CVI_AI_DET_TYPE_OVEN,
  CVI_AI_DET_TYPE_TOASTER,
  CVI_AI_DET_TYPE_SINK,
  CVI_AI_DET_TYPE_REFRIGERATOR,
  CVI_AI_DET_TYPE_BLENDER,
  CVI_AI_DET_TYPE_BOOK,
  CVI_AI_DET_TYPE_CLOCK,
  CVI_AI_DET_TYPE_VASE,
  CVI_AI_DET_TYPE_SCISSORS,
  CVI_AI_DET_TYPE_TEDDY_BEAR,
  CVI_AI_DET_TYPE_HAIR_DRIER,
  CVI_AI_DET_TYPE_TOOTHBRUSH,
  CVI_AI_DET_TYPE_HAIR_BRUSH,
  CVI_AI_DET_TYPE_END,
} cvai_obj_class_id_e;

/** @enum cvai_obj_det_group_type_e
 *  @ingroup core_cviaicore
 *  @brief group id for detection classes
 *
 * group id bit format:
 *    1 bits          15 bits            16 bits
 * -----------------------------------------------------
 * | mask bit |  start of class id  |  end of class id |
 * -----------------------------------------------------
 */
#define GROUP_ID(id_start, id_end) (1 << 31) | (id_start << 16) | id_end
typedef enum {
  CVI_AI_DET_GROUP_ALL = GROUP_ID(CVI_AI_DET_TYPE_PERSON, CVI_AI_DET_TYPE_HAIR_BRUSH),
  CVI_AI_DET_GROUP_PERSON = GROUP_ID(CVI_AI_DET_TYPE_PERSON, CVI_AI_DET_TYPE_PERSON),
  CVI_AI_DET_GROUP_VEHICLE = GROUP_ID(CVI_AI_DET_TYPE_BICYCLE, CVI_AI_DET_TYPE_BOAT),
  CVI_AI_DET_GROUP_OUTDOOR = GROUP_ID(CVI_AI_DET_TYPE_TRAFFIC_LIGHT, CVI_AI_DET_TYPE_BENCH),
  CVI_AI_DET_GROUP_ANIMAL = GROUP_ID(CVI_AI_DET_TYPE_BIRD, CVI_AI_DET_TYPE_GIRAFFE),
  CVI_AI_DET_GROUP_ACCESSORY = GROUP_ID(CVI_AI_DET_TYPE_HAT, CVI_AI_DET_TYPE_SUITCASE),
  CVI_AI_DET_GROUP_SPORTS = GROUP_ID(CVI_AI_DET_TYPE_FRISBEE, CVI_AI_DET_TYPE_TENNIS_RACKET),
  CVI_AI_DET_GROUP_KITCHEN = GROUP_ID(CVI_AI_DET_TYPE_BOTTLE, CVI_AI_DET_TYPE_BOWL),
  CVI_AI_DET_GROUP_FOOD = GROUP_ID(CVI_AI_DET_TYPE_BANANA, CVI_AI_DET_TYPE_CAKE),
  CVI_AI_DET_GROUP_FURNITURE = GROUP_ID(CVI_AI_DET_TYPE_CHAIR, CVI_AI_DET_TYPE_DOOR),
  CVI_AI_DET_GROUP_ELECTRONIC = GROUP_ID(CVI_AI_DET_TYPE_TV_MONITOR, CVI_AI_DET_TYPE_CELL_PHONE),
  CVI_AI_DET_GROUP_APPLIANCE = GROUP_ID(CVI_AI_DET_TYPE_MICROWAVE, CVI_AI_DET_TYPE_BLENDER),
  CVI_AI_DET_GROUP_INDOOR = GROUP_ID(CVI_AI_DET_TYPE_BOOK, CVI_AI_DET_TYPE_HAIR_BRUSH),

  CVI_AI_DET_GROUP_MASK_HEAD = 0x1 << 31,
  CVI_AI_DET_GROUP_MASK_START = 0x7FFF << 16,
  CVI_AI_DET_GROUP_MASK_END = 0xFFFF,
} cvai_obj_det_group_type_e;

#undef GROUP_ID

/** @struct cvai_pose17_meta_t
 * @ingroup core_cviaicore
 * @brief A structure to describe person pose.
 *
 * @var cvai_pose17_meta_t::x
 * Position x point.
 * @var cvai_pose17_meta_t::y
 * Position y point.
 * @var cvai_pose17_meta_t::score
 * Point score
 *
 * @see cvai_object_t
 */
typedef struct {
  float x[17];
  float y[17];
  float score[17];
} cvai_pose17_meta_t;

/** @struct cvai_vehicle_meta
 * @ingroup core_cviaicore
 * @brief A structure to describe a vehicle properity.
 * @var cvai_vehicle_meta::license_pts
 * The license plate 4 corner points.
 * @var cvai_vehicle_meta::license_bbox
 * The license bounding box.
 * @var cvai_vehicle_meta::license_char
 * The license characters
 * @see cvai_4_pts_t
 * @see cvai_bbox_t
 * @see cvai_object_info_t
 */
typedef struct {
  cvai_4_pts_t license_pts;
  cvai_bbox_t license_bbox;
  char license_char[255];
} cvai_vehicle_meta;

/** @struct cvai_pedestrian_meta
 * @ingroup core_cviaicore
 * @brief A structure to describe a pedestrian properity.
 * @var cvai_pedestrian_meta::pose_17
 * The Person 17 keypoints detected by pose estimation models
 * @var cvai_pedestrian_meta::fall
 * Whether people is fall or not
 * @see cvai_pose17_meta_t
 * @see cvai_object_info_t
 */
typedef struct {
  cvai_pose17_meta_t pose_17;
  bool fall;
} cvai_pedestrian_meta;

/** @struct cvai_object_info_t
 * @ingroup core_cviaicore
 * @brief A structure to describe a found object.
 *
 * @var cvai_object_info_t::name
 * A human readable class name.
 * @var cvai_object_info_t::unique_id
 * The unique id of an object.
 * @var cvai_object_info_t::bbox
 * The bounding box of an object.
 * @var cvai_object_info_t::bpts
 * The bounding points of an object. (Deprecated)
 * @var cvai_object_info_t::feature
 * The feature describing an object.
 * @var cvai_object_info_t::classes
 * The class label of an object.
 * @var cvai_object_info_t::vehicle_properity
 * The vehicle properity
 * @var cvai_object_info_t::pedestrian_properity
 * The pedestrian properity
 * @see cvai_object_t
 * @see cvai_pedestrian_meta
 * @see cvai_vehicle_meta
 * @see cvai_bbox_t
 * @see cvai_pts_t
 * @see cvai_feature_t
 */
typedef struct {
  char name[128];
  uint64_t unique_id;
  cvai_bbox_t bbox;
  cvai_feature_t feature;
  int classes;

  cvai_vehicle_meta *vehicle_properity;
  cvai_pedestrian_meta *pedestrian_properity;
} cvai_object_info_t;

/** @struct cvai_object_t
 *  @ingroup core_cviaicore
 *  @brief The data structure for storing object meta.
 *
 *  @var cvai_object_t::size
 *  The size of the info.
 *  @var cvai_object_t::width
 *  The current width. Affects the coordinate recovery of bbox.
 *  @var cvai_object_t::height
 *  The current height. Affects the coordinate recovery of bbox.
 *  @var cvai_object_t::info
 *  The information of each object.
 *
 *  @see cvai_object_info_t
 */
typedef struct {
  uint32_t size;
  uint32_t width;
  uint32_t height;
  meta_rescale_type_e rescale_type;
  cvai_object_info_t *info;
} cvai_object_t;

/** @struct cvai_class_filter_t
 *  @ingroup core_cviaicore
 *  @brief Preserve class id of model output and filter out the others. This struct can be used in
 *  Semantic Segmentation.
 *  @var cvai_class_filter_t::preserved_class_ids
 *  The class IDs to be preserved
 *  @var cvai_class_filter_t::num_preserved_classes
 *  Number of classes to be preserved
 */
typedef struct {
  uint32_t *preserved_class_ids;
  uint32_t num_preserved_classes;
} cvai_class_filter_t;

#endif