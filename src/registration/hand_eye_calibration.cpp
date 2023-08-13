//==============================================================================
/*
    Software License Agreement (BSD License)
    Copyright (c) 2019-2022, AMBF
    (https://github.com/WPI-AIM/ambf)

    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
    copyright notice, this list of conditions and the following
    disclaimer in the documentation and/or other materials provided
    with the distribution.

    * Neither the name of authors nor the names of its contributors may
    be used to endorse or promote products derived from this software
    without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
    ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

    \author    <hishida3@jhu.edu>
    \author    Hisashi Ishida
*/
//==============================================================================

#include "hand_eye_calibration.h"

HandEyeCalibration::HandEyeCalibration(){

}

int cTransformToCVMat(cTransform trans, cv::Mat& Rot, cv::Mat& Trans){

    if (Rot.size().height == 3 && Rot.size().width == 3 && Trans.size().height == 3){
        cerr << "ERORR! Coversion in CV is impossible. Check the size of the matrix." << endl;
        return -1;
    }

    for (int i = 0 ; i < 3; i++){
        for (int j = 0 ; j < 3; j++){
            Rot.at<double>(i,j) = trans.getLocalRot()(i,j);
        }
        Trans.at<double>(i,0) = trans.getLocalPos()(i);
    }
    return 1;
}

int CVMatTocTransform(cTransform& trans, cv::Mat Rot, cv::Mat Trans){

    if (Rot.size().height == 3 && Rot.size().width == 3 && Trans.size().height == 3){
        cerr << "ERORR! Coversion in CV is impossible. Check the size of the matrix." << endl;
        return -1;
    }

    cMatrix3d cRot;
    cVector3d cTrans;
    for (int i = 0 ; i < 3; i++){
        for (int j = 0 ; j < 3; j++){
            cRot(i,j) = Rot.at<double>(i,j);
        }
        cTrans(i)= Trans.at<double>(i);
    }
   trans.setLocalRot(cRot);
   trans.setLocalPos(cTrans);

   return 1;
}


int HandEyeCalibration::calibrate(vector<cTransform> transEE, vector<cTransform> transMarker, cTransform& ee2marker, cTransform& tracker){
    if (transEE.size() != transMarker.size()){
        cerr << "HandEye Calibration ERROR! Sizes of the recorded data don't match!" << endl;
        return -1;
    }
    
    vector<cv::Mat> R_cam2marker;
	vector<cv::Mat> t_cam2marker;
	vector<cv::Mat> R_base2ee;
	vector<cv::Mat> t_base2ee;
	vector<cv::Mat> R_base2cam;
	vector<cv::Mat> t_base2cam;
	vector<cv::Mat> R_ee2marker;
	vector<cv::Mat> t_ee2marker;

    for (int i = 0; i < transEE.size(); i++){
        cv::Mat M_ee = cv::Mat_<double>(3,3);
        cv::Mat t_ee = cv::Mat_<double>(3,1);
        cv::Mat M_marker = cv::Mat_<double>(3,3);
        cv::Mat t_marker = cv::Mat_<double>(3,1);

        cTransformToCVMat(transEE[i], M_ee, t_ee);
        cTransformToCVMat(transMarker[i], M_marker, t_marker);

        R_cam2marker.push_back(M_marker);
        t_cam2marker.push_back(t_marker);

        R_base2ee.push_back(M_ee);
	    t_base2ee.push_back(t_ee);       
    }

    // Perform OpenCV function "calibrateRobotWorldHandEye()"
    cv::calibrateRobotWorldHandEye(R_cam2marker, t_cam2marker, R_base2ee, t_base2ee, R_base2cam, t_base2cam, R_ee2marker, t_ee2marker, 0);
    // cv::calibrateHandEye(R_cam2marker, t_cam2marker, R_base2ee, t_base2ee, R_base2cam, t_base2cam, R_ee2marker, t_ee2marker, 0);

    // CVMatTocTransform(ee2marker, R_ee2marker[0], t_ee2marker[0]);
    // CVMatTocTransform(tracker, R_base2cam[0], t_base2cam[0]);

    

    return 1;
}
