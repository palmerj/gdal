/******************************************************************************
 *
 * Project:  GDAL Utilities
 * Purpose:  Convert nearly black or nearly white border to exact black/white.
 * Authors:  Even Rouault, <even dot rouault at spatialys dot com>
 *
 ******************************************************************************
 * Copyright (c) 2015, Even Rouault <even dot rouault at spatialys dot com>
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

#include "cpl_string.h"
#include "gdal_version.h"
#include "commonutils.h"
#include "gdal_utils_priv.h"


/************************************************************************/
/*                               Usage()                                */
/************************************************************************/

static void Usage( const char* pszErrorMsg = nullptr )
{
    printf("nearblack [-of format] [-white | [-color c1,c2,c3...cn]*] [-near dist] [-nb non_black_pixels]\n"
           "          [-setalpha] [-setmask] [-o outfile] [-q] [-co \"NAME=VALUE\"]* infile\n");

    if( pszErrorMsg != nullptr )
        fprintf(stderr, "\nFAILURE: %s\n", pszErrorMsg);

    exit(1);
}

/************************************************************************/
/*                       GDALNearblackOptionsForBinaryNew()             */
/************************************************************************/

static GDALNearblackOptionsForBinary *GDALNearblackOptionsForBinaryNew()
{
    return static_cast<GDALNearblackOptionsForBinary *>(
        CPLCalloc(1, sizeof(GDALNearblackOptionsForBinary)));
}

/************************************************************************/
/*                       GDALNearblackOptionsForBinaryFree()            */
/************************************************************************/

static void GDALNearblackOptionsForBinaryFree(
    GDALNearblackOptionsForBinary* psOptionsForBinary )
{
    if( psOptionsForBinary )
    {
        CPLFree(psOptionsForBinary->pszInFile);
        CPLFree(psOptionsForBinary->pszOutFile);
        CPLFree(psOptionsForBinary);
    }
}
/************************************************************************/
/*                                main()                                */
/************************************************************************/

MAIN_START(argc, argv)
{
    /* Check strict compilation and runtime library version as we use C++ API */
    if( !GDAL_CHECK_VERSION(argv[0]) )
        exit(1);

    EarlySetConfigOptions(argc, argv);

/* -------------------------------------------------------------------- */
/*      Generic arg processing.                                         */
/* -------------------------------------------------------------------- */
    GDALAllRegister();

    if( CPLGetConfigOption("GDAL_CACHEMAX", nullptr) == nullptr )
        GDALSetCacheMax( 100000000 );
    argc = GDALGeneralCmdLineProcessor(argc, &argv, 0);
    if( argc < 1 )
        exit( -argc );

    for( int i = 0; i < argc; i++ )
    {
        if( EQUAL(argv[i], "--utility_version") )
        {
            printf("%s was compiled against GDAL %s and "
                   "is running against GDAL %s\n",
                   argv[0], GDAL_RELEASE_NAME, GDALVersionInfo("RELEASE_NAME"));
            CSLDestroy(argv);
            return 0;
        }
        else if( EQUAL(argv[i], "--help") )
        {
            Usage();
        }
    }

    GDALNearblackOptionsForBinary* psOptionsForBinary =
        GDALNearblackOptionsForBinaryNew();
    GDALNearblackOptions *psOptions =
        GDALNearblackOptionsNew(argv + 1, psOptionsForBinary);
    CSLDestroy(argv);

    if( psOptions == nullptr )
    {
        Usage();
    }

    if( !(psOptionsForBinary->bQuiet) )
    {
        GDALNearblackOptionsSetProgress(psOptions, GDALTermProgress, nullptr);
    }

    if( psOptionsForBinary->pszInFile == nullptr )
        Usage("No input file specified.");

    if( psOptionsForBinary->pszOutFile == nullptr )
        psOptionsForBinary->pszOutFile =
            CPLStrdup(psOptionsForBinary->pszInFile);

/* -------------------------------------------------------------------- */
/*      Open input file.                                                */
/* -------------------------------------------------------------------- */
    GDALDatasetH hInDS = nullptr;
    GDALDatasetH hOutDS = nullptr;
    bool bCloseRetDS = false;

    if( strcmp(psOptionsForBinary->pszOutFile,
               psOptionsForBinary->pszInFile) == 0 )
    {
        hInDS = GDALOpen(psOptionsForBinary->pszInFile, GA_Update);
        hOutDS = hInDS;
    }
    else
    {
        hInDS = GDALOpen(psOptionsForBinary->pszInFile, GA_ReadOnly);
        bCloseRetDS = true;
    }

    if( hInDS == nullptr )
        exit(1);

    int bUsageError = FALSE;
    GDALDatasetH hRetDS = GDALNearblack(psOptionsForBinary->pszOutFile,
                                        hOutDS,
                                        hInDS,
                                        psOptions, &bUsageError);
    if( bUsageError )
        Usage();
    const int nRetCode = hRetDS ? 0 : 1;

    GDALClose(hInDS);
    if( bCloseRetDS )
        GDALClose(hRetDS);
    GDALNearblackOptionsFree(psOptions);
    GDALNearblackOptionsForBinaryFree(psOptionsForBinary);

    GDALDestroyDriverManager();

    return nRetCode;
}
MAIN_END
