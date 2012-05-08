/*
Copyright (c) 2012, Kevin Han
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

    Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

#include "seq.hpp"
#include "mouse.hpp"

using namespace std;

/*! Human-related computer vision utilities. */
namespace humancv {
    /*! Since contours are actually "ring" structures but are represented as
     * `vector`s in OpenCV, we wrap them so that indices wrap around.
     */
    typedef seq::WrappedSeq<vector<cv::Point>> Contour;

    /*! Indices into a `Contour` denoting a finger. */
    struct FingerData
    {
        /*! Fingertip index. */
        size_t i;
        /*! Left index of contour representing edge of finger. */
        size_t leftI;
        /*! Right index of contour representing edge of finger. */
        size_t rightI;

        friend ostream &operator<<(ostream &out, const FingerData &f);
    };

    /*! Store finger data from a `Contour` representing a hand (with
     * possible arm attached).
     *
     * @param ctr
     *      The `Contour` to search for fingers in.
     * @param handSize
     *      Estimated hand size (must be big enough to contain any possible
     *      hands).
     * @param out
     *      Output vector of `FingerData` objects.
     * @param k
     *      Number to use for k-curvature computation.
     * @param minDist
     *      Minimum distance between finger peaks.
     * @param fingerAngle
     *      Maximum angle (curvature) a finger can have.
     *
     * See the implementation in `cfinder.cpp` for more details.
     */
    void getFingers(
            const Contour &ctr,
            const cv::Size &handSize,
            vector<FingerData> &out,
            int k,
            float minDist,
            float fingerAngle=1.0
            );

    /*! Basic face detection and modeling. */
    namespace face
    {
        /*! Given rectangles containing face and the size of the parent image,
         * store face skin masks in dest of size `imSize`.
         *
         * Faces are approximated as ellipses with a small overlapping rectangle
         * to model the neck.
         *
         * @param imSize
         *      Size of parent image where faces were found.
         * @param rois
         *      Vector of face rectangles.
         * @param dest
         *      Destination mask to store faces - will be resized to `imSize`.
         */
        void storeMasks(const cv::Size imSize, const vector<cv::Rect> &rois, cv::Mat_<uchar> &dest);

        /*! Store rectangles containing faces in `out`.
         *
         * @param im
         *      Grayscale image to search for faces.
         * @param cascade
         *      Classifier cascade, typically loaded from an XML file. OpenCV
         *      comes with several of these; look for the `haarcascades`
         *      directory in your OpenCV install path.
         * @param out
         *      Output vector of `cv::Rect`s.
         * @param scale
         *      Scaling factor for the image. Image is scaled by this factor
         *      before being passed into the face detector, which may improve
         *      performance for large images.
         */
        void getRects(
                const cv::Mat_<uchar> &im,
                cv::CascadeClassifier &cascade,
                vector<cv::Rect> &out,
                float scale=1
                );
    }

    /*! Color-based skin segmentation. */
    namespace skin
    {
        /*! Store a binary mask with skin pixels = 255, non-skin pixels = 0.
         *
         * Runs a boosted classifier on each pixel to classify it.
         *
         * @param im
         *      Image to compute the mask on. Must have same pixel format as the
         *      one used to train the classifier.
         * @param cls
         *      Boosted classifier that takes a 1x3 `Mat` representing one pixel
         *      and classifies it as skin/non-skin. It should have been trained
         *      with skin as the higher-valued class, as `thres` is the minimum
         *      response (weighted sum) required to classify a pixel as skin.
         * @param dest
         *      Where to store the resulting mask.
         * @param lookup
         *      Pointer to a lookup table for the pixels - can significantly
         *      speed up computation. If `NULL` is provided a new one will be
         *      initialized and destroyed each function call.
         * @param thres
         *      Pixels with classifier responses (weighted sums) above this
         *      threshold will be classified as skin.
         */
        void getMask(
                const cv::Mat_<cv::Vec3b> &im,
                const CvBoost &cls,
                cv::Mat_<uchar> &dest,
                cv::SparseMat_<float> *lookup=NULL,
                float thres=0
                );

    }

    /*! Framework for tracking hand gestures and translating them into mouse
     * motion and actions.
     *
     * Currently, three gestures are defined:
     *
     * no fingers -> left mouse button down<br>
     * one fingers -> no buttons down<br>
     * two fingers -> middle mouse button down
     *
     * ## Brief description of the method
     *
     * ### Training
     *
     * A sequence of frames, each containing a face, is gathered from a video
     * source.
     *
     * This sequence is passed to `train()`, faces are detected, and an Adaboost
     * classifier is trained with the face pixels as the positive class and all
     * other pixels as the negative class. The assumption is that no other
     * objects containing skin, besides the faces, are present in the frames.
     *
     * ### Runtime
     *
     * A skin mask is generated using the classifier, then post-processed with
     * erosion/dilation and removal of small contours and those inside the face.
     * If any contours remain, the largest is selected as the hand contour and
     * used to store the mouse state and position via mouseStateFromContour().
     * Finally, the mouse data is passed to a Kalman filter to smooth out noise.
     *
     * ---
     *
     * For more details please look at the source:
     *
     * https://github.com/fferen/KUtils/blob/master/src/humancv.cpp
     */
    class CursorFinder
    {
    private:
        // these are all temporary variables placed here so they don't have to be
        // initalized each time
        cv::Mat_<uchar> gray;
        cv::Mat_<cv::Vec3b> ycrcb;

        cv::Mat_<uchar> skinMask;
        vector<vector<cv::Point>> skinContours;

        vector<cv::Rect> faceRects;
    public:
        cv::CascadeClassifier cascade;
        float addChance;
        float minCtrProp;
        float handSizeProp;
        float kHandHeightProp;
        float minFingerDistProp;

        CvBoost boost;
        cv::SparseMat_<float> pxToPrediction;
        float thres;

        cv::KalmanFilter kFilter;

        cv::Rect screenRect;
        cv::Rect mouseRect;

        /*! @param mouseRect
         *      Rectangle in frame defining hand borders.
         * @param screenRect
         *      Rectangle defining the screen - where points in `mouseRect` get
         *      projected to.
         * @param cascade
         *      `cv::CascadeClassifier` for detecting faces.
         * @param addChance
         *      Probability that pixel gets added to training samples - used to
         *      reduce training time.
         * @param minCtrProp
         *      Minimum area of contour relative to face (as a proportion).
         * @param handSizeProp
         *      Estimated size of hand relative to face (as a proportion).
         * @param kHandHeightProp
         *      Multiplied by hand height to get k for k-curvature.
         * @param minFingerDistProp
         *      Multiplied by hand height to get minimum distance between
         *      fingers.
         */
        CursorFinder(
                const cv::Rect &mouseRect,
                const cv::Rect &screenRect,
                const cv::CascadeClassifier &cascade,
                float addChance=0.1,
                float minCtrProp=0.05,
                float handSizeProp=1,
                float kHandHeightProp=0.25,
                float minFingerDistProp=0.15
                );

        /*! Store the new mouse state given a hand contour (with possible arm
         * attached).
         *
         * @param ctr
         *      Contour of hand.
         * @param handSize
         *      The maximum estimated size of hand.
         * @param windowSize
         *      The size of the window containing contour.
         * @param out
         *      Where to store the new mouse state.
         *
         */
        void mouseStateFromContour(
                const Contour &ctr,
                const cv::Size &handSize,
                const cv::Size &windowSize,
                mouse::State &out
                ) const ;

        /*! Train the Adaboost classifier.
         *
         * @param negFrames
         *      Vector of frames in the BGR color format. Each must contain a
         *      face and no other skin.
         */
        void train(const vector<cv::Mat_<cv::Vec3b>> &negFrames);

        /*! Compute new mouse state from new frame and return true if new mouse
         * state was stored in `out` (hand was found). Frame must also contain face.
         *
         * @param frame
         *      Frame in which to search for hand, in BGR color format.
         * @param out
         *      Where to store the new mouse state, if hand is found.
         * @param skinMask
         *      Pointer to a mask in which to store the raw skin mask, if face
         *      is found.
         * @param withoutFaceAndSmall
         *      Pointer to a mask in which to store the skin mask with face
         *      contours and small contours removed, if any remain.
         * @param withLargestCtr
         *      Pointer to a mask in which to store the skin mask with only the
         *      largest contour drawn, if any remain.
         * @param foundFace
         *      If provided, stores whether face is found.
         */
        bool getMouseState(
                const cv::Mat_<cv::Vec3b> &frame,
                mouse::State &out,
                cv::Mat_<uchar> *skinMask=NULL,
                cv::Mat_<uchar> *withoutFaceAndSmall=NULL,
                cv::Mat_<uchar> *withLargestCtr=NULL,
                bool *foundFace=NULL
                );
    };
}
