#include "opencv2/core.hpp"
#include "opencv2/face.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core/utils/logger.hpp"// utils::logging::LOG_LEVEL_WARNING
#include <iostream>
#include <fstream>
#include <filesystem>
using namespace cv;
using namespace std;
//g++ -std=c++17 facerec.cpp -lopencv_face -lopencv_core -lopencv_imgcodecs

int width = 184;
int height = 224;

cv::Point p1(0,0);//where does the rectangle start at
cv::Point p2(width,height);//stores width and height values for rect
cv::Point txtp(100, 100);//where text goes in window
bool dragOn = false;
void onMouse(int event,int x,int y, int,void*) //detects mouse drag and moves rectangle to it
{
    if (event == EVENT_LBUTTONDOWN) {
        dragOn = true;
    }
    else if (event == EVENT_LBUTTONUP) {
        dragOn = false;
    }
    if (dragOn) {
        int wherex = x - (p2.x - p1.x) / 2;
        int wherey = y - (p2.y - p1.y) / 2;
        p1.x = wherex;
        p1.y = wherey;
        p2.x = wherex + width;
        p2.y = wherey + height;
    }
}

int main(int argc, char *argv[])
{
  namespace fs = std::filesystem;
 
  cv::Mat frame;
  double fps = 30;
  const char win_name[] = "Live Video...";

  cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);

  std::cout << "Wait 60 secs. for camera access to be obtained..." << std::endl;
  cv::VideoCapture vid_in(0);   // argument is the camera id

  if (vid_in.isOpened())
  {
      std::cout << "Camera capture obtained." << std::endl;
  }
  else
  {
      std::cerr << "error: Camera 0 could not be opened for capture.\n";
      return -1;
  }

  std::vector<cv::Mat> images;
  std::vector<int>     labels;

  

  // Iterate through all subdirectories, looking for .pgm files
  fs::path p(argc > 1 ? argv[1] : "../../att_faces");
  for (const auto &entry : fs::recursive_directory_iterator{ p }) {
    if (fs::is_regular_file(entry.status())) { // Was once always (wrongly) false in VS
      if (entry.path().extension() == ".pgm") {
        std::string str = entry.path().parent_path().stem().string(); // s26 s27 etc.
        int label = atoi(str.c_str() + 1); // s1 -> 1 (pointer arithmetic)
        images.push_back(cv::imread(entry.path().string().c_str(), cv::IMREAD_GRAYSCALE));
        labels.push_back(label);
      }
    }
  }
  
  cv::Ptr<cv::face::BasicFaceRecognizer> model = cv::face::EigenFaceRecognizer::create();
  model->train(images, labels);

  cv::namedWindow(win_name);
  setMouseCallback(win_name, onMouse, nullptr);
  while (1) {
      
      
      vid_in >> frame;//video aquired
      rectangle(frame, p1, p2, Scalar(128, 128, 128,90));//draws rectangle on window
      Rect rectangle(p1.x, p1.y, width, height);//used as a coordinate for mats
      Mat roi(frame,rectangle);//grabs the coords and makes a mat
      Mat greyroi;//used to grayscale before sending to predict
      cv::cvtColor(roi, greyroi, cv::COLOR_BGR2GRAY);//used for the predict
      cv::cvtColor(greyroi, roi, cv::COLOR_GRAY2BGR);//used for the effect display
      Mat resized;//used to store the resized image to send to predict
      resize(greyroi, resized, Size(92, 112), INTER_LINEAR);//resized the image and sends it to previous mat
      int predictedLabel;//int used to store what folder the prediction thinks it is
      double predictedconfidence;// double used to store the predicitons confidence
      model->predict(resized,predictedLabel,predictedconfidence);//predictor constructor
      if (predictedLabel == 41 || predictedLabel == 42 || predictedLabel == 43) // checks if the system thinks it is one of the team
      {
          std::cout << "group member face found" << std::endl;
      }
      std::string s = std::to_string(predictedLabel);//sending to string for display
      std::string s2 = std::to_string(predictedconfidence);//sending to string for display
      cv::putText(frame,"folder: " + s,txtp,cv::FONT_HERSHEY_COMPLEX_SMALL,1,CV_RGB(0,0,255),2);//displays which folder it thinks
      cv::putText(frame, "confidence: " + s2, txtp + Point(0,15), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, CV_RGB(0, 0, 255), 2);//displays confidence in prediction
      

      

      imshow(win_name, frame);
      if (cv::waitKey(60 / fps) >= 0) // how long to wait for a key (milliseconds)
          break;
  }

  return 0;
}
