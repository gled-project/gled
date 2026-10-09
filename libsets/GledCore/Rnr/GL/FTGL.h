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

#ifndef GledCore_FTGL_amalgamation_H
#define GledCore_FTGL_amalgamation_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_TRUETYPE_TABLES_H
#include <GL/glew.h>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <map>
#include <math.h>
#include <string>

namespace gled {

// ---- FTGL.h
#ifndef     __FTGL__
#define     __FTGL__


typedef double   FTGL_DOUBLE;
typedef float    FTGL_FLOAT;

// Fixes for deprecated identifiers in 2.1.5
#ifndef FT_OPEN_MEMORY
    #define FT_OPEN_MEMORY (FT_Open_Flags)1
#endif

#ifndef FT_RENDER_MODE_MONO
    #define FT_RENDER_MODE_MONO ft_render_mode_mono
#endif

#ifndef FT_RENDER_MODE_NORMAL
    #define FT_RENDER_MODE_NORMAL ft_render_mode_normal
#endif


#ifndef WIN32
    // Required for compatibility with glext.h style function definitions of
    // OpenGL extensions, such as in src/osg/Point.cpp.
    #ifndef APIENTRY
        #define APIENTRY
    #endif
#endif

// Compiler-specific conditional compilation
#ifdef _MSC_VER // MS Visual C++

    // Disable various warning.
    // 4786: template name too long
    #pragma warning( disable : 4251 )
    #pragma warning( disable : 4275 )
    #pragma warning( disable : 4786 )

    // The following definitions control how symbols are exported.
    // If the target is a static library ensure that FTGL_LIBRARY_STATIC
    // is defined. If building a dynamic library (ie DLL) ensure the
    // FTGL_LIBRARY macro is defined, as it will mark symbols for
    // export. If compiling a project to _use_ the _dynamic_ library
    // version of the library, no definition is required.
    #ifdef FTGL_LIBRARY_STATIC      // static lib - no special export required
    #  define FTGL_EXPORT
    #elif FTGL_LIBRARY              // dynamic lib - must export/import symbols appropriately.
    #  define FTGL_EXPORT //  __declspec(dllexport)
    #else
    #  define FTGL_EXPORT //  __declspec(dllimport)
    #endif

#else
    // Compiler that is not MS Visual C++.
    // Ensure that the export symbol is defined (and blank)
    #define FTGL_EXPORT
#endif

#endif  //  __FTGL__

// ---- FTPoint.h
#ifndef     __FTPoint__
#define     __FTPoint__



/**
 * FTPoint class is a basic 3 dimensional point or vector.
 */
class FTGL_EXPORT FTPoint
{
    public:
        /**
         * Default constructor. Point is set to zero.
         */
        FTPoint()
        {
            values[0] = 0;
            values[1] = 0;
            values[2] = 0;
        }

        /**
         * Constructor.
         *
         * @param x First component
         * @param y Second component
         * @param z Third component
         */
        FTPoint( const FTGL_DOUBLE x, const FTGL_DOUBLE y, const FTGL_DOUBLE z)
        {
            values[0] = x;
            values[1] = y;
            values[2] = z;
        }

        /**
         * Constructor. This converts an FT_Vector to an FT_Point
         *
         * @param ft_vector A freetype vector
         */
        FTPoint( const FT_Vector& ft_vector)
        {
            values[0] = ft_vector.x;
            values[1] = ft_vector.y;
            values[2] = 0;
        }

        /**
         * Operator += In Place Addition.
         *
         * @param point
         * @return this plus point.
         */
        FTPoint& operator += ( const FTPoint& point)
        {
            values[0] += point.values[0];
            values[1] += point.values[1];
            values[2] += point.values[2];

            return *this;
        }

        /**
         * Operator +
         *
         * @param point
         * @return this plus point.
         */
        FTPoint operator + ( const FTPoint& point)
        {
            FTPoint temp;
            temp.values[0] = values[0] + point.values[0];
            temp.values[1] = values[1] + point.values[1];
            temp.values[2] = values[2] + point.values[2];

            return temp;
        }


        /**
         * Operator *
         *
         * @param multiplier
         * @return <code>this</code> multiplied by <code>multiplier</code>.
         */
        FTPoint operator * ( double multiplier)
        {
            FTPoint temp;
            temp.values[0] = values[0] * multiplier;
            temp.values[1] = values[1] * multiplier;
            temp.values[2] = values[2] * multiplier;

            return temp;
        }


        /**
         * Operator *
         *
         * @param point
         * @param multiplier
         * @return <code>multiplier</code> multiplied by <code>point</code>.
         */
        friend FTPoint operator*( double multiplier, FTPoint& point);


        /**
         * Operator == Tests for eqaulity
         *
         * @param a
         * @param b
         * @return true if a & b are equal
         */
        friend bool operator == ( const FTPoint &a, const FTPoint &b);

        /**
         * Operator != Tests for non equality
         *
         * @param a
         * @param b
         * @return true if a & b are not equal
         */
        friend bool operator != ( const FTPoint &a, const FTPoint &b);


        /**
         * Cast to FTGL_DOUBLE*
         */
        operator const FTGL_DOUBLE*() const
        {
            return values;
        }


        /**
         * Setters
         */
        void X( FTGL_DOUBLE x) { values[0] = x;};
        void Y( FTGL_DOUBLE y) { values[1] = y;};
        void Z( FTGL_DOUBLE z) { values[2] = z;};


        /**
         * Getters
         */
        FTGL_DOUBLE X() const { return values[0];};
        FTGL_DOUBLE Y() const { return values[1];};
        FTGL_DOUBLE Z() const { return values[2];};

    private:
        /**
         * The point data
         */
        FTGL_DOUBLE values[3];
};

#endif  //  __FTPoint__

// ---- FTBBox.h
#ifndef     __FTBBox__
#define     __FTBBox__

//#include FT_GLYPH_H



/**
 * FTBBox is a convenience class for handling bounding boxes.
 */
class FTGL_EXPORT FTBBox
{
    public:
        /**
         * Default constructor. Bounding box is set to zero.
         */
        FTBBox()
        :   lowerX(0.0f),
            lowerY(0.0f),
            lowerZ(0.0f),
            upperX(0.0f),
            upperY(0.0f),
            upperZ(0.0f)
        {}

        /**
         * Constructor.
         */
        FTBBox( float lx, float ly, float lz, float ux, float uy, float uz)
        :   lowerX(lx),
            lowerY(ly),
            lowerZ(lz),
            upperX(ux),
            upperY(uy),
            upperZ(uz)
        {}

        /**
         * Constructor. Extracts a bounding box from a freetype glyph. Uses
         * the control box for the glyph. <code>FT_Glyph_Get_CBox()</code>
         *
         * @param glyph A freetype glyph
         */
        FTBBox( FT_GlyphSlot glyph)
        :   lowerX(0.0f),
            lowerY(0.0f),
            lowerZ(0.0f),
            upperX(0.0f),
            upperY(0.0f),
            upperZ(0.0f)
        {
            FT_BBox bbox;
            FT_Outline_Get_CBox( &(glyph->outline), &bbox);

            lowerX = static_cast<float>( bbox.xMin) / 64.0f;
            lowerY = static_cast<float>( bbox.yMin) / 64.0f;
            lowerZ = 0.0f;
            upperX = static_cast<float>( bbox.xMax) / 64.0f;
            upperY = static_cast<float>( bbox.yMax) / 64.0f;
            upperZ = 0.0f;

        }

        /**
         * Destructor
         */
        ~FTBBox()
        {}


        /**
         * Move the Bounding Box by a vector.
         *
         * @param distance The distance to move the bbox in 3D space.
         */
        FTBBox& Move( FTPoint distance)
        {
            lowerX += distance.X();
            lowerY += distance.Y();
            lowerZ += distance.Z();
            upperX += distance.X();
            upperY += distance.Y();
            upperZ += distance.Z();
            return *this;
        }

        FTBBox& operator += ( const FTBBox& bbox)
        {
            lowerX = bbox.lowerX < lowerX? bbox.lowerX: lowerX;
            lowerY = bbox.lowerY < lowerY? bbox.lowerY: lowerY;
            lowerZ = bbox.lowerZ < lowerZ? bbox.lowerZ: lowerZ;
            upperX = bbox.upperX > upperX? bbox.upperX: upperX;
            upperY = bbox.upperY > upperY? bbox.upperY: upperY;
            upperZ = bbox.upperZ > upperZ? bbox.upperZ: upperZ;

            return *this;
        }

        void SetDepth( float depth)
        {
            upperZ = lowerZ + depth;
        }


        /**
         * The bounds of the box
         */
        // Make these ftPoints & private
        float lowerX, lowerY, lowerZ, upperX, upperY, upperZ;
    protected:


    private:

};


#endif  //  __FTBBox__

// ---- FTGlyph.h
#ifndef     __FTGlyph__
#define     __FTGlyph__




/**
 * FTGlyph is the base class for FTGL glyphs.
 *
 * It provides the interface between Freetype glyphs and their openGL
 * renderable counterparts. This is an abstract class and derived classes
 * must implement the <code>render</code> function.
 *
 * @see FTGlyphContainer
 * @see FTBBox
 * @see FTPoint
 *
 */
class FTGL_EXPORT FTGlyph
{
    public:
        /**
         * Constructor
         *
         * @param glyph The Freetype glyph to be processed
         * @param useDisplayList Enable or disable the use of Display Lists for this glyph
         *                       <code>true</code> turns ON display lists.
         *                       <code>false</code> turns OFF display lists.
         */
        FTGlyph( FT_GlyphSlot glyph, bool useDisplayList = true);

        /**
         * Destructor
         */
        virtual ~FTGlyph();

        /**
         * Renders this glyph at the current pen position.
         *
         * @param pen   The current pen position.
         * @return      The advance distance for this glyph.
         */
        virtual const FTPoint& Render( const FTPoint& pen) = 0;

        /**
         * Return the advance width for this glyph.
         *
         * @return  advance width.
         */
        const FTPoint& Advance() const { return advance;}

        /**
         * Return the bounding box for this glyph.
         *
         * @return  bounding box.
         */
        const FTBBox& BBox() const { return bBox;}

        /**
         * Queries for errors.
         *
         * @return  The current error code.
         */
        FT_Error Error() const { return err;}

    protected:
        /**
         * The advance distance for this glyph
         */
        FTPoint advance;

        /**
         * The bounding box of this glyph.
         */
        FTBBox bBox;

        /**
         * Flag to enable or disable the use of Display Lists inside FTGL
         * <code>true</code> turns ON display lists.
         * <code>false</code> turns OFF display lists.
         */
        bool useDisplayList;

        /**
         * Current error code. Zero means no error.
         */
        FT_Error err;

    private:

};


#endif  //  __FTGlyph__

// ---- FTBitmapGlyph.h
#ifndef     __FTBitmapGlyph__
#define     __FTBitmapGlyph__





/**
 * FTBitmapGlyph is a specialisation of FTGlyph for creating bitmaps.
 *
 * It provides the interface between Freetype glyphs and their openGL
 * Renderable counterparts. This is an abstract class and derived classes
 * must implement the <code>Render</code> function.
 *
 * @see FTGlyphContainer
 *
 */
class FTGL_EXPORT FTBitmapGlyph : public FTGlyph
{
    public:
        /**
         * Constructor
         *
         * @param glyph The Freetype glyph to be processed
         */
        FTBitmapGlyph( FT_GlyphSlot glyph);

        /**
         * Destructor
         */
        virtual ~FTBitmapGlyph();

        /**
         * Renders this glyph at the current pen position.
         *
         * @param pen   The current pen position.
         * @return      The advance distance for this glyph.
         */
        virtual const FTPoint& Render( const FTPoint& pen) override;

    private:
        /**
         * The width of the glyph 'image'
         */
        unsigned int destWidth;

        /**
         * The height of the glyph 'image'
         */
        unsigned int destHeight;

        /**
         * The pitch of the glyph 'image'
         */
        unsigned int destPitch;

        /**
         * Vector from the pen position to the topleft corner of the bitmap
         */
        FTPoint pos;

        /**
         * Pointer to the 'image' data
         */
        unsigned char* data;

};


#endif  //  __FTBitmapGlyph__

// ---- FTCharToGlyphIndexMap.h
#ifndef    __FTCharToGlyphIndexMap__
#define    __FTCharToGlyphIndexMap__



/**
 * Provides a non-STL alternative to the STL std::map<unsigned long, unsigned long>
 * which maps character codes to glyph indices inside FTCharmap.
 *
 * Implementation:
 *   - NumberOfBuckets buckets are considered.
 *   - Each bucket has BucketSize entries.
 *   - When the glyph index for the character code C has to be stored, the
 *     bucket this character belongs to is found using 'C div BucketSize'.
 *     If this bucket has not been allocated yet, do it now.
 *     The entry in the bucked is found using 'C mod BucketSize'.
 *     If it is set to IndexNotFound, then the glyph entry has not been set.
 *   - Try to mimic the calls made to the STL map API.
 *
 * Caveats:
 *   - The glyph index is now a signed long instead of unsigned long, so
 *     the special value IndexNotFound (= -1) can be used to specify that the
 *     glyph index has not been stored yet.
 */
class FTGL_EXPORT FTCharToGlyphIndexMap
{
    public:

        typedef unsigned long CharacterCode;
        typedef signed long GlyphIndex;

        enum
        {
            NumberOfBuckets = 256,
            BucketSize = 256,
            IndexNotFound = -1
        };

        FTCharToGlyphIndexMap()
        {
            this->Indices = nullptr;
        }

        virtual ~FTCharToGlyphIndexMap()
        {
            if( this->Indices)
            {
                // Free all buckets
                this->clear();

                // Free main structure
                delete [] this->Indices;
                this->Indices = nullptr;
            }
        }

        void clear()
        {
            if(this->Indices)
            {
                for( int i = 0; i < FTCharToGlyphIndexMap::NumberOfBuckets; i++)
                {
                    if( this->Indices[i])
                    {
                        delete [] this->Indices[i];
                        this->Indices[i] = nullptr;
                    }
                }
            }
        }

        GlyphIndex find( CharacterCode c)
        {
            if( !this->Indices)
            {
                return 0;
            }

            // Find position of char code in buckets
            div_t pos = div( c, FTCharToGlyphIndexMap::BucketSize);

            if( !this->Indices[pos.quot])
            {
                return 0;
            }

            const FTCharToGlyphIndexMap::GlyphIndex *ptr = &this->Indices[pos.quot][pos.rem];
            if( *ptr == FTCharToGlyphIndexMap::IndexNotFound)
            {
                return 0;
            }

            return *ptr;
        }

        void insert( CharacterCode c, GlyphIndex g)
        {
            if( !this->Indices)
            {
                this->Indices = new GlyphIndex* [FTCharToGlyphIndexMap::NumberOfBuckets];
                for( int i = 0; i < FTCharToGlyphIndexMap::NumberOfBuckets; i++)
                {
                    this->Indices[i] = nullptr;
                }
            }

            // Find position of char code in buckets
            div_t pos = div(c, FTCharToGlyphIndexMap::BucketSize);

            // Allocate bucket if does not exist yet
            if( !this->Indices[pos.quot])
            {
                this->Indices[pos.quot] = new GlyphIndex [FTCharToGlyphIndexMap::BucketSize];
                for( int i = 0; i < FTCharToGlyphIndexMap::BucketSize; i++)
                {
                    this->Indices[pos.quot][i] = FTCharToGlyphIndexMap::IndexNotFound;
                }
            }

            this->Indices[pos.quot][pos.rem] = g;
        }

    private:
        GlyphIndex** Indices;
};


#endif  //  __FTCharToGlyphIndexMap__

// ---- FTCharmap.h
#ifndef     __FTCharmap__
#define     __FTCharmap__






/**
 * FTCharmap takes care of specifying the encoding for a font and mapping
 * character codes to glyph indices.
 *
 * It doesn't preprocess all indices, only on an as needed basis. This may
 * seem like a performance penalty but it is quicker than using the 'raw'
 * freetype calls and will save significant amounts of memory when dealing
 * with unicode encoding
 *
 * @see "Freetype 2 Documentation"
 *
 */

class FTFace;

class FTGL_EXPORT FTCharmap
{
    public:
        /**
         * Constructor
         */
        FTCharmap( FTFace* face);

        /**
         * Destructor
         */
        virtual ~FTCharmap();

        /**
         * Queries for the current character map code.
         *
         * @return  The current character map code.
         */
        FT_Encoding Encoding() const { return ftEncoding;}

        /**
         * Sets the character map for the face.
         * Valid encodings as at Freetype 2.0.4
         *      ft_encoding_none
         *      ft_encoding_symbol
         *      ft_encoding_unicode
         *      ft_encoding_latin_2
         *      ft_encoding_sjis
         *      ft_encoding_gb2312
         *      ft_encoding_big5
         *      ft_encoding_wansung
         *      ft_encoding_johab
         *      ft_encoding_adobe_standard
         *      ft_encoding_adobe_expert
         *      ft_encoding_adobe_custom
         *      ft_encoding_apple_roman
         *
         * @param encoding  the Freetype encoding symbol. See above.
         * @return          <code>true</code> if charmap was valid and set
         *                  correctly. If the requested encoding is
         *                  unavailable it will be set to ft_encoding_none.
         */
        bool CharMap( FT_Encoding encoding);

        /**
         * Get the FTGlyphContainer index of the input character.
         *
         * @param characterCode The character code of the requested glyph in
         *                      the current encoding eg apple roman.
         * @return      The FTGlyphContainer index for the character or zero
         *              if it wasn't found
         */
        unsigned int GlyphListIndex( const unsigned int characterCode);

        /**
         * Get the font glyph index of the input character.
         *
         * @param characterCode The character code of the requested glyph in
         *                      the current encoding eg apple roman.
         * @return      The glyph index for the character.
         */
        unsigned int FontIndex( const unsigned int characterCode);

        /**
         * Set the FTGlyphContainer index of the character code.
         *
         * @param characterCode  The character code of the requested glyph in
         *                       the current encoding eg apple roman.
         * @param containerIndex The index into the FTGlyphContainer of the
         *                       character code.
         */
        void InsertIndex( const unsigned int characterCode, const unsigned int containerIndex);

        /**
         * Queries for errors.
         *
         * @return  The current error code. Zero means no error.
         */
        FT_Error Error() const { return err;}

    private:
        /**
         * Current character map code.
         */
        FT_Encoding ftEncoding;

        /**
         * The current Freetype face.
         */
        const FT_Face ftFace;

        /**
         * A structure that maps glyph indices to character codes
         *
         * < character code, face glyph index>
         */
        typedef FTCharToGlyphIndexMap CharacterMap;
        CharacterMap charMap;

        /**
         * Current error code.
         */
        FT_Error err;

};


#endif  //  __FTCharmap__

// ---- FTVector.h
#ifndef    __FTVector__
#define    __FTVector__


/**
 * Provides a non-STL alternative to the STL vector
 */
template <typename FT_VECTOR_ITEM_TYPE>
class FTGL_EXPORT FTVector
{
    public:
        typedef FT_VECTOR_ITEM_TYPE value_type;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef value_type* iterator;
        typedef const value_type* const_iterator;
        typedef size_t size_type;

        FTVector()
        {
            Capacity = Size = 0;
            Items = nullptr;
        }


        virtual ~FTVector()
        {
            clear();
        }

        FTVector& operator =(const FTVector& v)
        {
            reserve(v.capacity());

            iterator ptr = begin();
            const_iterator vbegin = v.begin();
            const_iterator vend = v.end();

            while( vbegin != vend)
            {
                *ptr++ = *vbegin++;
            }

            Size = v.size();
            return *this;
        }

        size_type size() const
        {
            return Size;
        }

        size_type capacity() const
        {
            return Capacity;
        }

        iterator begin()
        {
            return Items;
        }

        const_iterator begin() const
        {
            return Items;
        }

        iterator end()
        {
            return begin() + size();
        }

        const_iterator end() const
        {
            return begin() + size();
        }

        bool empty() const
        {
            return size() == 0;
        }

        reference operator [](size_type pos)
        {
            return( *(begin() + pos));
        }

        const_reference operator []( size_type pos) const
        {
            return( *(begin() + pos));
        }

        void clear()
        {
            if(Capacity)
            {
                delete [] Items;
                Capacity = Size = 0;
                Items = nullptr;
            }
        }

        void reserve( size_type n)
        {
            if( capacity() < n)
            {
                expand(n);
            }
        }

        void push_back(const value_type& x)
        {
            if( size() == capacity())
            {
                expand();
            }

           ( *this)[size()] = x;
            ++Size;
        }

        void resize(size_type n, value_type x)
        {
            if( n == size())
            {
                return;
            }

            reserve(n);
            iterator begin1, end1;

            if( n >= Size)
            {
                begin1 = this->end();
                end1 = this->begin() + n;
            }
            else
            {
                begin1 = this->begin() + n;
                end1 = this->end();
            }

            while( begin1 != end1)
            {
                *begin1++ = x;
            }

            Size = n;
        }


    private:
        void expand(size_type capacity_hint = 0)
        {
            size_type new_capacity =( capacity() == 0) ? 256 : capacity()* 2;
            if( capacity_hint)
            {
                while( new_capacity < capacity_hint)
                {
                    new_capacity *= 2;
                }
            }

            value_type *new_items = new value_type[new_capacity];

            iterator begin2 = this->begin();
            iterator end2 = this->end();
            value_type *ptr = new_items;

            while( begin2 != end2)
            {
                *ptr++ = *begin2++;
            }

            if( Capacity)
            {
                delete [] Items;
            }

            Items = new_items;
            Capacity = new_capacity;
        }

        size_type Capacity;
        size_type Size;
        value_type* Items;
};

#endif  //  __FTVector__

// ---- FTContour.h
#ifndef     __FTContour__
#define     __FTContour__



/**
 * FTContour class is a container of points that describe a vector font
 * outline. It is used as a container for the output of the bezier curve
 * evaluator in FTVectoriser.
 *
 * @see FTOutlineGlyph
 * @see FTPolyGlyph
 * @see FTPoint
 */
class FTGL_EXPORT FTContour
{
    public:
        /**
         * Constructor
         *
         * @param contour
         * @param pointTags
         * @param numberOfPoints
         */
        FTContour( FT_Vector* contour, char* pointTags, unsigned int numberOfPoints);

        /**
         * Destructor
         */
        ~FTContour()
        {
            pointList.clear();
        }

        /**
         * Return a point at index.
         *
         * @param index of the point in the curve.
         * @return const point reference
         */
        const FTPoint& Point( unsigned int index) const { return pointList[index];}

        /**
         * How many points define this contour
         *
         * @return the number of points in this contour
         */
        size_t PointCount() const { return pointList.size();}

    private:
        /**
         * Add a point to this contour. This function tests for duplicate
         * points.
         *
         * @param point The point to be added to the contour.
         */
        inline void AddPoint( FTPoint point);

        inline void AddPoint( float x, float y);

        /**
         * De Casteljau (bezier) algorithm contributed by Jed Soane
         * Evaluates a quadratic or conic (second degree) curve
         */
        inline void evaluateQuadraticCurve();

        /**
         * De Casteljau (bezier) algorithm contributed by Jed Soane
         * Evaluates a cubic (third degree) curve
         */
        inline void evaluateCubicCurve();

        /**
         *  The list of points in this contour
         */
        typedef FTVector<FTPoint> PointVector;
        PointVector pointList;

        /**
         * 2D array storing values of de Casteljau algorithm.
         */
        float controlPoints[4][2];
};

#endif // __FTContour__

// ---- FTExtrdGlyph.h
#ifndef     __FTExtrdGlyph__
#define     __FTExtrdGlyph__



class FTVectoriser;

/**
 * FTExtrdGlyph is a specialisation of FTGlyph for creating tessellated
 * extruded polygon glyphs.
 *
 * @see FTGlyphContainer
 * @see FTVectoriser
 *
 */
class FTGL_EXPORT FTExtrdGlyph : public FTGlyph
{
    public:
        /**
         * Constructor. Sets the Error to Invalid_Outline if the glyph isn't an outline.
         *
         * @param glyph The Freetype glyph to be processed
         * @param depth The distance along the z axis to extrude the glyph
         * @param useDisplayList Enable or disable the use of Display Lists for this glyph
         *                       <code>true</code> turns ON display lists.
         *                       <code>false</code> turns OFF display lists.
         */
        FTExtrdGlyph( FT_GlyphSlot glyph, float depth, bool useDisplayList);

        /**
         * Destructor
         */
        virtual ~FTExtrdGlyph();

        /**
         * Renders this glyph at the current pen position.
         *
         * @param pen   The current pen position.
         * @return      The advance distance for this glyph.
         */
        virtual const FTPoint& Render( const FTPoint& pen) override;

    private:
        /**
         * Calculate the normal vector to 2 points. This is 2D and ignores
         * the z component. The normal will be normalised
         *
         * @param a
         * @param b
         * @return
         */
        FTPoint GetNormal( const FTPoint &a, const FTPoint &b);


        /**
         * OpenGL display list
         */
        GLuint glList;

};


#endif  //  __FTExtrdGlyph__

// ---- FTSize.h
#ifndef     __FTSize__
#define     __FTSize__






/**
 * FTSize class provides an abstraction layer for the Freetype Size.
 *
 * @see "Freetype 2 Documentation"
 *
 */
class FTGL_EXPORT FTSize
{
    public:
        /**
         * Default Constructor
         */
         FTSize();

        /**
         * Destructor
         */
        virtual ~FTSize();

        /**
         * Sets the char size for the current face.
         *
         * This doesn't guarantee that the size was set correctly. Clients
         * should check errors.
         *
         * @param face           Parent face for this size object
         * @param point_size     the face size in points (1/72 inch)
         * @param x_resolution   the horizontal resolution of the target device.
         * @param y_resolution   the vertical resolution of the target device.
         * @return          <code>true</code> if the size has been set. Clients should check Error() for more information if this function returns false()
         */
        bool CharSize( FT_Face* face, unsigned int point_size, unsigned int x_resolution, unsigned int y_resolution);

        /**
         * get the char size for the current face.
         *
         * @return The char size in points
         */
        unsigned int CharSize() const;

        /**
         * Gets the global ascender height for the face in pixels.
         *
         * @return  Ascender height
         */
        float Ascender() const;

        /**
         * Gets the global descender height for the face in pixels.
         *
         * @return  Ascender height
         */
        float Descender() const;

        /**
         * Gets the global face height for the face.
         *
         * If the face is scalable this returns the height of the global
         * bounding box which ensures that any glyph will be less than or
         * equal to this height. If the font isn't scalable there is no
         * guarantee that glyphs will not be taller than this value.
         *
         * @return  height in pixels.
         */
        float Height() const;

        /**
         * Gets the global face width for the face.
         *
         * If the face is scalable this returns the width of the global
         * bounding box which ensures that any glyph will be less than or
         * equal to this width. If the font isn't scalable this value is
         * the max_advance for the face.
         *
         * @return  width in pixels.
         */
        float Width() const;

        /**
         * Gets the underline position for the face.
         *
         * @return  underline position in pixels
         */
        float Underline() const;

        /**
         * Queries for errors.
         *
         * @return  The current error code.
         */
        FT_Error Error() const { return err; }

    private:
        /**
         * The current Freetype face that this FTSize object relates to.
         */
        FT_Face* ftFace;

        /**
         *  The Freetype size.
         */
        FT_Size ftSize;

        /**
         *  The size in points.
         */
        unsigned int size;

        /**
         *  The horizontal resolution.
         */
        unsigned int xResolution;

        /**
         *  The vertical resolution.
         */
        unsigned int yResolution;

        /**
         * Current error code. Zero means no error.
         */
        FT_Error err;

};

#endif  //  __FTSize__

// ---- FTFace.h
#ifndef     __FTFace__
#define     __FTFace__



/**
 * FTFace class provides an abstraction layer for the Freetype Face.
 *
 * @see "Freetype 2 Documentation"
 *
 */
class FTGL_EXPORT FTFace
{
    public:
        /**
         * Opens and reads a face file. Error is set.
         *
         * @param fontFilePath  font file path.
         */
        FTFace( const char* fontFilePath);

        /**
         * Read face data from an in-memory buffer. Error is set.
         *
         * @param pBufferBytes  the in-memory buffer
         * @param bufferSizeInBytes  the length of the buffer in bytes
         */
        FTFace( const unsigned char *pBufferBytes, size_t bufferSizeInBytes );

        /**
         * Destructor
         *
         * Disposes of the current Freetype Face.
         */
        virtual ~FTFace();

        /**
         * Attach auxiliary file to font (e.g., font metrics).
         *
         * @param fontFilePath  auxiliary font file path.
         * @return          <code>true</code> if file has opened
         *                  successfully.
         */
        bool Attach( const char* fontFilePath);

        /**
         * Attach auxiliary data to font (e.g., font metrics) from memory
         *
         * @param pBufferBytes  the in-memory buffer
         * @param bufferSizeInBytes  the length of the buffer in bytes
         * @return          <code>true</code> if file has opened
         *                  successfully.
         */
        bool Attach( const unsigned char *pBufferBytes, size_t bufferSizeInBytes);

        /**
         * Get the freetype face object.
         *
         * @return pointer to an FT_Face.
         */
        FT_Face* Face() const { return ftFace;}

        /**
         * Sets the char size for the current face.
         *
         * This doesn't guarantee that the size was set correctly. Clients
         * should check errors.
         *
         * @param size      the face size in points (1/72 inch)
         * @param res       the resolution of the target device.
         * @return          <code>FTSize</code> object
         */
        const FTSize& Size( const unsigned int size, const unsigned int res);

        /**
         * Get the number of character maps in this face.
         *
         * @return character map count.
         */
        unsigned int CharMapCount();

        /**
         * Get a list of character maps in this face.
         *
         * @return pointer to the first encoding.
         */
        FT_Encoding* CharMapList();

        /**
         * Gets the kerning vector between two glyphs
         */
        FTPoint KernAdvance( unsigned int index1, unsigned int index2);

        /**
         * Loads and creates a Freetype glyph.
         */
        FT_GlyphSlot Glyph( unsigned int index, FT_Int load_flags);

        /**
         * Gets the number of glyphs in the current face.
         */
        unsigned int GlyphCount() const { return numGlyphs;}

        /**
         * Queries for errors.
         *
         * @return  The current error code.
         */
        FT_Error Error() const { return err; }

    private:
        /**
         * The Freetype face
         */
        FT_Face* ftFace;

        /**
         * The size object associated with this face
         */
        FTSize  charSize;

        /**
         * The number of glyphs in this face
         */
        int numGlyphs;

        FT_Encoding* fontEncodingList;

        /**
         * This face has kerning tables
         */
         bool hasKerningTable;

        /**
         * Current error code. Zero means no error.
         */
        FT_Error err;
};


#endif  //  __FTFace__

// ---- FTFont.h
#ifndef    __FTFont__
#define    __FTFont__



class FTGlyphContainer;
class FTGlyph;


/**
 * FTFont is the public interface for the FTGL library.
 *
 * Specific font classes are derived from this class. It uses the helper
 * classes FTFace and FTSize to access the Freetype library. This class
 * is abstract and deriving classes must implement the protected
 * <code>MakeGlyph</code> function to create glyphs of the
 * appropriate type.
 *
 * It is good practice after using these functions to test the error
 * code returned. <code>FT_Error Error()</code>. Check the freetype file fterrdef.h
 * for error definitions.
 *
 * @see     FTFace
 * @see     FTSize
 * @see     FTGlyphContainer
 * @see     FTGlyph
 */
class FTGL_EXPORT FTFont
{
    public:
        /**
         * Open and read a font file. Sets Error flag.
         *
         * @param fontFilePath  font file path.
         */
        FTFont( const char* fontFilePath);

        /**
         * Open and read a font from a buffer in memory. Sets Error flag.
         * The buffer is owned by the client and is NOT copied by FTGL. The
         * pointer must be valid while using FTGL.
         *
         * @param pBufferBytes  the in-memory buffer
         * @param bufferSizeInBytes  the length of the buffer in bytes
         */
        FTFont( const unsigned char *pBufferBytes, size_t bufferSizeInBytes);

        /**
         * Destructor
         */
        virtual ~FTFont();

        /**
         * Attach auxiliary file to font e.g font metrics.
         *
         * Note: not all font formats implement this function.
         *
         * @param fontFilePath  auxiliary font file path.
         * @return          <code>true</code> if file has been attached
         *                  successfully.
         */
        bool Attach( const char* fontFilePath);

        /**
         * Attach auxiliary data to font e.g font metrics, from memory
         *
         * Note: not all font formats implement this function.
         *
         * @param pBufferBytes  the in-memory buffer
         * @param bufferSizeInBytes  the length of the buffer in bytes
         * @return          <code>true</code> if file has been attached
         *                  successfully.
         */
        bool Attach( const unsigned char *pBufferBytes, size_t bufferSizeInBytes);

        /**
         * Set the character map for the face.
         *
         * @param encoding      Freetype enumerate for char map code.
         * @return              <code>true</code> if charmap was valid and
         *                      set correctly
         */
        bool CharMap( FT_Encoding encoding );

        /**
         * Get the number of character maps in this face.
         *
         * @return character map count.
         */
        unsigned int CharMapCount();

        /**
         * Get a list of character maps in this face.
         *
         * @return pointer to the first encoding.
         */
        FT_Encoding* CharMapList();

        /**
         * Set the char size for the current face.
         *
         * @param size      the face size in points (1/72 inch)
         * @param res       the resolution of the target device.
         * @return          <code>true</code> if size was set correctly
         */
        virtual bool FaceSize( const unsigned int size, const unsigned int res = 72);

        /**
         * Get the current face size in points.
         *
         * @return face size
         */
        unsigned int FaceSize() const;

        /**
         * Set the extrusion distance for the font. Only implemented by
         * FTGLExtrdFont
         *
         * @param depth  The extrusion distance.
         */
        virtual void Depth(float /* depth */){}

        /**
         * Enable or disable the use of Display Lists inside FTGL
         *
         * @param  useList <code>true</code> turns ON display lists.
         *                 <code>false</code> turns OFF display lists.
         */
        void UseDisplayList( bool useList);

        /**
         * Get the global ascender height for the face.
         *
         * @return  Ascender height
         */
        float Ascender() const;

        /**
         * Gets the global descender height for the face.
         *
         * @return  Descender height
         */
        float Descender() const;

        /**
         * Gets the line spacing for the font.
         *
         * @return  Line height
         */
        float LineHeight() const;

        /**
         * Get the bounding box for a string.
         *
         * @param string    a char string
         * @param llx       lower left near x coord
         * @param lly       lower left near y coord
         * @param llz       lower left near z coord
         * @param urx       upper right far x coord
         * @param ury       upper right far y coord
         * @param urz       upper right far z coord
         */
        void BBox( const char* string, float& llx, float& lly, float& llz, float& urx, float& ury, float& urz);

        /**
         * Get the bounding box for a string.
         *
         * @param string    a wchar_t string
         * @param llx       lower left near x coord
         * @param lly       lower left near y coord
         * @param llz       lower left near z coord
         * @param urx       upper right far x coord
         * @param ury       upper right far y coord
         * @param urz       upper right far z coord
         */
        void BBox( const wchar_t* string, float& llx, float& lly, float& llz, float& urx, float& ury, float& urz);

        /**
         * Get the advance width for a string.
         *
         * @param string    a wchar_t string
         * @return      advance width
         */
        float Advance( const wchar_t* string);

        /**
         * Get the advance width for a string.
         *
         * @param string    a char string
         * @return      advance width
         */
        float Advance( const char* string);

        /**
         * Prepare for rendering
         */
        virtual void PreRender() { preRenderCalled = true; }

        /**
         * Cleanup after rendering
         */
        virtual void PostRender() { preRenderCalled = false; }

        /**
         * Render a string of characters
         *
         * @param string    'C' style string to be output.
         */
        virtual void Render( const char* string );

        /**
         * Render a string of characters
         *
         * @param string    'C' style string to be output.
         * @param w_max     maximum width of text
         * @param w_fade    width at which text starts to fade
         */
        virtual void Render( const char* string, float w_max, float w_fade );

        /**
         * Render a string of characters
         *
         * @param string    wchar_t string to be output.
         */
        virtual void Render( const wchar_t* string );

        /**
         * Queries the Font for errors.
         *
         * @return  The current error code.
         */
        FT_Error Error() const { return err;}

    protected:
        /**
         * Construct a glyph of the correct type.
         *
         * Clients must overide the function and return their specialised
         * FTGlyph.
         *
         * @param g The glyph index NOT the char code.
         * @return  An FT****Glyph or <code>null</code> on failure.
         */
        virtual FTGlyph* MakeGlyph( unsigned int g) = 0;

        /**
         * Current face object
         */
        FTFace face;

        /**
         * Current size object
         */
        FTSize charSize;

        /**
         * Flag to enable or disable the use of Display Lists inside FTGL
         * <code>true</code> turns ON display lists.
         * <code>false</code> turns OFF display lists.
         */
        bool useDisplayLists;

        /**
         * Current error code. Zero means no error.
         */
        FT_Error err;

    private:
        /**
         * Check that the glyph at <code>chr</code> exist. If not load it.
         *
         * @param chr  character index
         * @return <code>true</code> if the glyph can be created.
         */
        inline bool CheckGlyph( const unsigned int chr);

        /**
         * PreRender() was called from outside. Do not call it again
         * from Render(), nor call PostRender().
         * User has to call it themselves.
         */
        bool preRenderCalled;

        /**
         * An object that holds a list of glyphs
         */
        FTGlyphContainer* glyphList;

        /**
         * Current pen or cursor position;
         */
        FTPoint pen;
};


#endif  //  __FTFont__

// ---- FTGLBitmapFont.h
#ifndef __FTGLBitmapFont__
#define __FTGLBitmapFont__



class FTGlyph;

/**
 * FTGLBitmapFont is a specialisation of the FTFont class for handling
 * Bitmap fonts
 *
 * @see     FTFont
 */
class FTGL_EXPORT FTGLBitmapFont : public FTFont
{
    public:
        /**
         * Open and read a font file. Sets Error flag.
         *
         * @param fontFilePath  font file path.
         */
        FTGLBitmapFont( const char* fontFilePath);

        /**
         * Open and read a font from a buffer in memory. Sets Error flag.
         *
         * @param pBufferBytes  the in-memory buffer
         * @param bufferSizeInBytes  the length of the buffer in bytes
         */
        FTGLBitmapFont( const unsigned char *pBufferBytes, size_t bufferSizeInBytes);

        /**
         * Destructor
         */
        ~FTGLBitmapFont();

        /**
         * Prepare for rendering
         */
        virtual void PreRender() override;

        /**
         * Cleanup after rendering
         */
        virtual void PostRender() override;

    private:
        /**
         * Construct a FTBitmapGlyph.
         *
         * @param g The glyph index NOT the char code.
         * @return  An FTBitmapGlyph or <code>null</code> on failure.
         */
        inline virtual FTGlyph* MakeGlyph( unsigned int g) override;

};
#endif  //  __FTGLBitmapFont__

// ---- FTGLExtrdFont.h
#ifndef __FTGLExtrdFont__
#define __FTGLExtrdFont__


class FTGlyph;

/**
 * FTGLExtrdFont is a specialisation of the FTFont class for handling
 * extruded Polygon fonts
 *
 * @see  FTFont
 * @see  FTGLPolygonFont
 */
class FTGL_EXPORT FTGLExtrdFont : public FTFont
{
    public:
        /**
         * Open and read a font file. Sets Error flag.
         *
         * @param fontFilePath  font file path.
         */
        FTGLExtrdFont( const char* fontFilePath);

        /**
         * Open and read a font from a buffer in memory. Sets Error flag.
         *
         * @param pBufferBytes  the in-memory buffer
         * @param bufferSizeInBytes  the length of the buffer in bytes
         */
        FTGLExtrdFont( const unsigned char *pBufferBytes, size_t bufferSizeInBytes);

        /**
         * Destructor
         */
        ~FTGLExtrdFont();

        /**
         * Set the extrusion distance for the font.
         *
         * @param d  The extrusion distance.
         */
        void Depth( float d) override { depth = d;}

    private:
        /**
         * Construct a FTPolyGlyph.
         *
         * @param glyphIndex The glyph index NOT the char code.
         * @return An FTExtrdGlyph or <code>null</code> on failure.
         */
        inline virtual FTGlyph* MakeGlyph( unsigned int glyphIndex) override;

        /**
         * The extrusion distance for the font.
         */
        float depth;
};


#endif // __FTGLExtrdFont__

// ---- FTGLOutlineFont.h
#ifndef     __FTGLOutlineFont__
#define     __FTGLOutlineFont__



class FTGlyph;


/**
 * FTGLOutlineFont is a specialisation of the FTFont class for handling
 * Vector Outline fonts
 *
 * @see     FTFont
 */
class FTGL_EXPORT FTGLOutlineFont : public FTFont
{
    public:
        /**
         * Open and read a font file. Sets Error flag.
         *
         * @param fontFilePath  font file path.
         */
        FTGLOutlineFont( const char* fontFilePath);

        /**
         * Open and read a font from a buffer in memory. Sets Error flag.
         *
         * @param pBufferBytes  the in-memory buffer
         * @param bufferSizeInBytes  the length of the buffer in bytes
         */
        FTGLOutlineFont( const unsigned char *pBufferBytes, size_t bufferSizeInBytes);

        /**
         * Destructor
         */
        ~FTGLOutlineFont();

        /**
         * Prepare for rendering
         */
        virtual void PreRender() override;

        /**
         * Cleanup after rendering
         */
        virtual void PostRender() override;

    private:
        /**
         * Construct a FTOutlineGlyph.
         *
         * @param g The glyph index NOT the char code.
         * @return  An FTOutlineGlyph or <code>null</code> on failure.
         */
        inline virtual FTGlyph* MakeGlyph( unsigned int g) override;

};
#endif // __FTGLOutlineFont__

// ---- FTGLPixmapFont.h
#ifndef     __FTGLPixmapFont__
#define     __FTGLPixmapFont__




class FTGlyph;


/**
 * FTGLPixmapFont is a specialisation of the FTFont class for handling
 * Pixmap (Grey Scale) fonts
 *
 * @see     FTFont
 */
class FTGL_EXPORT FTGLPixmapFont : public FTFont
{
    public:
        /**
         * Open and read a font file. Sets Error flag.
         *
         * @param fontFilePath  font file path.
         */
        FTGLPixmapFont( const char* fontFilePath);

        /**
         * Open and read a font from a buffer in memory. Sets Error flag.
         *
         * @param pBufferBytes  the in-memory buffer
         * @param bufferSizeInBytes  the length of the buffer in bytes
         */
        FTGLPixmapFont( const unsigned char *pBufferBytes, size_t bufferSizeInBytes);

        /**
         * Destructor
         */
        ~FTGLPixmapFont();

        /**
         * Prepare for rendering
         */
        virtual void PreRender() override;

        /**
         * Cleanup after rendering
         */
        virtual void PostRender() override;

    private:
        /**
         * Construct a FTPixmapGlyph.
         *
         * @param g The glyph index NOT the char code.
         * @return  An FTPixmapGlyph or <code>null</code> on failure.
         */
        inline virtual FTGlyph* MakeGlyph( unsigned int g) override;

};


#endif  //  __FTGLPixmapFont__

// ---- FTGLPolygonFont.h
#ifndef     __FTGLPolygonFont__
#define     __FTGLPolygonFont__



class FTGlyph;


/**
 * FTGLPolygonFont is a specialisation of the FTFont class for handling
 * tesselated Polygon Mesh fonts
 *
 * @see     FTFont
 */
class FTGL_EXPORT FTGLPolygonFont : public FTFont
{
    public:
        /**
         * Open and read a font file. Sets Error flag.
         *
         * @param fontFilePath  font file path.
         */
        FTGLPolygonFont( const char* fontFilePath);

        /**
         * Open and read a font from a buffer in memory. Sets Error flag.
         *
         * @param pBufferBytes  the in-memory buffer
         * @param bufferSizeInBytes  the length of the buffer in bytes
         */
        FTGLPolygonFont( const unsigned char *pBufferBytes, size_t bufferSizeInBytes);

        /**
         * Destructor
         */
        ~FTGLPolygonFont();

    private:
        /**
         * Construct a FTPolyGlyph.
         *
         * @param g The glyph index NOT the char code.
         * @return  An FTPolyGlyph or <code>null</code> on failure.
         */
        inline virtual FTGlyph* MakeGlyph( unsigned int g) override;

};


#endif  //  __FTGLPolygonFont__

// ---- FTGLTextureFont.h
#ifndef     __FTGLTextureFont__
#define     __FTGLTextureFont__


class FTTextureGlyph;


/**
 * FTGLTextureFont is a specialisation of the FTFont class for handling
 * Texture mapped fonts
 *
 * @see     FTFont
 */
class  FTGL_EXPORT FTGLTextureFont : public FTFont
{
    public:
        /**
         * Open and read a font file. Sets Error flag.
         *
         * @param fontFilePath  font file path.
         */
        FTGLTextureFont( const char* fontFilePath);

        /**
         * Open and read a font from a buffer in memory. Sets Error flag.
         *
         * @param pBufferBytes  the in-memory buffer
         * @param bufferSizeInBytes  the length of the buffer in bytes
         */
        FTGLTextureFont( const unsigned char *pBufferBytes, size_t bufferSizeInBytes);

        /**
         * Destructor
         */
        virtual ~FTGLTextureFont();

        /**
            * Set the char size for the current face.
         *
         * @param size      the face size in points (1/72 inch)
         * @param res       the resolution of the target device.
         * @return          <code>true</code> if size was set correctly
         */
        virtual bool FaceSize( const unsigned int size, const unsigned int res = 72) override;

         /**
         * Prepare for rendering
         */
        virtual void PreRender() override;

         /**
         * Cleanup after rendering
         */
        virtual void PostRender() override;

    private:
        /**
         * Construct a FTTextureGlyph.
         *
         * @param glyphIndex The glyph index NOT the char code.
         * @return  An FTTextureGlyph or <code>null</code> on failure.
         */
        inline virtual FTGlyph* MakeGlyph( unsigned int glyphIndex) override;

        /**
         * Get the size of a block of memory required to layout the glyphs
         *
         * Calculates a width and height based on the glyph sizes and the
         * number of glyphs. It over estimates.
         */
        inline void CalculateTextureSize();

        /**
         * Creates a 'blank' OpenGL texture object.
         *
         * The format is GL_ALPHA and the params are
         * GL_TEXTURE_WRAP_S = GL_CLAMP
         * GL_TEXTURE_WRAP_T = GL_CLAMP
         * GL_TEXTURE_MAG_FILTER = GL_LINEAR
         * GL_TEXTURE_MIN_FILTER = GL_LINEAR
         * Note that mipmapping is NOT used
         */
        inline GLuint CreateTexture();

        /**
         * The maximum texture dimension on this OpenGL implemetation
         */
        GLsizei maximumGLTextureSize;

        /**
         * The minimum texture width required to hold the glyphs
         */
        GLsizei textureWidth;

        /**
         * The minimum texture height required to hold the glyphs
         */
        GLsizei textureHeight;

        /**
         *An array of texture ids
         */
         FTVector<GLuint> textureIDList;

        /**
         * The max height for glyphs in the current font
         */
        int glyphHeight;

        /**
         * The max width for glyphs in the current font
         */
        int glyphWidth;

        /**
         * A value to be added to the height and width to ensure that
         * glyphs don't overlap in the texture
         */
        unsigned int padding;

        /**
         *
         */
         unsigned int numGlyphs;

        /**
         */
        unsigned int remGlyphs;

        /**
         */
        int xOffset;

        /**
         */
        int yOffset;

};


#endif // __FTGLTextureFont__

// ---- FTGlyphContainer.h
#ifndef     __FTGlyphContainer__
#define     __FTGlyphContainer__



class FTFace;
class FTGlyph;
class FTCharmap;

/**
 * FTGlyphContainer holds the post processed FTGlyph objects.
 *
 * @see FTGlyph
 */
class FTGL_EXPORT FTGlyphContainer
{
        typedef FTVector<FTGlyph*> GlyphVector;
    public:
        /**
         * Constructor
         *
         * @param face      The Freetype face
         */
        FTGlyphContainer( FTFace* face);

        /**
         * Destructor
         */
        ~FTGlyphContainer();

        /**
         * Sets the character map for the face.
         *
         * @param encoding      the Freetype encoding symbol. See above.
         * @return              <code>true</code> if charmap was valid
         *                      and set correctly
         */
        bool CharMap( FT_Encoding encoding);

        /**
         * Get the font index of the input character.
         *
         * @param characterCode The character code of the requested glyph in the
         *                      current encoding eg apple roman.
         * @return      The font index for the character.
         */
        unsigned int FontIndex( const unsigned int characterCode ) const;

        /**
         * Adds a glyph to this glyph list.
         *
         * @param glyph         The FTGlyph to be inserted into the container
         * @param characterCode The char code of the glyph NOT the glyph index.
         */
        void Add( FTGlyph* glyph, const unsigned int characterCode);

        /**
         * Get a glyph from the glyph list
         *
         * @param characterCode The char code of the glyph NOT the glyph index
         * @return              An FTGlyph or <code>null</code> is it hasn't been
         * loaded.
         */
        FTGlyph* Glyph( const unsigned int characterCode) const;

        /**
         * Get the bounding box for a character.
         * @param characterCode The char code of the glyph NOT the glyph index
         */
        FTBBox BBox( const unsigned int characterCode) const;

        /**
        * Returns the kerned advance width for a glyph.
        *
        * @param characterCode     glyph index of the character
        * @param nextCharacterCode the next glyph in a string
        * @return                  advance width
        */
        float Advance( const unsigned int characterCode, const unsigned int nextCharacterCode);

        /**
         * Renders a character
         * @param characterCode      the glyph to be Rendered
         * @param nextCharacterCode  the next glyph in the string. Used for kerning.
         * @param penPosition        the position to Render the glyph
         * @return                   The distance to advance the pen position after Rendering
         */
        FTPoint Render( const unsigned int characterCode, const unsigned int nextCharacterCode, FTPoint penPosition);

        /**
         * Queries the Font for errors.
         *
         * @return  The current error code.
         */
        FT_Error Error() const { return err;}

    private:
        /**
         * The FTGL face
         */
        FTFace* face;

        /**
         * The Character Map object associated with the current face
         */
        FTCharmap* charMap;

        /**
         * A structure to hold the glyphs
         */
        GlyphVector glyphs;

        /**
         * Current error code. Zero means no error.
         */
        FT_Error err;
};


#endif  //  __FTGlyphContainer__

// ---- FTLibrary.h
#ifndef     __FTLibrary__
#define     __FTLibrary__

//#include FT_CACHE_H



/**
 * FTLibrary class is the global accessor for the Freetype library.
 *
 * This class encapsulates the Freetype Library. This is a singleton class
 * and ensures that only one FT_Library is in existence at any one time.
 * All constructors are private therefore clients cannot create or
 * instantiate this class themselves and must access it's methods via the
 * static <code>FTLibrary::Instance()</code> function.
 *
 * Just because this class returns a valid <code>FTLibrary</code> object
 * doesn't mean that the Freetype Library has been successfully initialised.
 * Clients should check for errors. You can initialse the library AND check
 * for errors using the following code...
 * <code>err = FTLibrary::Instance().Error();</code>
 *
 * @see "Freetype 2 Documentation"
 *
 */
class FTGL_EXPORT FTLibrary
{
    public:
        /**
         * Global acces point to the single FTLibrary object.
         *
         * @return  The global <code>FTLibrary</code> object.
         */
        static const FTLibrary& Instance();

        /**
         * Gets a pointer to the native Freetype library.
         *
         * @return A handle to a FreeType library instance.
         */
        const FT_Library* GetLibrary() const { return library;}

        /**
         * Queries the library for errors.
         *
         * @return  The current error code.
         */
        FT_Error Error() const { return err;}

        /**
         * Destructor
         *
         * Disposes of the Freetype library
         */
        ~FTLibrary();

    private:
        /**
         * Default constructors.
         *
         * Made private to stop clients creating there own FTLibrary
         * objects.
         */
        FTLibrary();
        FTLibrary( const FT_Library&){}
        FTLibrary& operator=( const FT_Library&) { return *this; }

        /**
         * Initialises the Freetype library
         *
         * Even though this function indicates success via the return value,
         * clients can't see this so must check the error codes. This function
         * is only ever called by the default c_stor
         *
         * @return  <code>true</code> if the Freetype library was
         *          successfully initialised, <code>false</code>
         *          otherwise.
         */
        bool Initialise();

        /**
         * Freetype library handle.
         */
        FT_Library* library;
//      FTC_Manager* manager;

        /**
         * Current error code. Zero means no error.
         */
        FT_Error err;

};

#endif  //  __FTLibrary__

// ---- FTList.h
#ifndef    __FTList__
#define    __FTList__


/**
* Provides a non-STL alternative to the STL list
 */
template <typename FT_LIST_ITEM_TYPE>
class FTGL_EXPORT FTList
{
    public:
        typedef FT_LIST_ITEM_TYPE value_type;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef size_t size_type;

        /**
         * Constructor
         */
        FTList()
        :   listSize(0),
            tail(nullptr)
        {
            tail = nullptr;
            head = new Node;
        }

        /**
         * Destructor
         */
        ~FTList()
        {
            Node* next;

            for( Node *walk = head; walk; walk = next)
            {
                next = walk->next;
                delete walk;
            }
        }

        /**
         * Get the number of items in the list
         */
        size_type size() const
        {
            return listSize;
        }

        /**
         * Add an item to the end of the list
         */
        void push_back( const value_type& item)
        {
            Node* node = new Node( item);

            if( head->next == nullptr)
            {
                head->next = node;
            }

            if( tail)
            {
                tail->next = node;
            }
            tail = node;
            ++listSize;
        }

        /**
         * Get the item at the front of the list
         */
        reference front() const
        {
            return head->next->payload;
        }

        /**
         * Get the item at the end of the list
         */
        reference back() const
        {
            return tail->payload;
        }

    private:
        struct Node
        {
            Node()
            : next(nullptr)
            {}

            Node( const value_type& item)
            : next(nullptr)
            {
                payload = item;
            }

            Node* next;

            value_type payload;
        };

        size_type listSize;

        Node* head;
        Node* tail;
};

#endif // __FTList__

// ---- FTOutlineGlyph.h
#ifndef  __FTOutlineGlyph__
#define  __FTOutlineGlyph__



class FTVectoriser;


/**
 * FTOutlineGlyph is a specialisation of FTGlyph for creating outlines.
 *
 * @see FTGlyphContainer
 * @see FTVectoriser
 *
 */
class FTGL_EXPORT FTOutlineGlyph : public FTGlyph
{
    public:
        /**
         * Constructor. Sets the Error to Invalid_Outline if the glyphs isn't an outline.
         *
         * @param glyph The Freetype glyph to be processed
         * @param useDisplayList Enable or disable the use of Display Lists for this glyph
         *                       <code>true</code> turns ON display lists.
         *                       <code>false</code> turns OFF display lists.
         */
        FTOutlineGlyph( FT_GlyphSlot glyph, bool useDisplayList);

        /**
         * Destructor
         */
        virtual ~FTOutlineGlyph();

        /**
         * Renders this glyph at the current pen position.
         *
         * @param pen The current pen position.
         * @return    The advance distance for this glyph.
         */
        virtual const FTPoint& Render( const FTPoint& pen) override;

    private:
        /**
         * OpenGL display list
         */
        GLuint glList;

};


#endif // __FTOutlineGlyph__

// ---- FTPixmapGlyph.h
#ifndef     __FTPixmapGlyph__
#define     __FTPixmapGlyph__





/**
 * FTPixmapGlyph is a specialisation of FTGlyph for creating pixmaps.
 *
 * @see FTGlyphContainer
 *
 */
class  FTGL_EXPORT FTPixmapGlyph : public FTGlyph
{
    public:
        /**
         * Constructor
         *
         * @param glyph The Freetype glyph to be processed
         */
        FTPixmapGlyph( FT_GlyphSlot glyph);

        /**
         * Destructor
         */
        virtual ~FTPixmapGlyph();

        /**
         * Renders this glyph at the current pen position.
         *
         * @param pen   The current pen position.
         * @return      The advance distance for this glyph.
         */
        virtual const FTPoint& Render( const FTPoint& pen) override;

        // attributes

    private:
        /**
         * The width of the glyph 'image'
         */
        int destWidth;

        /**
         * The height of the glyph 'image'
         */
        int destHeight;

        /**
         * Vector from the pen position to the topleft corner of the pixmap
         */
        FTPoint pos;

        /**
         * Pointer to the 'image' data
         */
        unsigned char* data;

};


#endif  //  __FTPixmapGlyph__

// ---- FTPolyGlyph.h
#ifndef     __FTPolyGlyph__
#define     __FTPolyGlyph__




class FTVectoriser;

/**
 * FTPolyGlyph is a specialisation of FTGlyph for creating tessellated
 * polygon glyphs.
 *
 * @see FTGlyphContainer
 * @see FTVectoriser
 *
 */
class FTGL_EXPORT FTPolyGlyph : public FTGlyph
{
    public:
        /**
         * Constructor. Sets the Error to Invalid_Outline if the glyphs isn't an outline.
         *
         * @param glyph The Freetype glyph to be processed
         * @param useDisplayList Enable or disable the use of Display Lists for this glyph
         *                       <code>true</code> turns ON display lists.
         *                       <code>false</code> turns OFF display lists.
         */
        FTPolyGlyph( FT_GlyphSlot glyph, bool useDisplayList);

        /**
         * Destructor
         */
        virtual ~FTPolyGlyph();

        /**
         * Renders this glyph at the current pen position.
         *
         * @param pen   The current pen position.
         * @return      The advance distance for this glyph.
         */
        virtual const FTPoint& Render( const FTPoint& pen) override;

    private:
        /**
         * OpenGL display list
         */
        GLuint glList;

};


#endif  //  __FTPolyGlyph__

// ---- FTTextureGlyph.h
#ifndef     __FTTextureGlyph__
#define     __FTTextureGlyph__





/**
 * FTTextureGlyph is a specialisation of FTGlyph for creating texture
 * glyphs.
 *
 * @see FTGlyphContainer
 *
 */
class FTGL_EXPORT FTTextureGlyph : public FTGlyph
{
    public:
        /**
         * Constructor
         *
         * @param glyph     The Freetype glyph to be processed
         * @param id        The id of the texture that this glyph will be
         *                  drawn in
         * @param xOffset   The x offset into the parent texture to draw
         *                  this glyph
         * @param yOffset   The y offset into the parent texture to draw
         *                  this glyph
         * @param width     The width of the parent texture
         * @param height    The height (number of rows) of the parent texture
         */
        FTTextureGlyph( FT_GlyphSlot glyph, int id, int xOffset, int yOffset, GLsizei width, GLsizei height);

        /**
         * Destructor
         */
        virtual ~FTTextureGlyph();

        /**
         * Renders this glyph at the current pen position.
         *
         * @param pen   The current pen position.
         * @return      The advance distance for this glyph.
         */
        virtual const FTPoint& Render( const FTPoint& pen) override;

        /**
         * Reset the currently active texture to zero to get into a known state before
         * drawing a string. This is to get round possible threading issues.
         */
        static void ResetActiveTexture(){ activeTextureID = 0;}

    private:
        /**
         * The width of the glyph 'image'
         */
        int destWidth;

        /**
         * The height of the glyph 'image'
         */
        int destHeight;

        /**
         * Vector from the pen position to the topleft corner of the pixmap
         */
        FTPoint pos;

        /**
         * The texture co-ords of this glyph within the texture.
         */
        FTPoint uv[2];

        /**
         * The texture index that this glyph is contained in.
         */
        int glTextureID;

        /**
         * The texture index of the currently active texture
         *
         * We keep track of the currently active texture to try to reduce the number
         * of texture bind operations.
         */
        static GLint activeTextureID;

};


#endif  //  __FTTextureGlyph__

// ---- FTVectoriser.h
#ifndef     __FTVectoriser__
#define     __FTVectoriser__




#ifndef CALLBACK
#define CALLBACK
#endif


/**
 * FTTesselation captures points that are output by OpenGL's gluTesselator.
 */
class FTGL_EXPORT FTTesselation
{
    public:
        /**
         * Default constructor
         */
        FTTesselation( GLenum m)
        : meshType(m)
        {
            pointList.reserve( 128);
        }

        /**
         *  Destructor
         */
        ~FTTesselation()
        {
            pointList.clear();
        }

        /**
         * Add a point to the mesh.
         */
        void AddPoint( const FTGL_DOUBLE x, const FTGL_DOUBLE y, const FTGL_DOUBLE z)
        {
            pointList.push_back( FTPoint( x, y, z));
        }

        /**
         * The number of points in this mesh
         */
        size_t PointCount() const { return pointList.size();}

        /**
         *
         */
        const FTPoint& Point( unsigned int index) const { return pointList[index];}

        /**
         * Return the OpenGL polygon type.
         */
        GLenum PolygonType() const { return meshType;}

    private:
        /**
         * Points generated by gluTesselator.
         */
        typedef FTVector<FTPoint> PointVector;
        PointVector pointList;

        /**
         * OpenGL primitive type from gluTesselator.
         */
        GLenum meshType;
};


/**
 * FTMesh is a container of FTTesselation's that make up a polygon glyph
 */
class FTGL_EXPORT FTMesh
{
   typedef FTVector<FTTesselation*> TesselationVector;
   typedef FTList<FTPoint> PointList;

    public:
        /**
         * Default constructor
         */
        FTMesh();

        /**
         *  Destructor
         */
        ~FTMesh();

        /**
         * Add a point to the mesh
         */
        void AddPoint( const FTGL_DOUBLE x, const FTGL_DOUBLE y, const FTGL_DOUBLE z);

        /**
         *  Create a combine point for the gluTesselator
         */
        const FTGL_DOUBLE* Combine( const FTGL_DOUBLE x, const FTGL_DOUBLE y, const FTGL_DOUBLE z);

        /**
         * Begin a new polygon
         */
        void Begin( GLenum meshType);

        /**
         * End a polygon
         */
        void End();

        /**
         * Record a gluTesselation error
         */
        void Error( GLenum e) { err = e;}

        /**
         * The number of tesselations in the mesh
         */
        unsigned int TesselationCount() const { return tesselationList.size();}

        /**
         * Get a tesselation by index
         */
        const FTTesselation* Tesselation( unsigned int index) const;

        /**
         * Return the temporary point list. For testing only.
         */
        const PointList& TempPointList() const { return tempPointList;}

        /**
         * Get the GL ERROR returned by the glu tesselator
         */
        GLenum Error() const { return err;}

    private:
        /**
         * The current sub mesh that we are constructing.
         */
        FTTesselation* currentTesselation;

        /**
         * Holds each sub mesh that comprises this glyph.
         */
        TesselationVector tesselationList;

        /**
         * Holds extra points created by gluTesselator. See ftglCombine.
         */
        PointList tempPointList;

        /**
         * GL ERROR returned by the glu tesselator
         */
        GLenum err;

};

const FTGL_DOUBLE FTGL_FRONT_FACING = 1.0;
const FTGL_DOUBLE FTGL_BACK_FACING = -1.0;

/**
 * FTVectoriser class is a helper class that converts font outlines into
 * point data.
 *
 * @see FTExtrdGlyph
 * @see FTOutlineGlyph
 * @see FTPolyGlyph
 * @see FTContour
 * @see FTPoint
 *
 */
class FTGL_EXPORT FTVectoriser
{
    public:
        /**
         * Constructor
         *
         * @param glyph The freetype glyph to be processed
         */
        FTVectoriser( const FT_GlyphSlot glyph);

        /**
         *  Destructor
         */
        virtual ~FTVectoriser();

        /**
         * Build an FTMesh from the vector outline data.
         *
         * @param zNormal   The direction of the z axis of the normal
         *                  for this mesh
         */
        void MakeMesh( FTGL_DOUBLE zNormal = FTGL_FRONT_FACING);

        /**
         * Get the current mesh.
         */
        const FTMesh* GetMesh() const { return mesh;}

        /**
         * Get the total count of points in this outline
         *
         * @return the number of points
         */
        size_t PointCount();

        /**
         * Get the count of contours in this outline
         *
         * @return the number of contours
         */
        size_t ContourCount() const { return ftContourCount;}

        /**
         * Return a contour at index
         *
         * @return the number of contours
         */
         const FTContour* Contour( unsigned int index) const;

        /**
         * Get the number of points in a specific contour in this outline
         *
         * @param c     The contour index
         * @return      the number of points in contour[c]
         */
        size_t ContourSize( int c) const { return contourList[c]->PointCount();}

        /**
         * Get the flag for the tesselation rule for this outline
         *
         * @return The contour flag
         */
        int ContourFlag() const { return contourFlag;}

    private:
        /**
         * Process the freetype outline data into contours of points
         */
        void ProcessContours();

        /**
         * The list of contours in the glyph
         */
        FTContour** contourList;

        /**
         * A Mesh for tesselations
         */
        FTMesh* mesh;

        /**
         * The number of contours reported by Freetype
         */
        short ftContourCount;

        /**
         * A flag indicating the tesselation rule for the glyph
         */
        int contourFlag;

        /**
         * A Freetype outline
         */
        FT_Outline outline;
};


#endif  //  __FTVectoriser__

} // namespace gled

#endif
