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

#include <cstdlib>
#include <cmath>
#include <climits>

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <opencv2/opencv.hpp>

#include "cvutils.hpp"
#include "seq.hpp"
#include "krandom.hpp"
#include "kmath.hpp"
#include "mouse.hpp"
#include "io.hpp"

#include "humancv.hpp"

using namespace std;
using namespace cv;
using namespace seq::functional;
using seq::math::xrange;
using namespace io;
using namespace humancv;

ostream &operator<<(ostream &out, const FingerData &f) {
    out << "<FingerData i=" << f.i << " leftI=" << f.leftI << " rightI=" << f.rightI << ">";
    return out;
}

void face::storeMasks(const Size imSize, const vector<Rect> &rois, Mat_<uchar> &dest) {
    dest = Mat_<uchar>::zeros(imSize);

    for (auto roi : rois) {
        int w = roi.width, h = roi.height;
        ellipse(dest, roi.tl() + Point(w / 2, h / 2), Size(int(w * 0.8 / 2), int(h * 1.0 / 2)), 0, 0, 360, 255, CV_FILLED);
        rectangle(dest, Rect(roi.x + int(w * 0.25), roi.y + int(0.8 * h), int(w * 0.5), int(h * 0.4)), 255, CV_FILLED);
    }
}

void face::getRects(
        const Mat_<uchar> &im,
        CascadeClassifier &cascade,
        vector<Rect> &out,
        float scale
        ) {
    Mat scaled;
    resize(im, scaled, Size(), scale, scale);

    cascade.detectMultiScale(scaled, out);

    for (auto &rect : out) {
        rect.x = int(float(rect.x) / scale);
        rect.y = int(float(rect.y) / scale);
        rect.width = int(float(rect.width) / scale);
        rect.height = int(float(rect.height) / scale);
    }

    sort(
            out.begin(),
            out.end(),
            [](Rect a, Rect b) { return a.area() > b.area(); }
            );
}

void skin::getMask(
        const Mat_<Vec3b> &im,
        const CvBoost &cls,
        Mat_<uchar> &dest,
        SparseMat_<float> *lookup,
        float thres
        )
{
    dest.create(im.size());
    dest = 0;

    Mat_<float> samp(1, 3);

    if (lookup == NULL) {
        SparseMat_<float> temp(3, (int []){256, 256, 256});
        lookup = &temp;
    }

    float sum;
    uchar p1, p2, p3;
    cvutils::applyBinaryOp(
            [&](int row, int col, const Vec3b *px, uchar *destPx) {
                p1 = (*px)[0];
                p2 = (*px)[1];
                p3 = (*px)[2];
                float *sumPtr = (float *)((*lookup).ptr(p1, p2, p3, false));
                if (sumPtr == NULL) {
                    samp(0, 0) = p1;
                    samp(0, 1) = p2;
                    samp(0, 2) = p3;

                    sum = cls.predict(samp, Mat(), Range::all(), false, true);
                    sumPtr = &sum;

                    (*lookup).ref(p1, p2, p3) = sum;
                }

                if (*sumPtr > thres) {
                    *destPx = 255;
                }
            },
            im,
            dest
            );
//        imshow("init skin mask", dest);
//        waitForKeypress();
}

/*! Given a contour and a hand size (must be guaranteed to contain whole hand),
 * return a vector of FingerData.
 *
 * `k` is the number to use for k-curvature computation.
 * `minDist` is the minimum distance between finger peaks.
 * `fingerAngle` is the maximum angle (curvature) a finger can have.
 */
void humancv::getFingers(
        const Contour &ctr,
        const Size &handSize,
        vector<FingerData> &out,
        int k,
        float minDist,
        float fingerAngle
        )
{
    Point curPt;
    Point leftPt;
    Point rightPt;

    Point highestPt(0, INT_MAX);

    vector<FingerData> rawFingers;

    // Store points with angle less than `fingerAngle`, and also find highest
    // point.
    for (int i : xrange(int(ctr.size()))) {
        curPt = ctr[i];
        if (curPt.y < highestPt.y) {
            highestPt = curPt;
        }

        try {
            leftPt = ctr[i - k];
            rightPt = ctr[i + k];
        } catch (out_of_range &err) {
            continue;
        }

        float angle = cvutils::geom::ptAngle(leftPt, curPt, rightPt);
        if (    angle < fingerAngle
                and angle > 0
                and curPt.y < leftPt.y
                and curPt.y < rightPt.y) {
            // Crawl down to get ends of finger.
            //
            // {right, left}I is the index further down the finger (further
            // away from the tip index).
            int rightI = i + k;
            int leftI = i - k;
            while (rightI - leftI < (int)ctr.size()) {
                Vec2i v1(ctr[rightI] - ctr[rightI - k / 2]);
                Vec2i v2(ctr[leftI] - ctr[leftI - k / 2]);

                if (fabs(cvutils::geom::vecAngle(v1, v2)) > 1) {
                    rawFingers.push_back(FingerData{
                            ctr.realI(i),
                            ctr.realI(leftI),
                            ctr.realI(rightI)
                            });
                    break;
                }
                rightI++;
                leftI--;
            }
        }
    }

    // Filter by proximity to highest point (assuming `handSize` contains
    // highest point).
    filter(
            [&](const FingerData &fData) {
                return ctr[int(fData.i)].y < highestPt.y + handSize.height;
            },
            rawFingers,
            rawFingers
          );

    vector<vector<FingerData>> clusters;

    // Cluster fingers by spatial distance (max distance `k`).
    cvutils::cluster(
            rawFingers,
            [&](const FingerData &f1, const FingerData &f2) {
                return cvutils::geom::ptNormSqrd(ctr[int(f1.i)] - ctr[int(f2.i)]);
            },
            minDist * minDist,
            clusters
           );

//    print(clusters);

    // Take the middle of each cluster as a representative `FingerData`.
    out.resize(clusters.size());
    for (auto i : xrange(clusters.size())) {
        out[i] = clusters[i][clusters[i].size() / 2];
    }

//    print(out);
}

CursorFinder::CursorFinder(
            const Rect &mouseRect,
            const Rect &screenRect,
            const CascadeClassifier &cascade,
            float addChance,
            float minCtrProp,
            float handSizeProp,
            float kHandHeightProp,
            float minFingerDistProp
            )
        : cascade(cascade), addChance(addChance), minCtrProp(minCtrProp),
        handSizeProp(handSizeProp), kHandHeightProp(kHandHeightProp),
        minFingerDistProp(minFingerDistProp),
        pxToPrediction(3, (int []){256, 256, 256}), thres(0), kFilter(4, 2, 0),
        screenRect(screenRect), mouseRect(mouseRect) {
    this->kFilter.transitionMatrix = (Mat_<float>(4, 4) << 1,0,1,0, 0,1,0,1, 0,0,1,0, 0,0,0,1);
    setIdentity(this->kFilter.measurementMatrix);
    setIdentity(this->kFilter.measurementNoiseCov, 0.3);
    setIdentity(this->kFilter.processNoiseCov, 0.0001);
    setIdentity(this->kFilter.errorCovPost, 0.1);
}

void CursorFinder::mouseStateFromContour(
        const Contour &ctr,
        const Size &handSize,
        const Size &windowSize,
        mouse::State &out
        ) const {
    vector<FingerData> fingers;

    int k = int(this->kHandHeightProp * float(handSize.height)); // for k-curvatures
    float minFingerDist = this->minFingerDistProp * float(handSize.height);
    getFingers(ctr, handSize, fingers, k, minFingerDist);

    Mat_<Vec3b> temp = Mat_<Vec3b>::zeros(windowSize);
    for (auto f : fingers) {
        circle(temp, ctr[int(f.i)], 3, Scalar(255, 255, 255));
        circle(temp, ctr[int(f.leftI)], 3, Scalar(0, 0, 255));
        circle(temp, ctr[int(f.rightI)], 3, Scalar(0, 255, 0));
    }
    imshow("fingers", temp);

    switch (fingers.size()) {
        case 0:
            out.btn = mouse::State::LEFT_DOWN;
            break;
        case 1:
            out.btn = 0;
            break;
        case 2:
            out.btn = mouse::State::MIDDLE_DOWN;
            break;
        default:
            // assume there's some error and just say it's normal navigation
            out.btn = 0;
            break;
    }

    // flag which points in contour belong to finger `(shouldCopy[i] ==
    // false)`
    vector<bool> shouldCopy(ctr.size(), true);
    for (auto finger : fingers) {
        if (finger.leftI < finger.rightI) {
            for (size_t i = finger.leftI; i < finger.rightI; i++) {
                shouldCopy[i] = false;
            }
        } else {
            for (size_t i = finger.leftI; i < ctr.size(); i++) {
                shouldCopy[i] = false;
            }
            for (unsigned i = 0; i < finger.rightI; i++) {
                shouldCopy[i] = false;
            }
        }
    }

    // copy points over
    vector<Point> withoutFingers;
    for (auto i : xrange(ctr.size())) {
        if (shouldCopy[i]) {
            withoutFingers.push_back(ctr[int(i)]);
        }
    }

    // draw it on skin mask
    Mat_<uchar> skinMask = Mat_<uchar>::zeros(windowSize);
    drawContours(skinMask, vector<vector<Point>>{withoutFingers}, 0, 255, CV_FILLED);

//    imshow("mask without finger", skinMask);

    // find highest point
    Point highestPt = *min_element(
            withoutFingers.begin(),
            withoutFingers.end(),
            [](Point a, Point b) {
                return a.y < b.y;
            }
            );

    // get approx bounding rect of hand (without fingers)
    Rect handBounds(
            Point(highestPt.x - int(0.5 * handSize.width), highestPt.y),
            handSize
            );
    handBounds &= Rect(Point(0, 0), windowSize); // bound to screen

    Mat handMask(skinMask(handBounds));

    // get center point
    Moments ms = moments(handMask, true);
    Point2f windowPt(ms.m10 / ms.m00, ms.m01 / ms.m00);
    windowPt += Point2f(handBounds.tl().x, handBounds.tl().y);

    print(windowPt, this->mouseRect, this->screenRect);
    // project it to screen coordinates
    out.pos = kmath::PointI(
            kmath::intervalProject(
                windowPt.x,
                kmath::Interval(
                    this->mouseRect.x,
                    this->mouseRect.x + this->mouseRect.width
                ),
                kmath::Interval(
                    this->screenRect.x,
                    this->screenRect.x + this->screenRect.width - 1
                )
            ),
            kmath::intervalProject(
                windowPt.y,
                kmath::Interval(
                    this->mouseRect.y,
                    this->mouseRect.y + this->mouseRect.height
                ),
                kmath::Interval(
                    this->screenRect.y,
                    this->screenRect.y + this->screenRect.height - 1
                )
            ));
}

void CursorFinder::train(const vector<Mat_<Vec3b>> &negFrames)
{
    vector<Mat_<Vec3b>> negYcrcbFrames(negFrames.size());
    vector<Mat_<uchar>> masks(negFrames.size());

    printImm("converting frames and storing face masks...");
    for (auto i : xrange(negFrames.size())) {
        // store ycrcb frame
        cvtColor(negFrames[i], negYcrcbFrames[i], CV_BGR2YCrCb);

        // find faces and store mask
        cvtColor(negFrames[i], gray, CV_BGR2GRAY);

        face::getRects(gray, this->cascade, this->faceRects, min(300.0f / negFrames[0].cols, 1.f));

        if (this->faceRects.empty()) {
            print("no face detected, frame", i);
        }

        face::storeMasks(negFrames[0].size(), this->faceRects, masks[i]);
//        imshow("im", negFrames[i]);
//        imshow("mask", masks[i]);
//        waitForKeypress();
    }
    print("done");

    vector<float> samples;
    vector<float> resps;

    printImm("creating samples...");
    auto negIt = negYcrcbFrames.begin();
    auto maskIt = masks.begin();
    for (; negIt != negYcrcbFrames.end(); negIt++, maskIt++) {
        auto &negFrame = *negIt;
        auto &mask = *maskIt;

        unsigned added = 0;
        cvutils::applyBinaryOp(
                [&](int row, int col, const Vec3b *px, const uchar *maskPx) {
                    if (krandom::random() < this->addChance and *maskPx) {
                        samples.push_back((*px)[0]);
                        samples.push_back((*px)[1]);
                        samples.push_back((*px)[2]);
                        added++;
                    }
                },
                negFrame,
                mask
                );
        resps.insert(resps.end(), added, 1);

        Mat_<uchar> temp;
        bitwise_not(mask, temp);
        erode(temp, temp, Mat(), Point(-1, -1), 3);

        added = 0;
        cvutils::applyBinaryOp(
                [&](int row, int col, const Vec3b *px, const uchar *maskPx) {
                    if (krandom::random() < this->addChance and *maskPx) {
                        samples.push_back((*px)[0]);
                        samples.push_back((*px)[1]);
                        samples.push_back((*px)[2]);
                        added++;
                    }
                },
                negFrame,
                temp
                );
        resps.insert(resps.end(), added, 0);
    }
    print(samples.size(), "samples,", resps.size(), "resps");

    Mat_<float> sampMat(samples);
    Mat_<float> respMat(resps);
    sampMat = sampMat.reshape(int(samples.size() / 3));

    Mat varTypes(4, 1, CV_8UC1);
    varTypes = Scalar(CV_VAR_NUMERICAL);
    varTypes.at<uchar>(3, 0) = CV_VAR_CATEGORICAL;

    CvBoostParams params(CvBoost::GENTLE, 100, 0.95, 1, false, NULL);
    printImm("training classifier...");
    this->boost.train(sampMat, CV_ROW_SAMPLE, respMat, Mat(), Mat(), varTypes, Mat(), params);
    print("done");

//    for (this->thres = 0; ; this->thres -= 0.05) {
//        print("testing threshold", this->thres);
//        if (any(
//                [&](const Mat_<Vec3b> &frame) {
//                    mouse::State tempState;
//                    return this->getMouseState(frame, tempState);
//                },
//                negFrames
//               )) {
//            this->thres += 0.1;
//            break;
//        }
//    }
}

bool CursorFinder::getMouseState(
        const Mat_<Vec3b> &frame,
        mouse::State &out,
        Mat_<uchar> *skinMask,
        Mat_<uchar> *withoutFaceAndSmall,
        Mat_<uchar> *withHighestCtr,
        bool *foundFace
        )
{
    cvtColor(frame, this->gray, CV_BGR2GRAY);
    cvtColor(frame, this->ycrcb, CV_BGR2YCrCb);

    face::getRects(this->gray, this->cascade, this->faceRects, min(300.0f / frame.cols, 1.f));

    if (this->faceRects.empty()) {
        // face not found
        if (foundFace != NULL) {
            *foundFace = false;
        }
        return false;
    }
    if (foundFace != NULL) {
        *foundFace = true;
    }

    Rect maxFaceRect = this->faceRects[0];

    skin::getMask(this->ycrcb, this->boost, this->skinMask, &(this->pxToPrediction), this->thres);

    if (skinMask != NULL) {
        *skinMask = this->skinMask.clone();
    }

    // erodilate
    cvutils::dilerode(this->skinMask, this->skinMask, 2);
    cvutils::erodilate(this->skinMask, this->skinMask, 1);

    // filter by in face + small
    Mat tempMat = this->skinMask;
    findContours(tempMat, this->skinContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    filter(
            [&](const vector<Point> &ctr) {
                if (contourArea(ctr) < this->minCtrProp * maxFaceRect.area()) {
                    return false;
                }
                for (auto &r : this->faceRects) {
                    if (any([&](Point pt) { return r.contains(pt); }, ctr)) {
                        return false;
                    }
                }
                return true;
            },
            this->skinContours,
            this->skinContours
            );

    if (this->skinContours.empty()) {
        return false;
    }

    if (withoutFaceAndSmall != NULL) {
        // draw them if needed
        this->skinMask = 0;
        drawContours(this->skinMask, this->skinContours, -1, 255, CV_FILLED);

        *withoutFaceAndSmall = this->skinMask.clone();
    }

//    Mat fg, bg;
//    binMaskToGCMask(this->skinMask, this->skinMask);
//    grabCut(frame, this->skinMask, Rect(), bg, fg, 1, GC_INIT_WITH_MASK);
//    gcMaskToBinMask(this->skinMask, this->skinMask);
//
//    imshow("4 after gc", this->skinMask);

    // find the contour with highest center of mass
    seq::ElemView<vector<vector<Point>>> maxCtrView(
            max_element(
                this->skinContours.begin(),
                this->skinContours.end(),
                [&](const vector<Point> &a, const vector<Point> &b) {
                    Moments ms = moments(a);
                    Point2f aPt(float(ms.m10 / ms.m00), float(ms.m01 / ms.m00));
                    ms = moments(b);
                    Point2f bPt(float(ms.m10 / ms.m00), float(ms.m01 / ms.m00));
                    return aPt.y > bPt.y;
                }
                ),
            this->skinContours.begin()
            );


    if (withHighestCtr != NULL) {
        // draw max contour on mask if needed
        this->skinMask = 0;
        drawContours(
                this->skinMask,
                this->skinContours,
                int(maxCtrView.i),
                255,
                CV_FILLED
                );

        *withHighestCtr = this->skinMask.clone();
    }

    // get new state
    this->mouseStateFromContour(
            Contour(maxCtrView.e),
            maxFaceRect.size() * this->handSizeProp,
            frame.size(),
            out
            );

    this->kFilter.predict();
    Mat_<float> corrected = this->kFilter.correct((Mat_<float>(2, 1) << out.pos.x, out.pos.y));
    out.pos.x = int(kmath::Interval(0, float(this->screenRect.width - 1)).closest(corrected(0)));
    out.pos.y = int(kmath::Interval(0, float(this->screenRect.height - 1)).closest(corrected(1)));

    return true;
}
