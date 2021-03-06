/**
 * Copyright (c) 2014-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

#include "Yoga.h"

using YGVector = std::vector<YGNodeRef>;

YG_EXTERN_C_BEGIN

WIN_EXPORT float YGRoundValueToPixelGrid(const float value,
                                         const float pointScaleFactor,
                                         const bool forceCeil,
                                         const bool forceFloor);

YG_EXTERN_C_END

extern const std::array<YGEdge, 4> trailing;
extern const std::array<YGEdge, 4> leading;
extern bool YGValueEqual(const YGValue a, const YGValue b);
extern const YGValue YGValueUndefined;
extern const YGValue YGValueAuto;
extern const YGValue YGValueZero;

template <std::size_t size>
bool YGValueArrayEqual(
    const std::array<YGValue, size> val1,
    const std::array<YGValue, size> val2) {
  bool areEqual = true;
  for (uint32_t i = 0; i < size && areEqual; ++i) {
    areEqual = YGValueEqual(val1[i], val2[i]);
  }
  return areEqual;
}

typedef struct YGCachedMeasurement {
  float availableWidth;
  float availableHeight;
  YGMeasureMode widthMeasureMode;
  YGMeasureMode heightMeasureMode;

  float computedWidth;
  float computedHeight;
} YGCachedMeasurement;

// This value was chosen based on empiracle data. Even the most complicated
// layouts should not require more than 16 entries to fit within the cache.
#define YG_MAX_CACHED_RESULT_COUNT 16

struct YGLayout {
  std::array<float, 4> position;
  std::array<float, 2> dimensions;
  std::array<float, 6> margin;
  std::array<float, 6> border;
  std::array<float, 6> padding;
  YGDirection direction;

  uint32_t computedFlexBasisGeneration;
  float computedFlexBasis;
  bool hadOverflow;

  // Instead of recomputing the entire layout every single time, we
  // cache some information to break early when nothing changed
  uint32_t generationCount;
  YGDirection lastParentDirection;

  uint32_t nextCachedMeasurementsIndex;
  YGCachedMeasurement cachedMeasurements[YG_MAX_CACHED_RESULT_COUNT];
  std::array<float, 2> measuredDimensions;

  YGCachedMeasurement cachedLayout;
};

struct YGStyle {
  YGDirection direction;
  YGFlexDirection flexDirection;
  YGJustify justifyContent;
  YGAlign alignContent;
  YGAlign alignItems;
  YGAlign alignSelf;
  YGPositionType positionType;
  YGWrap flexWrap;
  YGOverflow overflow;
  YGDisplay display;
  float flex;
  float flexGrow;
  float flexShrink;
  YGValue flexBasis;
  std::array<YGValue, YGEdgeCount> margin;
  std::array<YGValue, YGEdgeCount> position;
  std::array<YGValue, YGEdgeCount> padding;
  std::array<YGValue, YGEdgeCount> border;
  std::array<YGValue, 2> dimensions;
  std::array<YGValue, 2> minDimensions;
  std::array<YGValue, 2> maxDimensions;

  // Yoga specific properties, not compatible with flexbox specification
  float aspectRatio;
  bool operator==(YGStyle style) {
    bool areNonFloatValuesEqual = direction == style.direction &&
        flexDirection == style.flexDirection &&
        justifyContent == style.justifyContent &&
        alignContent == style.alignContent && alignItems == style.alignItems &&
        alignSelf == style.alignSelf && positionType == style.positionType &&
        flexWrap == style.flexWrap && overflow == style.overflow &&
        display == style.display && YGValueEqual(flexBasis, style.flexBasis) &&
        YGValueArrayEqual(margin, style.margin) &&
        YGValueArrayEqual(position, style.position) &&
        YGValueArrayEqual(padding, style.padding) &&
        YGValueArrayEqual(border, style.border) &&
        YGValueArrayEqual(dimensions, style.dimensions) &&
        YGValueArrayEqual(minDimensions, style.minDimensions) &&
        YGValueArrayEqual(maxDimensions, style.maxDimensions);

    if (!(std::isnan(flex) && std::isnan(style.flex))) {
      areNonFloatValuesEqual = areNonFloatValuesEqual && flex == style.flex;
    }

    if (!(std::isnan(flexGrow) && std::isnan(style.flexGrow))) {
      areNonFloatValuesEqual =
          areNonFloatValuesEqual && flexGrow == style.flexGrow;
    }

    if (!(std::isnan(flexShrink) && std::isnan(style.flexShrink))) {
      areNonFloatValuesEqual =
          areNonFloatValuesEqual && flexShrink == style.flexShrink;
    }

    if (!(std::isnan(aspectRatio) && std::isnan(style.aspectRatio))) {
      areNonFloatValuesEqual =
          areNonFloatValuesEqual && aspectRatio == style.aspectRatio;
    }

    return areNonFloatValuesEqual;
  }

  bool operator!=(YGStyle style) {
    return !(*this == style);
  }
};

typedef struct YGConfig {
  bool experimentalFeatures[YGExperimentalFeatureCount + 1];
  bool useWebDefaults;
  bool useLegacyStretchBehaviour;
  float pointScaleFactor;
  YGLogger logger;
  YGNodeClonedFunc cloneNodeCallback;
  void* context;
} YGConfig;

#define YG_UNDEFINED_VALUES \
  { YGUndefined, YGUnitUndefined }

#define YG_AUTO_VALUES \
  { YGUndefined, YGUnitAuto }

#define YG_DEFAULT_EDGE_VALUES_UNIT                                            \
  {                                                                            \
    YG_UNDEFINED_VALUES, YG_UNDEFINED_VALUES,     \
    YG_UNDEFINED_VALUES, YG_UNDEFINED_VALUES, \
    YG_UNDEFINED_VALUES, YG_UNDEFINED_VALUES,    \
    YG_UNDEFINED_VALUES,                                  \
    YG_UNDEFINED_VALUES, YG_UNDEFINED_VALUES, \
  }

#define YG_DEFAULT_DIMENSION_VALUES \
  { YGUndefined, YGUndefined, }

#define YG_DEFAULT_DIMENSION_VALUES_UNIT       \
  {                                            \
    YG_UNDEFINED_VALUES,  \
    YG_UNDEFINED_VALUES, \
  }

#define YG_DEFAULT_DIMENSION_VALUES_AUTO_UNIT \
  { YG_AUTO_VALUES, YG_AUTO_VALUES, }

static const float kDefaultFlexGrow = 0.0f;
static const float kDefaultFlexShrink = 0.0f;
static const float kWebDefaultFlexShrink = 1.0f;

static const YGStyle gYGNodeStyleDefaults = {
    YGDirectionInherit,
    YGFlexDirectionColumn,
    YGJustifyFlexStart,
    YGAlignFlexStart,
    YGAlignStretch,
    YGAlignAuto,
    YGPositionTypeRelative,
    YGWrapNoWrap,
    YGOverflowVisible,
    YGDisplayFlex,
    YGUndefined,
    YGUndefined,
    YGUndefined,
    YG_AUTO_VALUES,
    {{YG_UNDEFINED_VALUES,
                YG_UNDEFINED_VALUES,
                YG_UNDEFINED_VALUES,
                YG_UNDEFINED_VALUES,
                YG_UNDEFINED_VALUES,
                YG_UNDEFINED_VALUES,
                YG_UNDEFINED_VALUES,
                YG_UNDEFINED_VALUES,
                YG_UNDEFINED_VALUES}},
    {{YG_UNDEFINED_VALUES,
                  YG_UNDEFINED_VALUES,
                  YG_UNDEFINED_VALUES,
                  YG_UNDEFINED_VALUES,
                  YG_UNDEFINED_VALUES,
                  YG_UNDEFINED_VALUES,
                  YG_UNDEFINED_VALUES,
                  YG_UNDEFINED_VALUES,
                  YG_UNDEFINED_VALUES}},
    {{YG_UNDEFINED_VALUES,
                 YG_UNDEFINED_VALUES,
                 YG_UNDEFINED_VALUES,
                 YG_UNDEFINED_VALUES,
                 YG_UNDEFINED_VALUES,
                 YG_UNDEFINED_VALUES,
                 YG_UNDEFINED_VALUES,
                 YG_UNDEFINED_VALUES,
                 YG_UNDEFINED_VALUES}},
    {{YG_UNDEFINED_VALUES,
                YG_UNDEFINED_VALUES,
                YG_UNDEFINED_VALUES,
                YG_UNDEFINED_VALUES,
                YG_UNDEFINED_VALUES,
                YG_UNDEFINED_VALUES,
                YG_UNDEFINED_VALUES,
                YG_UNDEFINED_VALUES,
                YG_UNDEFINED_VALUES}},
    {{YG_AUTO_VALUES, YG_AUTO_VALUES}},
    {{YG_UNDEFINED_VALUES, YG_UNDEFINED_VALUES}},
    {{YG_UNDEFINED_VALUES, YG_UNDEFINED_VALUES}},
    YGUndefined,
};

static const YGLayout gYGNodeLayoutDefaults = {
    {},
    {{YGUndefined, YGUndefined}},
    {},
    {},
    {},
    YGDirectionInherit,
    0,
    YGUndefined,
    false,
    0,
    (YGDirection)-1,
    0,
    {},
    {{YGUndefined, YGUndefined}},
    
        {
            0,
            0,
            (YGMeasureMode)-1,
            (YGMeasureMode)-1,
            -1,
            -1,
        },
};

extern bool YGFloatsEqual(const float a, const float b);
extern bool YGValueEqual(const YGValue a, const YGValue b);
extern const YGValue* YGComputedEdgeValue(
    const std::array<YGValue, YGEdgeCount>& edges,
    const YGEdge edge,
    const YGValue* const defaultValue);
