#pragma once

struct Position {
    int metaBitPosition;
    int metaByteParameterPosition;
    int dataBytePosition;
    enum SegmentType segmentType;
};