/******************************************************************************
 *
 * Project:  OGR
 * Purpose:  WKB geometry related methods
 * Author:   Even Rouault <even dot rouault at spatialys.com>
 *
 ******************************************************************************
 * Copyright (c) 2022, Even Rouault <even dot rouault at spatialys.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#ifndef OGR_WKB_H_INCLUDED
#define OGR_WKB_H_INCLUDED

#include "cpl_port.h"

bool OGRWKBGetGeomType(const GByte* pabyWkb, size_t nWKBSize,
                       bool& bNeedSwap, uint32_t& nType);
bool OGRWKBPolygonGetArea(const GByte*& pabyWkb, size_t& nWKBSize, double& dfArea);
bool OGRWKBMultiPolygonGetArea(const GByte*& pabyWkb, size_t& nWKBSize, double& dfArea);

/** Modifies a PostGIS-style Extended WKB geometry to a regular WKB one.
 * pabyEWKB will be modified in place.
 * The return value will be either at the beginning of pabyEWKB or 4 bytes later,
 * and thus has the same lifetime of pabyEWKB.
 * The function returns in nWKBSizeOut the length of the returned WKB pointer.
 * pnSRIDOut may be NULL, or if not NULL, the function will return in it the SRID,
 * if present, or INT_MIN if not present.
 */
const GByte CPL_DLL *WKBFromEWKB( GByte *pabyEWKB, size_t nEWKBSize, size_t& nWKBSizeOut, int* pnSRIDOut );

#endif // OGR_WKB_H_INCLUDED
