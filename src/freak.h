//  freak.h
//
//	Copyright (C) 2011-2012  Signal processing laboratory 2, EPFL,
//	Raphael Ortiz (raphael.ortiz@a3.epfl.ch),
//	Kirell Benzi (kirell.benzi@epfl.ch)
//	Alexandre Alahi (alexandre.alahi@epfl.ch)
//	and Pierre Vandergheynst (pierre.vandergheynst@epfl.ch)
//
//  Redistribution and use in source and binary forms, with or without modification,
//  are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
//  This software is provided by the copyright holders and contributors "as is" and
//  any express or implied warranties, including, but not limited to, the implied
//  warranties of merchantability and fitness for a particular purpose are disclaimed.
//  In no event shall the Intel Corporation or contributors be liable for any direct,
//  indirect, incidental, special, exemplary, or consequential damages
//  (including, but not limited to, procurement of substitute goods or services;
//  loss of use, data, or profits; or business interruption) however caused
//  and on any theory of liability, whether in contract, strict liability,
//  or tort (including negligence or otherwise) arising in any way out of
//  the use of this software, even if advised of the possibility of such damage.

#ifndef FREAK_H_INCLUDED
#define FREAK_H_INCLUDED

#include <opencv2/core/core.hpp>
#include <string>

#ifndef M_PI
    #define M_PI 3.141592653589793
#endif

namespace cv {

static const double kSQRT2 = 1.4142135623731;
static const double kINV_SQRT2 = 1.0 / kSQRT2;
static const double kLOG2 = 0.693147180559945;
static const int kNB_SCALES = 64;
static const int kNB_ORIENTATION = 256;
static const int kNB_POINTS = 43;
static const int kNB_PAIRS = 512;
static const int kSMALLEST_KP_SIZE = 7; // smallest size of keypoints
static const int kNB_ORIENPAIRS = 45;

typedef unsigned char uint8;
struct PatternPoint
{
    float x; // x coordinate relative to center
    float y; // x coordinate relative to center
    float sigma; // Gaussian smoothing sigma
};

struct DescriptionPair
{
    uint8 i; // index of the first point
    uint8 j; // index of the second point
};

struct OrientationPair
{
    uint8 i; // index of the first point
    uint8 j; // index of the second point
    int weight_dx; // dx/(norm_sq))*4096
    int weight_dy; // dy/(norm_sq))*4096
};

struct PairStat
{ // used to sort pairs during pairs selection
    double mean;
    int idx;
};

struct sortMean
{
    CV_INLINE bool operator()( const PairStat& a, const PairStat& b ) const {
        return a.mean < b.mean;
    }
};

class CV_EXPORTS FreakDescriptorExtractor : public cv::DescriptorExtractor
{
public:
    /** Constructor
         * @param orientationNormalized enable orientation normalization
         * @param scaleNormalized enable scale normalization
         * @param patternScale scaling of the description pattern
         * @param nb_octave number of octaves covered by the detected keypoints
         * @param filename (optional) name of the file containing selected pairs
         */
    FreakDescriptorExtractor( bool orientationNormalized_ = true,
                              bool scaleNormalized_ = true,
                              float patternScale_ = 22.0f,
                              int nb_octave_ = 4,
                              const std::string& filename = ""
                             );

    virtual ~FreakDescriptorExtractor();

    //-----definition of inherited functions

    /** returns the descriptor length in bytes */
    virtual int descriptorSize() const;

    /** returns the descriptor type */
    virtual int descriptorType() const;

    //virtual void read( const cv::FileNode& );
    //virtual void write( cv::FileStorage& ) const;

    //-----FreakDescriptorExtractor specific function

    /** draw the description pattern */
    void drawPattern();

    /** select the 512 "best description pairs"
         * @param images grayscale images set
         * @param keypoints set of detected keypoints
         * @param filename file to store the list of pairs
         * @param corrTresh correlation threshold
         */
    void selectPairs( const std::vector<Mat>& images, std::vector<std::vector<KeyPoint> >& keypoints,
                      const std::string& filename, const double corrTresh = 0.7 );

protected:
    virtual void computeImpl( const Mat& image, std::vector<KeyPoint>& keypoints, Mat& descriptors ) const;

    /** initialize the pattern lookup table
         * @param filename (optional) name of the file containing selected pairs
         */
    void buildPattern( const std::string& filename ); // create the pattern lookup table for all orientation and scale


    /** compute the intensity of a pattern point (simple mean approximation on a square box)
         * @param image grayscale image
         * @param integral integral image
         * @param kp_x keypoint x coordinate in image (in pixels)
         * @param kp_y keypoint y coordinate in image (in pixels)
         * @param scale scale index in look-up table
         * @param rot orientation index in look-up table
         * @param point point index in look-up table
         */
    CV_INLINE uchar meanIntensity( const cv::Mat& image,
                                const cv::Mat& integral,
                                const float kp_x,
                                const float kp_y,
                                const unsigned int scale,
                                const unsigned int rot,
                                const unsigned int point
                                ) const;

protected:
    bool m_orientationNormalized; //true if the orientation is normalized, false otherwise
    bool m_scaleNormalized; //true if the scale is normalized, false otherwise
    float m_patternScale; //scaling of the pattern
    int m_nbOctaves; //number of octaves
    bool m_extAll; // true if all pairs need to be extracted for pairs selection
    PatternPoint* m_patternLookup; // look-up table for the pattern points (position+sigma of all points at all scales and orientation)
    int m_patternSizes[kNB_SCALES]; // size of the pattern at a specific scale (used to check if a point is within image boundaries)
    DescriptionPair m_descriptionPairs[kNB_PAIRS];
    OrientationPair m_orientationPairs[kNB_ORIENPAIRS];
};

} // END NAMESPACE CV

#endif // FREAK_H_INCLUDED
