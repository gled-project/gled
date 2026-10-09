// FTGL 2.1.2 as forked by ROOT (graf3d/ftgl of ROOT 6.28.06): FTGL by Henry
// Maddocks, with ROOT's FTFont::Render(text, w_max, w_fade) fade-out and
// PreRender()/PostRender().
//
// The fork's 27 headers and 22 sources, joined into FTGL.h and FTGL.cxx, with
// FTGL's declarations in namespace gled so that they cannot collide with the
// FTGL that ROOT's libRGL exports. The original files are in
// ftgl-2.1.2-gled.tar.gz in the cache of rsync://rsync.gled.org.
//
// FTGL's license, from its COPYING:
//
// FTGL
//
// Herewith is a license. Basically I want you to use this software and if
// you think this license is preventing you from doing so let me know.
//
// Copyright (C) 2001-3 Henry Maddocks
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "FTGL.h"

namespace gled {

// ---- FTBitmapGlyph.cxx
FTBitmapGlyph::FTBitmapGlyph( FT_GlyphSlot glyph)
:   FTGlyph( glyph),
    destWidth(0),
    destHeight(0),
    data(0)
{
    err = FT_Render_Glyph( glyph, FT_RENDER_MODE_MONO);
    if( err || ft_glyph_format_bitmap != glyph->format)
    {
        return;
    }

    FT_Bitmap bitmap = glyph->bitmap;

    unsigned int srcWidth = bitmap.width;
    unsigned int srcHeight = bitmap.rows;
    unsigned int srcPitch = bitmap.pitch;

    destWidth = srcWidth;
    destHeight = srcHeight;
    destPitch = srcPitch;

    if( destWidth && destHeight)
    {
        data = new unsigned char[destPitch * destHeight];
        unsigned char* dest = data + (( destHeight - 1) * destPitch);

        unsigned char* src = bitmap.buffer;

        for( unsigned int y = 0; y < srcHeight; ++y)
        {
            memcpy( dest, src, srcPitch);
            dest -= destPitch;
            src += srcPitch;
        }
    }

    pos = FTPoint(glyph->bitmap_left, static_cast<int>(srcHeight) - glyph->bitmap_top, 0.0);
}


FTBitmapGlyph::~FTBitmapGlyph()
{
    delete [] data;
}


const FTPoint& FTBitmapGlyph::Render( const FTPoint& pen)
{
    glBitmap( 0, 0, 0.0f, 0.0f, pen.X() + pos.X(), pen.Y() - pos.Y(), (const GLubyte*)0 );

    if( data)
    {
        glPixelStorei( GL_UNPACK_ROW_LENGTH, destPitch * 8);
        glBitmap( destWidth, destHeight, 0.0f, 0.0, 0.0, 0.0, (const GLubyte*)data);
    }

    glBitmap( 0, 0, 0.0f, 0.0f, -pos.X(), pos.Y(), (const GLubyte*)0 );

    return advance;
}

// ---- FTCharmap.cxx
FTCharmap::FTCharmap( FTFace* face)
:   ftFace( *(face->Face())),
    err(0)
{
    if( !ftFace->charmap)
    {
        err = FT_Set_Charmap( ftFace, ftFace->charmaps[0]);
    }

    ftEncoding = ftFace->charmap->encoding;
}


FTCharmap::~FTCharmap()
{
    charMap.clear();
}


bool FTCharmap::CharMap( FT_Encoding encoding)
{
    if( ftEncoding == encoding)
    {
        return true;
    }

    err = FT_Select_Charmap( ftFace, encoding );

    if( !err)
    {
        ftEncoding = encoding;
    }
    else
    {
        ftEncoding = ft_encoding_none;
    }

    charMap.clear();
    return !err;
}


unsigned int FTCharmap::GlyphListIndex( unsigned int characterCode )
{
    return charMap.find( characterCode);
}


unsigned int FTCharmap::FontIndex( unsigned int characterCode )
{
    return FT_Get_Char_Index( ftFace, characterCode);
}


void FTCharmap::InsertIndex( const unsigned int characterCode, const unsigned int containerIndex)
{
    charMap.insert( characterCode, containerIndex);
}

// ---- FTContour.cxx
static const float BEZIER_STEP_SIZE = 0.2f;


void FTContour::AddPoint( FTPoint point)
{
    if( pointList.empty() || point != pointList[pointList.size() - 1])
    {
        pointList.push_back( point);
    }
}


void FTContour::AddPoint( float x, float y)
{
    AddPoint( FTPoint( x, y, 0.0f));
}


void FTContour::evaluateQuadraticCurve()
{
    for( unsigned int i = 0; i <= ( 1.0f / BEZIER_STEP_SIZE); i++)
    {
        float bezierValues[2][2];

        float t = static_cast<float>(i) * BEZIER_STEP_SIZE;

        bezierValues[0][0] = (1.0f - t) * controlPoints[0][0] + t * controlPoints[1][0];
        bezierValues[0][1] = (1.0f - t) * controlPoints[0][1] + t * controlPoints[1][1];

        bezierValues[1][0] = (1.0f - t) * controlPoints[1][0] + t * controlPoints[2][0];
        bezierValues[1][1] = (1.0f - t) * controlPoints[1][1] + t * controlPoints[2][1];

        bezierValues[0][0] = (1.0f - t) * bezierValues[0][0] + t * bezierValues[1][0];
        bezierValues[0][1] = (1.0f - t) * bezierValues[0][1] + t * bezierValues[1][1];

        AddPoint( bezierValues[0][0], bezierValues[0][1]);
    }
}

void FTContour::evaluateCubicCurve()
{
    for( unsigned int i = 0; i <= ( 1.0f / BEZIER_STEP_SIZE); i++)
    {
        float bezierValues[3][2];

        float t = static_cast<float>(i) * BEZIER_STEP_SIZE;

        bezierValues[0][0] = (1.0f - t) * controlPoints[0][0] + t * controlPoints[1][0];
        bezierValues[0][1] = (1.0f - t) * controlPoints[0][1] + t * controlPoints[1][1];

        bezierValues[1][0] = (1.0f - t) * controlPoints[1][0] + t * controlPoints[2][0];
        bezierValues[1][1] = (1.0f - t) * controlPoints[1][1] + t * controlPoints[2][1];

        bezierValues[2][0] = (1.0f - t) * controlPoints[2][0] + t * controlPoints[3][0];
        bezierValues[2][1] = (1.0f - t) * controlPoints[2][1] + t * controlPoints[3][1];

        bezierValues[0][0] = (1.0f - t) * bezierValues[0][0] + t * bezierValues[1][0];
        bezierValues[0][1] = (1.0f - t) * bezierValues[0][1] + t * bezierValues[1][1];

        bezierValues[1][0] = (1.0f - t) * bezierValues[1][0] + t * bezierValues[2][0];
        bezierValues[1][1] = (1.0f - t) * bezierValues[1][1] + t * bezierValues[2][1];

        bezierValues[0][0] = (1.0f - t) * bezierValues[0][0] + t * bezierValues[1][0];
        bezierValues[0][1] = (1.0f - t) * bezierValues[0][1] + t * bezierValues[1][1];

        AddPoint( bezierValues[0][0], bezierValues[0][1]);
    }
}


FTContour::FTContour( FT_Vector* contour, char* pointTags, unsigned int numberOfPoints)
{
    for( unsigned int pointIndex = 0; pointIndex < numberOfPoints; ++ pointIndex)
    {
        char pointTag = pointTags[pointIndex];

        if( pointTag == FT_Curve_Tag_On || numberOfPoints < 2)
        {
            AddPoint( contour[pointIndex].x, contour[pointIndex].y);
            continue;
        }

        FTPoint controlPoint( contour[pointIndex]);
        FTPoint previousPoint = ( 0 == pointIndex)
                                ? FTPoint( contour[numberOfPoints - 1])
                                : pointList[pointList.size() - 1];

        FTPoint nextPoint = ( pointIndex == numberOfPoints - 1)
                            ? pointList[0]
                            : FTPoint( contour[pointIndex + 1]);

        if( pointTag == FT_Curve_Tag_Conic)
        {
            char nextPointTag = ( pointIndex == numberOfPoints - 1)
                                ? pointTags[0]
                                : pointTags[pointIndex + 1];

            while( nextPointTag == FT_Curve_Tag_Conic)
            {
                nextPoint = ( controlPoint + nextPoint) * 0.5f;

                controlPoints[0][0] = previousPoint.X(); controlPoints[0][1] = previousPoint.Y();
                controlPoints[1][0] = controlPoint.X();  controlPoints[1][1] = controlPoint.Y();
                controlPoints[2][0] = nextPoint.X();     controlPoints[2][1] = nextPoint.Y();

                evaluateQuadraticCurve();
                ++pointIndex;

                previousPoint = nextPoint;
                controlPoint = FTPoint( contour[pointIndex]);
                nextPoint = ( pointIndex == numberOfPoints - 1)
                            ? pointList[0]
                            : FTPoint( contour[pointIndex + 1]);
                nextPointTag = ( pointIndex == numberOfPoints - 1)
                               ? pointTags[0]
                               : pointTags[pointIndex + 1];
            }

            controlPoints[0][0] = previousPoint.X(); controlPoints[0][1] = previousPoint.Y();
            controlPoints[1][0] = controlPoint.X();  controlPoints[1][1] = controlPoint.Y();
            controlPoints[2][0] = nextPoint.X();     controlPoints[2][1] = nextPoint.Y();

            evaluateQuadraticCurve();
            continue;
        }

        if( pointTag == FT_Curve_Tag_Cubic)
        {
            FTPoint controlPoint2 = nextPoint;

            FTPoint nextPoint1 = ( pointIndex == numberOfPoints - 2)
                                ? pointList[0]
                                : FTPoint( contour[pointIndex + 2]);

            controlPoints[0][0] = previousPoint.X(); controlPoints[0][1] = previousPoint.Y();
            controlPoints[1][0] = controlPoint.X();  controlPoints[1][1] = controlPoint.Y();
            controlPoints[2][0] = controlPoint2.X(); controlPoints[2][1] = controlPoint2.Y();
            controlPoints[3][0] = nextPoint1.X();     controlPoints[3][1] = nextPoint1.Y();

            evaluateCubicCurve();
            ++pointIndex;
            continue;
        }
    }
}

// ---- FTExtrdGlyph.cxx
FTExtrdGlyph::FTExtrdGlyph( FT_GlyphSlot glyph, float depth, bool useDisplayList1)
:   FTGlyph( glyph),
    glList(0)
{
    bBox.SetDepth( -depth);

    if( ft_glyph_format_outline != glyph->format)
    {
        err = 0x14; // Invalid_Outline
        return;
    }

    FTVectoriser vectoriser( glyph);
    if( ( vectoriser.ContourCount() < 1) || ( vectoriser.PointCount() < 3))
    {
        return;
    }

    unsigned int tesselationIndex;

    if(useDisplayList1)
    {
        glList = glGenLists(1);
        glNewList( glList, GL_COMPILE);
    }

    vectoriser.MakeMesh( 1.0);
    glNormal3d(0.0, 0.0, 1.0);

    unsigned int horizontalTextureScale = glyph->face->size->metrics.x_ppem * 64;
    unsigned int verticalTextureScale = glyph->face->size->metrics.y_ppem * 64;

    const FTMesh* mesh = vectoriser.GetMesh();
    for( tesselationIndex = 0; tesselationIndex < mesh->TesselationCount(); ++tesselationIndex)
    {
        const FTTesselation* subMesh = mesh->Tesselation( tesselationIndex);
        unsigned int polyonType = subMesh->PolygonType();

        glBegin( polyonType);
            for( unsigned int pointIndex = 0; pointIndex < subMesh->PointCount(); ++pointIndex)
            {
                FTPoint point = subMesh->Point(pointIndex);

                glTexCoord2f( point.X() / horizontalTextureScale,
                              point.Y() / verticalTextureScale);

                glVertex3f( point.X() / 64.0f,
                            point.Y() / 64.0f,
                            0.0f);
            }
        glEnd();
    }

    vectoriser.MakeMesh( -1.0);
    glNormal3d(0.0, 0.0, -1.0);

    mesh = vectoriser.GetMesh();
    for( tesselationIndex = 0; tesselationIndex < mesh->TesselationCount(); ++tesselationIndex)
    {
        const FTTesselation* subMesh = mesh->Tesselation( tesselationIndex);
        unsigned int polyonType = subMesh->PolygonType();

        glBegin( polyonType);
            for( unsigned int pointIndex = 0; pointIndex < subMesh->PointCount(); ++pointIndex)
            {
                ///FTPoint point = subMesh->Point(pointIndex);

                glTexCoord2f( subMesh->Point(pointIndex).X() / horizontalTextureScale,
                              subMesh->Point(pointIndex).Y() / verticalTextureScale);

                glVertex3f( subMesh->Point( pointIndex).X() / 64.0f,
                            subMesh->Point( pointIndex).Y() / 64.0f,
                            -depth);
            }
        glEnd();
    }

    int contourFlag = vectoriser.ContourFlag();

    for( size_t c = 0; c < vectoriser.ContourCount(); ++c)
    {
        const FTContour* contour = vectoriser.Contour(c);
        unsigned int numberOfPoints = contour->PointCount();

        glBegin( GL_QUAD_STRIP);
            for( unsigned int j = 0; j <= numberOfPoints; ++j)
            {
                unsigned int pointIndex = ( j == numberOfPoints) ? 0 : j;
                unsigned int nextPointIndex = ( pointIndex == numberOfPoints - 1) ? 0 : pointIndex + 1;

                FTPoint point = contour->Point(pointIndex);

                FTPoint normal = GetNormal( point, contour->Point(nextPointIndex));
                if(normal != FTPoint( 0.0f, 0.0f, 0.0f))
                {
                    glNormal3dv(static_cast<const FTGL_DOUBLE*>(normal));
                }

                if( contourFlag & ft_outline_reverse_fill)
                {
                    glTexCoord2f( point.X() / horizontalTextureScale,
                                  point.X() / verticalTextureScale);

                    glVertex3f( point.X() / 64.0f, point.Y() / 64.0f, 0.0f);
                    glVertex3f( point.X() / 64.0f, point.Y() / 64.0f, -depth);
                }
                else
                {
                    glTexCoord2f( point.X() / horizontalTextureScale,
                                  point.Y() / verticalTextureScale);

                    glVertex3f( point.X() / 64.0f, point.Y() / 64.0f, -depth);
                    glVertex3f( point.X() / 64.0f, point.Y() / 64.0f, 0.0f);
                }
            }
        glEnd();
    }

    if(useDisplayList1)
    {
        glEndList();
    }
}


FTExtrdGlyph::~FTExtrdGlyph()
{
    glDeleteLists( glList, 1);
}


const FTPoint& FTExtrdGlyph::Render( const FTPoint& pen)
{
    glTranslatef( pen.X(), pen.Y(), 0);

    if( glList)
    {
        glCallList( glList);
    }

    return advance;
}


FTPoint FTExtrdGlyph::GetNormal( const FTPoint &a, const FTPoint &b)
{
    float vectorX = a.X() - b.X();
    float vectorY = a.Y() - b.Y();

    float length = sqrt( vectorX * vectorX + vectorY * vectorY );

    if( length > 0.01f)
    {
        length = 1 / length;
    }
    else
    {
        length = 0.0f;
    }

    return FTPoint( -vectorY * length,
                     vectorX * length,
                     0.0f);
}

// ---- FTFace.cxx
FTFace::FTFace( const char* fontFilePath)
:   numGlyphs(0),
    fontEncodingList(0),
    err(0)
{
    const FT_Long DEFAULT_FACE_INDEX = 0;
    ftFace = new FT_Face;

    err = FT_New_Face( *FTLibrary::Instance().GetLibrary(), fontFilePath, DEFAULT_FACE_INDEX, ftFace);
    if (err) {
        delete ftFace;
        ftFace = 0;
    } else {
        numGlyphs = (*ftFace)->num_glyphs;
        hasKerningTable = FT_HAS_KERNING((*ftFace));
    }
}


FTFace::FTFace( const unsigned char *pBufferBytes, size_t bufferSizeInBytes)
:   numGlyphs(0),
    err(0)
{
    const FT_Long DEFAULT_FACE_INDEX = 0;
    ftFace = new FT_Face;

    err = FT_New_Memory_Face( *FTLibrary::Instance().GetLibrary(), (FT_Byte *)pBufferBytes, bufferSizeInBytes, DEFAULT_FACE_INDEX, ftFace);

    if (err) {
        delete ftFace;
        ftFace = 0;
    } else {
        numGlyphs = (*ftFace)->num_glyphs;
    }
}


FTFace::~FTFace()
{
    if (ftFace) {
        FT_Done_Face( *ftFace);
        delete ftFace;
        ftFace = 0;
    }
}


bool FTFace::Attach( const char* fontFilePath)
{
    err = FT_Attach_File( *ftFace, fontFilePath);
    return !err;
}


bool FTFace::Attach( const unsigned char *pBufferBytes, size_t bufferSizeInBytes)
{
    FT_Open_Args open;

    open.flags = FT_OPEN_MEMORY;
    open.memory_base = (FT_Byte *)pBufferBytes;
    open.memory_size = bufferSizeInBytes;

    err = FT_Attach_Stream( *ftFace, &open);
    return !err;
}


const FTSize& FTFace::Size( const unsigned int size, const unsigned int res)
{
    charSize.CharSize( ftFace, size, res, res);
    err = charSize.Error();

    return charSize;
}


unsigned int FTFace::CharMapCount()
{
    return (*ftFace)->num_charmaps;
}


FT_Encoding* FTFace::CharMapList()
{
    if (0 == fontEncodingList) {
        fontEncodingList = new FT_Encoding[CharMapCount()];
        for (size_t encodingIndex = 0; encodingIndex < CharMapCount(); ++encodingIndex) {
            fontEncodingList[encodingIndex] = (*ftFace)->charmaps[encodingIndex]->encoding;
        }
    }

    return fontEncodingList;
}


FTPoint FTFace::KernAdvance( unsigned int index1, unsigned int index2)
{
    float x, y;
    x = y = 0.0f;

    if (hasKerningTable && index1 && index2) {
        FT_Vector kernAdvance;
        kernAdvance.x = kernAdvance.y = 0;

        err = FT_Get_Kerning( *ftFace, index1, index2, ft_kerning_unfitted, &kernAdvance);
        if (!err) {
            x = static_cast<float>( kernAdvance.x) / 64.0f;
            y = static_cast<float>( kernAdvance.y) / 64.0f;
        }
    }

    return FTPoint( x, y, 0.0);
}


FT_GlyphSlot FTFace::Glyph( unsigned int index, FT_Int load_flags)
{
    err = FT_Load_Glyph( *ftFace, index, load_flags);
    if (err) return NULL;
    return (*ftFace)->glyph;
}

// ---- FTFont.cxx
FTFont::FTFont( const char* fontFilePath)
:   face( fontFilePath),
    useDisplayLists(true),
    preRenderCalled(false),
    glyphList(0)
{
    err = face.Error();
    if( err == 0)
    {
        glyphList = new FTGlyphContainer( &face);
    }
}


FTFont::FTFont( const unsigned char *pBufferBytes, size_t bufferSizeInBytes)
:   face( pBufferBytes, bufferSizeInBytes),
    glyphList(0)
{
    err = face.Error();
    if( err == 0)
    {
        glyphList = new FTGlyphContainer( &face);
    }
}


FTFont::~FTFont()
{
    delete glyphList;
}


bool FTFont::Attach( const char* fontFilePath)
{
    if( face.Attach( fontFilePath))
    {
        err = 0;
        return true;
    }
    else
    {
        err = face.Error();
        return false;
    }
}


bool FTFont::Attach( const unsigned char *pBufferBytes, size_t bufferSizeInBytes)
{
    if( face.Attach( pBufferBytes, bufferSizeInBytes))
    {
        err = 0;
        return true;
    }
    else
    {
        err = face.Error();
        return false;
    }
}


bool FTFont::FaceSize( const unsigned int size, const unsigned int res )
{
    charSize = face.Size( size, res);
    err = face.Error();

    if( err != 0)
    {
        return false;
    }

    if( glyphList != NULL)
    {
        delete glyphList;
    }

    glyphList = new FTGlyphContainer( &face);
    return true;
}


unsigned int FTFont::FaceSize() const
{
    return charSize.CharSize();
}


bool FTFont::CharMap( FT_Encoding encoding)
{
    bool result = glyphList->CharMap( encoding);
    err = glyphList->Error();
    return result;
}


unsigned int FTFont::CharMapCount()
{
    return face.CharMapCount();
}


FT_Encoding* FTFont::CharMapList()
{
    return face.CharMapList();
}


void FTFont::UseDisplayList( bool useList)
{
    useDisplayLists = useList;
}

float FTFont::Ascender() const
{
    return charSize.Ascender();
}


float FTFont::Descender() const
{
    return charSize.Descender();
}

float FTFont::LineHeight() const
{
    return charSize.Height();
}

void FTFont::BBox( const char* string,
                   float& llx, float& lly, float& llz, float& urx, float& ury, float& urz)
{
    FTBBox totalBBox;

    if((NULL != string) && ('\0' != *string))
    {
        const unsigned char* c = (unsigned char*)string;
        float advance = 0;

        if(CheckGlyph( *c))
        {
            totalBBox = glyphList->BBox( *c);
            advance = glyphList->Advance( *c, *(c + 1));
        }

        while( *++c)
        {
            if(CheckGlyph( *c))
            {
                FTBBox tempBBox = glyphList->BBox( *c);
                tempBBox.Move( FTPoint( advance, 0.0f, 0.0f));
                totalBBox += tempBBox;
                advance += glyphList->Advance( *c, *(c + 1));
            }
        }
    }

    llx = totalBBox.lowerX;
    lly = totalBBox.lowerY;
    llz = totalBBox.lowerZ;
    urx = totalBBox.upperX;
    ury = totalBBox.upperY;
    urz = totalBBox.upperZ;
}


void FTFont::BBox( const wchar_t* string,
                   float& llx, float& lly, float& llz, float& urx, float& ury, float& urz)
{
    FTBBox totalBBox;

    if((NULL != string) && ('\0' != *string))
    {
        const wchar_t* c = string;
        float advance = 0;

        if(CheckGlyph( *c))
        {
            totalBBox = glyphList->BBox( *c);
            advance = glyphList->Advance( *c, *(c + 1));
        }

        while( *++c)
        {
            if(CheckGlyph( *c))
            {
                FTBBox tempBBox = glyphList->BBox( *c);
                tempBBox.Move( FTPoint( advance, 0.0f, 0.0f));
                totalBBox += tempBBox;
                advance += glyphList->Advance( *c, *(c + 1));
            }
        }
    }

    llx = totalBBox.lowerX;
    lly = totalBBox.lowerY;
    llz = totalBBox.lowerZ;
    urx = totalBBox.upperX;
    ury = totalBBox.upperY;
    urz = totalBBox.upperZ;
}


float FTFont::Advance( const wchar_t* string)
{
    const wchar_t* c = string;
    float width = 0.0f;

    while( *c)
    {
        if(CheckGlyph( *c))
        {
            width += glyphList->Advance( *c, *(c + 1));
        }
        ++c;
    }

    return width;
}


float FTFont::Advance( const char* string)
{
    const unsigned char* c = (unsigned char*)string;
    float width = 0.0f;

    while( *c)
    {
        if(CheckGlyph( *c))
        {
            width += glyphList->Advance( *c, *(c + 1));
        }
        ++c;
    }

    return width;
}


void FTFont::Render( const char* string )
{
    bool pre_post = ! preRenderCalled;
    if (pre_post) PreRender();

    const unsigned char* c = (unsigned char*)string;
    pen.X(0); pen.Y(0);

    while( *c)
    {
        if(CheckGlyph( *c))
        {
            pen = glyphList->Render( *c, *(c + 1), pen);
        }
        ++c;
    }

    if (pre_post) PostRender();
}


void FTFont::Render( const char* string, float w_max, float w_fade )
{
    bool pre_post = ! preRenderCalled;
    if (pre_post) PreRender();

    float col[4];
    glGetFloatv(GL_CURRENT_COLOR, col);
    float alpha_fac = col[3] / (w_max - w_fade);
    float w = 0;

    const unsigned char* c = (unsigned char*)string;
    pen.X(0); pen.Y(0);

   while( *c)
   {
      if(CheckGlyph( *c))
      {
         pen = glyphList->Render( *c, *(c + 1), pen);
         w += pen.X();
         if(w > w_max)
            break;
         if(w > w_fade)
         {
            col[3] = alpha_fac * (w_max - w);
            glColor4fv(col);
         }
      }
      ++c;
   }

    if (pre_post) PostRender();
}


void FTFont::Render( const wchar_t* string )
{
    bool pre_post = ! preRenderCalled;
    if (pre_post) PreRender();

    const wchar_t* c = string;
    pen.X(0); pen.Y(0);

    while( *c)
    {
        if(CheckGlyph( *c))
        {
            pen = glyphList->Render( *c, *(c + 1), pen);
        }
        ++c;
    }

    if (pre_post) PostRender();
}


bool FTFont::CheckGlyph( const unsigned int characterCode)
{
    if( NULL == glyphList->Glyph( characterCode))
    {
        unsigned int glyphIndex = glyphList->FontIndex( characterCode);
        FTGlyph* tempGlyph = MakeGlyph( glyphIndex);
        if( NULL == tempGlyph)
        {
            if( 0 == err)
            {
                err = 0x13;
            }

            return false;
        }
        glyphList->Add( tempGlyph, characterCode);
    }

    return true;
}

// ---- FTGLBitmapFont.cxx
FTGLBitmapFont::FTGLBitmapFont( const char* fontFilePath)
:   FTFont( fontFilePath)
{}


FTGLBitmapFont::FTGLBitmapFont( const unsigned char *pBufferBytes, size_t bufferSizeInBytes)
:   FTFont( pBufferBytes, bufferSizeInBytes)
{}


FTGLBitmapFont::~FTGLBitmapFont()
{}


FTGlyph* FTGLBitmapFont::MakeGlyph( unsigned int g)
{
    FT_GlyphSlot ftGlyph = face.Glyph( g, FT_LOAD_DEFAULT);

    if( ftGlyph)
    {
        FTBitmapGlyph* tempGlyph = new FTBitmapGlyph( ftGlyph);
        return tempGlyph;
    }

    err = face.Error();
    return NULL;
}


void FTGLBitmapFont::PreRender()
{
    FTFont::PreRender();
    glPushClientAttrib( GL_CLIENT_PIXEL_STORE_BIT);
    glPushAttrib( GL_ENABLE_BIT);

    glPixelStorei( GL_UNPACK_LSB_FIRST, GL_FALSE);
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1);

    glDisable( GL_BLEND);
}


void FTGLBitmapFont::PostRender()
{
    glPopAttrib();
    glPopClientAttrib();
    FTFont::PostRender();
}

// ---- FTGLExtrdFont.cxx
FTGLExtrdFont::FTGLExtrdFont( const char* fontFilePath)
:   FTFont( fontFilePath),
    depth( 0.0f)
{}


FTGLExtrdFont::FTGLExtrdFont( const unsigned char *pBufferBytes, size_t bufferSizeInBytes)
:   FTFont( pBufferBytes, bufferSizeInBytes),
    depth( 0.0f)
{}


FTGLExtrdFont::~FTGLExtrdFont()
{}


FTGlyph* FTGLExtrdFont::MakeGlyph( unsigned int glyphIndex)
{
    FT_GlyphSlot ftGlyph = face.Glyph( glyphIndex, FT_LOAD_NO_HINTING);

    if( ftGlyph)
    {
        FTExtrdGlyph* tempGlyph = new FTExtrdGlyph( ftGlyph, depth, useDisplayLists);
        return tempGlyph;
    }

    err = face.Error();
    return NULL;
}

// ---- FTGLOutlineFont.cxx
FTGLOutlineFont::FTGLOutlineFont( const char* fontFilePath)
:   FTFont( fontFilePath)
{}


FTGLOutlineFont::FTGLOutlineFont( const unsigned char *pBufferBytes, size_t bufferSizeInBytes)
:   FTFont( pBufferBytes, bufferSizeInBytes)
{}


FTGLOutlineFont::~FTGLOutlineFont()
{}


FTGlyph* FTGLOutlineFont::MakeGlyph( unsigned int g)
{
    FT_GlyphSlot ftGlyph = face.Glyph( g, FT_LOAD_NO_HINTING);

    if( ftGlyph)
    {
        FTOutlineGlyph* tempGlyph = new FTOutlineGlyph( ftGlyph, useDisplayLists);
        return tempGlyph;
    }

    err = face.Error();
    return NULL;
}


void FTGLOutlineFont::PreRender()
{
    FTFont::PreRender();
    glPushAttrib( GL_ENABLE_BIT | GL_HINT_BIT | GL_LINE_BIT | GL_COLOR_BUFFER_BIT);

    glDisable( GL_TEXTURE_2D);

    glEnable( GL_LINE_SMOOTH);
    glHint( GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
    glEnable(GL_BLEND);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // GL_ONE
}


void FTGLOutlineFont::PostRender()
{
    glPopAttrib();
    FTFont::PostRender();
}

// ---- FTGLPixmapFont.cxx
FTGLPixmapFont::FTGLPixmapFont( const char* fontFilePath)
:   FTFont( fontFilePath)
{}


FTGLPixmapFont::FTGLPixmapFont( const unsigned char *pBufferBytes, size_t bufferSizeInBytes)
:   FTFont( pBufferBytes, bufferSizeInBytes)
{}


FTGLPixmapFont::~FTGLPixmapFont()
{}


FTGlyph* FTGLPixmapFont::MakeGlyph( unsigned int g)
{
    FT_GlyphSlot ftGlyph = face.Glyph( g, FT_LOAD_NO_HINTING);

    if( ftGlyph)
    {
        FTPixmapGlyph* tempGlyph = new FTPixmapGlyph( ftGlyph);
        return tempGlyph;
    }

    err = face.Error();
    return NULL;
}


void FTGLPixmapFont::PreRender()
{
    FTFont::PreRender();
    glPushAttrib( GL_ENABLE_BIT | GL_PIXEL_MODE_BIT | GL_COLOR_BUFFER_BIT);
    glPushClientAttrib( GL_CLIENT_PIXEL_STORE_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GEQUAL, 0.0625);

    glDisable(GL_TEXTURE_2D);

    GLfloat ftglColour[4];
    glGetFloatv( GL_CURRENT_RASTER_COLOR, ftglColour);

    glPixelTransferf(GL_RED_SCALE, ftglColour[0]);
    glPixelTransferf(GL_GREEN_SCALE, ftglColour[1]);
    glPixelTransferf(GL_BLUE_SCALE, ftglColour[2]);
    glPixelTransferf(GL_ALPHA_SCALE, ftglColour[3]);
}


void FTGLPixmapFont::PostRender()
{
    glPopClientAttrib();
    glPopAttrib();
    FTFont::PostRender();
}

// ---- FTGLPolygonFont.cxx
FTGLPolygonFont::FTGLPolygonFont( const char* fontFilePath)
:   FTFont( fontFilePath)
{}


FTGLPolygonFont::FTGLPolygonFont( const unsigned char *pBufferBytes, size_t bufferSizeInBytes)
:   FTFont( pBufferBytes, bufferSizeInBytes)
{}


FTGLPolygonFont::~FTGLPolygonFont()
{}


FTGlyph* FTGLPolygonFont::MakeGlyph( unsigned int g)
{
    FT_GlyphSlot ftGlyph = face.Glyph( g, FT_LOAD_NO_HINTING);

    if( ftGlyph)
    {
        FTPolyGlyph* tempGlyph = new FTPolyGlyph( ftGlyph, useDisplayLists);
        return tempGlyph;
    }

    err = face.Error();
    return NULL;
}

// ---- FTGLTextureFont.cxx
inline GLuint NextPowerOf2( GLuint in)
{
     in -= 1;

     in |= in >> 16;
     in |= in >> 8;
     in |= in >> 4;
     in |= in >> 2;
     in |= in >> 1;

     return in + 1;
}


FTGLTextureFont::FTGLTextureFont( const char* fontFilePath)
:   FTFont( fontFilePath),
    maximumGLTextureSize(0),
    textureWidth(0),
    textureHeight(0),
    glyphHeight(0),
    glyphWidth(0),
    padding(3),
    xOffset(0),
    yOffset(0)
{
    remGlyphs = numGlyphs = face.GlyphCount();
}


FTGLTextureFont::FTGLTextureFont( const unsigned char *pBufferBytes, size_t bufferSizeInBytes)
:   FTFont( pBufferBytes, bufferSizeInBytes),
    maximumGLTextureSize(0),
    textureWidth(0),
    textureHeight(0),
    glyphHeight(0),
    glyphWidth(0),
    padding(3),
    xOffset(0),
    yOffset(0)
{
    remGlyphs = numGlyphs = face.GlyphCount();
}


FTGLTextureFont::~FTGLTextureFont()
{
    glDeleteTextures( textureIDList.size(), (const GLuint*)&textureIDList[0]);
}


FTGlyph* FTGLTextureFont::MakeGlyph( unsigned int glyphIndex)
{
    FT_GlyphSlot ftGlyph = face.Glyph( glyphIndex, FT_LOAD_NO_HINTING);

    if( ftGlyph)
    {
        glyphHeight = static_cast<int>( charSize.Height());
        glyphWidth = static_cast<int>( charSize.Width());

        if( textureIDList.empty())
        {
            textureIDList.push_back( CreateTexture());
            xOffset = yOffset = padding;
        }

        if( xOffset > ( textureWidth - glyphWidth))
        {
            xOffset = padding;
            yOffset += glyphHeight;

            if( yOffset > ( textureHeight - glyphHeight))
            {
                textureIDList.push_back( CreateTexture());
                yOffset = padding;
            }
        }

        FTTextureGlyph* tempGlyph = new FTTextureGlyph( ftGlyph, textureIDList[textureIDList.size() - 1],
                                                        xOffset, yOffset, textureWidth, textureHeight);
        xOffset += static_cast<int>( tempGlyph->BBox().upperX - tempGlyph->BBox().lowerX + padding);

        --remGlyphs;
        return tempGlyph;
    }

    err = face.Error();
    return NULL;
}


void FTGLTextureFont::CalculateTextureSize()
{
    if( !maximumGLTextureSize)
    {
        glGetIntegerv( GL_MAX_TEXTURE_SIZE, (GLint*)&maximumGLTextureSize);
        assert(maximumGLTextureSize); // If you hit this then you have an invalid OpenGL context.
    }

    textureWidth = NextPowerOf2( (remGlyphs * glyphWidth) + ( padding * 2));
    textureWidth = textureWidth > maximumGLTextureSize ? maximumGLTextureSize : textureWidth;

    int h = static_cast<int>( (textureWidth - ( padding * 2)) / glyphWidth);

    textureHeight = NextPowerOf2( (( numGlyphs / h) + 1) * glyphHeight);
    textureHeight = textureHeight > maximumGLTextureSize ? maximumGLTextureSize : textureHeight;
}


GLuint FTGLTextureFont::CreateTexture()
{
    CalculateTextureSize();

    int totalMemory = textureWidth * textureHeight;
    unsigned char* textureMemory = new unsigned char[totalMemory];
    memset( textureMemory, 0, totalMemory);

    GLuint textID;
    glGenTextures( 1, (GLuint*)&textID);

    glBindTexture( GL_TEXTURE_2D, textID);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA, textureWidth, textureHeight, 0, GL_ALPHA, GL_UNSIGNED_BYTE, textureMemory);

    delete [] textureMemory;

    return textID;
}


bool FTGLTextureFont::FaceSize( const unsigned int size, const unsigned int res)
{
    if( !textureIDList.empty())
    {
        glDeleteTextures( textureIDList.size(), (const GLuint*)&textureIDList[0]);
        textureIDList.clear();
        remGlyphs = numGlyphs = face.GlyphCount();
    }

    return FTFont::FaceSize( size, res);
}


void FTGLTextureFont::PreRender()
{
    FTFont::PreRender();
    glPushAttrib( GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // GL_ONE
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GEQUAL, 0.0625);

    FTTextureGlyph::ResetActiveTexture();
}


void FTGLTextureFont::PostRender()
{
    glPopAttrib();
    FTFont::PostRender();
}

// ---- FTGlyph.cxx
FTGlyph::FTGlyph( FT_GlyphSlot glyph, bool useList)
:   useDisplayList(useList),
    err(0)
{
    if( glyph)
    {
        bBox = FTBBox( glyph);
        advance = FTPoint( glyph->advance.x / 64.0f, glyph->advance.y / 64.0f, 0.0f);
    }
}


FTGlyph::~FTGlyph()
{}

// ---- FTGlyphContainer.cxx
FTGlyphContainer::FTGlyphContainer( FTFace* f)
:   face(f),
    err(0)
{
    glyphs.push_back( NULL);
    charMap = new FTCharmap( face);
}


FTGlyphContainer::~FTGlyphContainer()
{
    GlyphVector::iterator glyphIterator;
    for( glyphIterator = glyphs.begin(); glyphIterator != glyphs.end(); ++glyphIterator)
    {
        delete *glyphIterator;
    }

    glyphs.clear();
    delete charMap;
}


bool FTGlyphContainer::CharMap( FT_Encoding encoding)
{
    bool result = charMap->CharMap( encoding);
    err = charMap->Error();
    return result;
}


unsigned int FTGlyphContainer::FontIndex( const unsigned int characterCode) const
{
    return charMap->FontIndex( characterCode);
}


void FTGlyphContainer::Add( FTGlyph* tempGlyph, const unsigned int characterCode)
{
    charMap->InsertIndex( characterCode, glyphs.size());
    glyphs.push_back( tempGlyph);
}


FTGlyph* FTGlyphContainer::Glyph( const unsigned int characterCode) const
{
    signed int index = charMap->GlyphListIndex( characterCode);
    return glyphs[index];
}


FTBBox FTGlyphContainer::BBox( const unsigned int characterCode) const
{
    return glyphs[charMap->GlyphListIndex( characterCode)]->BBox();
}


float FTGlyphContainer::Advance( const unsigned int characterCode, const unsigned int nextCharacterCode)
{
    unsigned int left = charMap->FontIndex( characterCode);
    unsigned int right = charMap->FontIndex( nextCharacterCode);

    float width = face->KernAdvance( left, right).X();
    width += glyphs[charMap->GlyphListIndex( characterCode)]->Advance().X();

    return width;
}


FTPoint FTGlyphContainer::Render( const unsigned int characterCode, const unsigned int nextCharacterCode, FTPoint penPosition)
{
    FTPoint kernAdvance, advance;

    unsigned int left = charMap->FontIndex( characterCode);
    unsigned int right = charMap->FontIndex( nextCharacterCode);

    kernAdvance = face->KernAdvance( left, right);

    if( !face->Error())
    {
        advance = glyphs[charMap->GlyphListIndex( characterCode)]->Render( penPosition);
    }

    kernAdvance += advance;
    return kernAdvance;
}

// ---- FTLibrary.cxx
const FTLibrary&  FTLibrary::Instance()
{
    static FTLibrary ftlib;
    return ftlib;
}


FTLibrary::~FTLibrary()
{
    if( library != 0)
    {
        FT_Done_FreeType( *library);

        delete library;
        library= 0;
    }

//  if( manager != 0)
//  {
//      FTC_Manager_Done( manager );
//
//      delete manager;
//      manager= 0;
//  }
}


FTLibrary::FTLibrary()
:   library(0),
    err(0)
{
    Initialise();
}


bool FTLibrary::Initialise()
{
    if( library != 0)
        return true;

    library = new FT_Library;

    err = FT_Init_FreeType( library);
    if( err)
    {
        delete library;
        library = 0;
        return false;
    }

//  FTC_Manager* manager;
//
//  if( FTC_Manager_New( lib, 0, 0, 0, my_face_requester, 0, manager )
//  {
//      delete manager;
//      manager= 0;
//      return false;
//  }

    return true;
}

// ---- FTOutlineGlyph.cxx
FTOutlineGlyph::FTOutlineGlyph( FT_GlyphSlot glyph, bool useDisplayList1)
:   FTGlyph( glyph),
    glList(0)
{
    if( ft_glyph_format_outline != glyph->format)
    {
        err = 0x14; // Invalid_Outline
        return;
    }

    FTVectoriser vectoriser( glyph);

    size_t numContours = vectoriser.ContourCount();
    if ( ( numContours < 1) || ( vectoriser.PointCount() < 3))
    {
        return;
    }

    if(useDisplayList1)
    {
        glList = glGenLists(1);
        glNewList( glList, GL_COMPILE);
    }

    for( unsigned int c = 0; c < numContours; ++c)
    {
        const FTContour* contour = vectoriser.Contour(c);

        glBegin( GL_LINE_LOOP);
            for( unsigned int pointIndex = 0; pointIndex < contour->PointCount(); ++pointIndex)
            {
                FTPoint point = contour->Point(pointIndex);
                glVertex2f( point.X() / 64.0f, point.Y() / 64.0f);
            }
        glEnd();
    }

    if(useDisplayList1)
    {
        glEndList();
    }
}


FTOutlineGlyph::~FTOutlineGlyph()
{
    glDeleteLists( glList, 1);
}


const FTPoint& FTOutlineGlyph::Render( const FTPoint& pen)
{
    glTranslatef( pen.X(), pen.Y(), 0.0f);

    if( glList)
    {
        glCallList( glList);
    }

    return advance;
}

// ---- FTPixmapGlyph.cxx
FTPixmapGlyph::FTPixmapGlyph( FT_GlyphSlot glyph)
:   FTGlyph( glyph),
    destWidth(0),
    destHeight(0),
    data(0)
{
    err = FT_Render_Glyph( glyph, FT_RENDER_MODE_NORMAL);
    if( err || ft_glyph_format_bitmap != glyph->format)
    {
        return;
    }

    FT_Bitmap bitmap = glyph->bitmap;

    //check the pixel mode
    //ft_pixel_mode_grays

    int srcWidth = bitmap.width;
    int srcHeight = bitmap.rows;

    destWidth = srcWidth;
    destHeight = srcHeight;

    if( destWidth && destHeight)
    {
        data = new unsigned char[destWidth * destHeight * 2];
        unsigned char* src = bitmap.buffer;

        unsigned char* dest = data + ((destHeight - 1) * destWidth * 2);
        size_t destStep = destWidth * 2 * 2;

        for( int y = 0; y < srcHeight; ++y)
        {
            for( int x = 0; x < srcWidth; ++x)
            {
                *dest++ = static_cast<unsigned char>(255);
                *dest++ = *src++;
            }
            dest -= destStep;
        }

        destHeight = srcHeight;
    }

    pos.X(glyph->bitmap_left);
    pos.Y(srcHeight - glyph->bitmap_top);
}


FTPixmapGlyph::~FTPixmapGlyph()
{
    delete [] data;
}


const FTPoint& FTPixmapGlyph::Render( const FTPoint& pen)
{
    glBitmap( 0, 0, 0.0f, 0.0f, pen.X() + pos.X(), pen.Y() - pos.Y(), (const GLubyte*)0);

    if( data)
    {
        glPixelStorei( GL_UNPACK_ROW_LENGTH, 0);
        glPixelStorei( GL_UNPACK_ALIGNMENT, 2);

        glDrawPixels( destWidth, destHeight, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, (const GLvoid*)data);
    }

    glBitmap( 0, 0, 0.0f, 0.0f, -pos.X(), pos.Y(), (const GLubyte*)0);

    return advance;
}

// ---- FTPoint.cxx
bool operator == ( const FTPoint &a, const FTPoint &b)
{
    return((a.values[0] == b.values[0]) && (a.values[1] == b.values[1]) && (a.values[2] == b.values[2]));
}

bool operator != ( const FTPoint &a, const FTPoint &b)
{
    return((a.values[0] != b.values[0]) || (a.values[1] != b.values[1]) || (a.values[2] != b.values[2]));
}


FTPoint operator*( double multiplier, FTPoint& point)
{
    return point * multiplier;
}

// ---- FTPolyGlyph.cxx
FTPolyGlyph::FTPolyGlyph( FT_GlyphSlot glyph, bool useDisplayList1)
:   FTGlyph( glyph),
    glList(0)
{
    if( ft_glyph_format_outline != glyph->format)
    {
        err = 0x14; // Invalid_Outline
        return;
    }

    FTVectoriser vectoriser( glyph);

    if(( vectoriser.ContourCount() < 1) || ( vectoriser.PointCount() < 3))
    {
        return;
    }

    unsigned int horizontalTextureScale = glyph->face->size->metrics.x_ppem * 64;
    unsigned int verticalTextureScale = glyph->face->size->metrics.y_ppem * 64;

    vectoriser.MakeMesh( 1.0);

    if( useDisplayList1)
    {
        glList = glGenLists( 1);
        glNewList( glList, GL_COMPILE);
    }

    const FTMesh* mesh = vectoriser.GetMesh();
    for( unsigned int index = 0; index < mesh->TesselationCount(); ++index)
    {
        const FTTesselation* subMesh = mesh->Tesselation( index);
        unsigned int polyonType = subMesh->PolygonType();

        glBegin( polyonType);
            for( unsigned int pointIndex = 0; pointIndex < subMesh->PointCount(); ++pointIndex)
            {
                FTPoint point = subMesh->Point(pointIndex);

                glTexCoord2f( point.X() / horizontalTextureScale,
                              point.Y() / verticalTextureScale);

                glVertex3f( point.X() / 64.0f,
                            point.Y() / 64.0f,
                            0.0f);
            }
        glEnd();
    }

    if(useDisplayList1)
    {
        glEndList();
    }
}


FTPolyGlyph::~FTPolyGlyph()
{
    glDeleteLists( glList, 1);
}


const FTPoint& FTPolyGlyph::Render( const FTPoint& pen)
{
    glTranslatef(  pen.X(),  pen.Y(), 0.0f);

    if( glList)
    {
        glCallList( glList);
    }

    return advance;
}

// ---- FTSize.cxx
FTSize::FTSize()
:   ftFace(0),
    ftSize(0),
    size(0),
    xResolution(0),
    yResolution(0),
    err(0)
{}


FTSize::~FTSize()
{}


bool FTSize::CharSize( FT_Face* face, unsigned int pointSize, unsigned int xRes, unsigned int yRes )
{
    if (size != pointSize || xResolution != xRes || yResolution != yRes) {
        err = FT_Set_Char_Size( *face, 0L, pointSize * 64, xResolution, yResolution);

        if (!err) {
            ftFace = face;
            size = pointSize;
            xResolution = xRes;
            yResolution = yRes;
            ftSize = (*ftFace)->size;
        } else {
            ftFace = 0;
            size = 0;
            xResolution = 0;
            yResolution = 0;
            ftSize = 0;
        }
    }

    return !err;
}


unsigned int FTSize::CharSize() const
{
    return size;
}


float FTSize::Ascender() const
{
    return ftSize == 0 ? 0.0f : static_cast<float>( ftSize->metrics.ascender) / 64.0f;
}


float FTSize::Descender() const
{
    return ftSize == 0 ? 0.0f : static_cast<float>( ftSize->metrics.descender) / 64.0f;
}


float FTSize::Height() const
{
    if (0 == ftSize) return 0.0f;

    if (FT_IS_SCALABLE((*ftFace))) {
        return ( (*ftFace)->bbox.yMax - (*ftFace)->bbox.yMin) * ( (float)ftSize->metrics.y_ppem / (float)(*ftFace)->units_per_EM);
    } else {
        return static_cast<float>( ftSize->metrics.height) / 64.0f;
    }
}


float FTSize::Width() const
{
    if( 0 == ftSize) return 0.0f;

    if( FT_IS_SCALABLE((*ftFace))) {
        return ( (*ftFace)->bbox.xMax - (*ftFace)->bbox.xMin) * ( static_cast<float>(ftSize->metrics.x_ppem) / static_cast<float>((*ftFace)->units_per_EM));
    } else {
        return static_cast<float>( ftSize->metrics.max_advance) / 64.0f;
    }
}


float FTSize::Underline() const
{
    return 0.0f;
}

// ---- FTTextureGlyph.cxx
GLint FTTextureGlyph::activeTextureID = 0;

FTTextureGlyph::FTTextureGlyph( FT_GlyphSlot glyph, int id, int xOffset, int yOffset, GLsizei width, GLsizei height)
:   FTGlyph( glyph),
    destWidth(0),
    destHeight(0),
    glTextureID(id)
{
    err = FT_Render_Glyph( glyph, FT_RENDER_MODE_NORMAL);
    if( err || glyph->format != ft_glyph_format_bitmap)
    {
        return;
    }

    FT_Bitmap      bitmap = glyph->bitmap;

    destWidth  = bitmap.width;
    destHeight = bitmap.rows;

    if( destWidth && destHeight)
    {
        glPushClientAttrib( GL_CLIENT_PIXEL_STORE_BIT);
        glPixelStorei( GL_UNPACK_LSB_FIRST, GL_FALSE);
        glPixelStorei( GL_UNPACK_ROW_LENGTH, 0);
        glPixelStorei( GL_UNPACK_ALIGNMENT, 1);

        glBindTexture( GL_TEXTURE_2D, glTextureID);
        glTexSubImage2D( GL_TEXTURE_2D, 0, xOffset, yOffset, destWidth, destHeight, GL_ALPHA, GL_UNSIGNED_BYTE, bitmap.buffer);

        glPopClientAttrib();
    }


//      0
//      +----+
//      |    |
//      |    |
//      |    |
//      +----+
//           1

    uv[0].X( static_cast<float>(xOffset) / static_cast<float>(width));
    uv[0].Y( static_cast<float>(yOffset) / static_cast<float>(height));
    uv[1].X( static_cast<float>( xOffset + destWidth) / static_cast<float>(width));
    uv[1].Y( static_cast<float>( yOffset + destHeight) / static_cast<float>(height));

    pos.X( glyph->bitmap_left);
    pos.Y( glyph->bitmap_top);
}


FTTextureGlyph::~FTTextureGlyph()
{}


const FTPoint& FTTextureGlyph::Render( const FTPoint& pen)
{
    if( activeTextureID != glTextureID)
    {
        glBindTexture( GL_TEXTURE_2D, (GLuint)glTextureID);
        activeTextureID = glTextureID;
    }

    glTranslatef( pen.X(),  pen.Y(), 0.0f);

    glBegin( GL_QUADS);
        glTexCoord2f( uv[0].X(), uv[0].Y());
        glVertex2f( pos.X(), pos.Y());

        glTexCoord2f( uv[0].X(), uv[1].Y());
        glVertex2f( pos.X(), pos.Y() - destHeight);

        glTexCoord2f( uv[1].X(), uv[1].Y());
        glVertex2f( destWidth + pos.X(), pos.Y() - destHeight);

        glTexCoord2f( uv[1].X(), uv[0].Y());
        glVertex2f( destWidth + pos.X(), pos.Y());
    glEnd();

    return advance;
}

// ---- FTVectoriser.cxx
#ifndef CALLBACK
#define CALLBACK
#endif

#if defined(__APPLE_CC__) && __APPLE_CC__ > 4000 && __APPLE_CC__ < 5450 && !defined(__INTEL_COMPILER)
    typedef GLvoid (*GLUTesselatorFunction)(...);
#elif defined( __mips ) || defined( __linux__ ) || defined( __FreeBSD__ ) || defined( __OpenBSD__ ) || defined( __sun ) || defined (__CYGWIN__) || defined (__APPLE__)
    typedef GLvoid (*GLUTesselatorFunction)();
#elif defined ( WIN32)
    typedef GLvoid (CALLBACK *GLUTesselatorFunction)( );
#else
    #error "Error - need to define type GLUTesselatorFunction for this platform/compiler"
#endif


void CALLBACK ftglError( GLenum errCode, FTMesh* mesh)
{
    mesh->Error( errCode);
}


void CALLBACK ftglVertex( void* data, FTMesh* mesh)
{
    FTGL_DOUBLE* vertex = static_cast<FTGL_DOUBLE*>(data);
    mesh->AddPoint( vertex[0], vertex[1], vertex[2]);
}


void CALLBACK ftglCombine( FTGL_DOUBLE coords[3], void* /*vertex_data*/[4], GLfloat /*weight*/[4], void** outData, FTMesh* mesh)
{
    const FTGL_DOUBLE* vertex = static_cast<const FTGL_DOUBLE*>(coords);
    *outData = const_cast<FTGL_DOUBLE*>(mesh->Combine( vertex[0], vertex[1], vertex[2]));
}


void CALLBACK ftglBegin( GLenum type, FTMesh* mesh)
{
    mesh->Begin( type);
}


void CALLBACK ftglEnd( FTMesh* mesh)
{
    mesh->End();
}


FTMesh::FTMesh()
: currentTesselation(0),
    err(0)
{
    tesselationList.reserve( 16);
}


FTMesh::~FTMesh()
{
    for( size_t t = 0; t < tesselationList.size(); ++t)
    {
        delete tesselationList[t];
    }

    tesselationList.clear();
}


void FTMesh::AddPoint( const FTGL_DOUBLE x, const FTGL_DOUBLE y, const FTGL_DOUBLE z)
{
    currentTesselation->AddPoint( x, y, z);
}


const FTGL_DOUBLE* FTMesh::Combine( const FTGL_DOUBLE x, const FTGL_DOUBLE y, const FTGL_DOUBLE z)
{
    tempPointList.push_back( FTPoint( x, y,z));
    return static_cast<const FTGL_DOUBLE*>(tempPointList.back());
}


void FTMesh::Begin( GLenum meshType)
{
    currentTesselation = new FTTesselation( meshType);
}


void FTMesh::End()
{
    tesselationList.push_back( currentTesselation);
}


const FTTesselation* FTMesh::Tesselation( unsigned int index) const
{
    return ( index < tesselationList.size()) ? tesselationList[index] : NULL;
}


FTVectoriser::FTVectoriser( const FT_GlyphSlot glyph)
:   contourList(0),
    mesh(0),
    ftContourCount(0),
    contourFlag(0)
{
    if( glyph)
    {
        outline = glyph->outline;

        ftContourCount = outline.n_contours;
        contourList = 0;
        contourFlag = outline.flags;

        ProcessContours();
    }
}


FTVectoriser::~FTVectoriser()
{
    for( size_t c = 0; c < ContourCount(); ++c)
    {
        delete contourList[c];
    }

    delete [] contourList;
    delete mesh;
}


void FTVectoriser::ProcessContours()
{
    short contourLength = 0;
    short startIndex = 0;
    short endIndex = 0;

    contourList = new FTContour*[ftContourCount];

    for( short contourIndex = 0; contourIndex < ftContourCount; ++contourIndex)
    {
        FT_Vector* pointList = &outline.points[startIndex];
        char* tagList = reinterpret_cast<char*>(&outline.tags[startIndex]); // FT_Outline::tags is unsigned char*

        endIndex = outline.contours[contourIndex];
        contourLength =  ( endIndex - startIndex) + 1;

        FTContour* contour = new FTContour( pointList, tagList, contourLength);

        contourList[contourIndex] = contour;

        startIndex = endIndex + 1;
    }
}


size_t FTVectoriser::PointCount()
{
    size_t s = 0;
    for( size_t c = 0; c < ContourCount(); ++c)
    {
        s += contourList[c]->PointCount();
    }

    return s;
}


const FTContour* FTVectoriser::Contour( unsigned int index) const
{
    return ( index < ContourCount()) ? contourList[index] : NULL;
}


void FTVectoriser::MakeMesh( FTGL_DOUBLE zNormal)
{
    if( mesh)
    {
        delete mesh;
    }

    mesh = new FTMesh;

    GLUtesselator* tobj = gluNewTess();

#if defined(__GNUC__) && __GNUC__ >= 8
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

    gluTessCallback( tobj, GLU_TESS_BEGIN_DATA,     (GLUTesselatorFunction)ftglBegin);
    gluTessCallback( tobj, GLU_TESS_VERTEX_DATA,    (GLUTesselatorFunction)ftglVertex);
    gluTessCallback( tobj, GLU_TESS_COMBINE_DATA,   (GLUTesselatorFunction)ftglCombine);
    gluTessCallback( tobj, GLU_TESS_END_DATA,       (GLUTesselatorFunction)ftglEnd);
    gluTessCallback( tobj, GLU_TESS_ERROR_DATA,     (GLUTesselatorFunction)ftglError);

#if defined(__GNUC__) && __GNUC__ >= 8
#pragma GCC diagnostic pop
#endif

    if( contourFlag & ft_outline_even_odd_fill) // ft_outline_reverse_fill
    {
        gluTessProperty( tobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);
    }
    else
    {
        gluTessProperty( tobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);
    }


    gluTessProperty( tobj, GLU_TESS_TOLERANCE, 0);
    gluTessNormal( tobj, 0.0f, 0.0f, zNormal);
    gluTessBeginPolygon( tobj, mesh);

        for( size_t c = 0; c < ContourCount(); ++c)
        {
            const FTContour* contour = contourList[c];

            gluTessBeginContour( tobj);

                for( size_t p = 0; p < contour->PointCount(); ++p)
                {
                    const FTGL_DOUBLE* d = contour->Point(p);
                    gluTessVertex( tobj, (GLdouble*)d, (GLdouble*)d);
                }

            gluTessEndContour( tobj);
        }

    gluTessEndPolygon( tobj);

    gluDeleteTess( tobj);
}

} // namespace gled
