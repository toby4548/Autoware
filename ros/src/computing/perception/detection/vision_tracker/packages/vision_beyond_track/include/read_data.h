/*
 *  Copyright (c) 2018, Tokyo University
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  * Neither the name of Autoware nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ********************
 *  v1.0: Yuki Tsuji (yukitsuji020832@gmail.com)
 *
 *  Created on: Aug 8th, 2018
 */

#ifndef READ_DATA_H
#define READ_DATA_H

#include <string>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <iostream>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <vector>
#include <boost/foreach.hpp>
#include "boost/filesystem.hpp"
#include <boost/format.hpp>

using namespace std;

namespace beyondtrack {
  cv::Mat read_calib(string path, int seqNo){
    ifstream ifs(path);
    int i = 0;

    if (ifs.fail()) {
      std::cerr << "Failed to read the specified calibration file\n";
      exit(0);
    }

    string str_values;
    while (getline(ifs, str_values)) {
      if (i == seqNo) {
        break;
      }
      i++;
    }

    std::vector<std::string> str_vec;
    std::vector<double> double_vec;
    boost::split(str_vec, str_values, boost::is_any_of(" "));
    for (int i=0; i<12; i++) {
      if ((i+1) % 4 == 0) continue;
      double f = std::stod(str_vec[i]);
      double_vec.push_back(f);
    }

    cv::Mat k_ = cv::Mat::zeros(3, 3, CV_64FC1);
    memcpy(k_.data, double_vec.data(), double_vec.size()*sizeof(double));
    return k_;
  }

  vector<vector<vector<double>>> read_detection(string dir) {
    namespace fs = boost::filesystem;
    const fs::path dir_path(dir);
    int file_count = 0;
    BOOST_FOREACH(const fs::path& p, std::make_pair(fs::directory_iterator(dir_path),
                                                    fs::directory_iterator())) {
        if (!fs::is_directory(p)) file_count++;
    }

    vector<vector<vector<double>>> all_detections;
    for (int i=0; i<file_count; ++i) {
      string path = dir + (boost::format("%06d.txt") % i).str();
      ifstream ifs(path);

      if (ifs.fail()) {
        std::cerr << "Failed to read the detection file\n";
        exit(0);
      }

      vector<string> raw_vec;
      vector<vector<double>> per_frame_detection;
      string col_str;
      while (getline(ifs, col_str)) {
        std::vector<std::string> str_vec;
        std::vector<double> double_vec;
        boost::split(str_vec, col_str, boost::is_any_of(" "));
        for (int j=0; j<6; ++j) {
          double f = std::stod(str_vec[j]);
          double_vec.push_back(f);
        }
        per_frame_detection.push_back(double_vec);
      }
      all_detections.push_back(per_frame_detection);
    }
    return all_detections;
  }

  vector<cv::Mat> read_pose(string path){
    ifstream ifs(path);

    if (ifs.fail()) {
      std::cerr << "Failed to read the specified pose file\n";
      exit(0);
    }

    vector<string> raw_vec;
    vector<cv::Mat> pose_list;
    string col_str;
    while (getline(ifs, col_str)) {
      string str_values = col_str;
      std::vector<std::string> str_vec;
      std::vector<double> double_vec;

      boost::split(str_vec, str_values, boost::is_any_of(" "));
      for (int j=10; j<13; ++j) {
        double f = std::stod(str_vec[j]);
        double_vec.push_back(f);
      }
      double_vec.push_back(0.0);
      cv::Mat pose = cv::Mat::zeros(1, 4, CV_64FC1);
      memcpy(pose.data, double_vec.data(), double_vec.size()*sizeof(double));

      pose_list.push_back(pose);
    }
    return pose_list;
  }

  vector<string> read_img(string dir) {
    namespace fs = boost::filesystem;
    const fs::path dir_path(dir);
    int file_count = 0;
    BOOST_FOREACH(const fs::path& p, std::make_pair(fs::directory_iterator(dir_path),
                                                    fs::directory_iterator())) {
        if (!fs::is_directory(p)) file_count++;
    }

    vector<string> img_list;
    for (int i=0; i<file_count; ++i) {
      string path = dir + (boost::format("%06d.png") % i).str();
      img_list.push_back(path);
    }
    return img_list;
  }
}
#endif
