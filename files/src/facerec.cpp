#include "opencv2/core.hpp"
#include "opencv2/face.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core/utils/logger.hpp" // utils::logging::LOG_LEVEL_WARNING
#include <iostream>
#include <fstream>
#include <filesystem>
using namespace cv;
using namespace std;
//g++ -std=c++17 facerec.cpp -lopencv_face -lopencv_core -lopencv_imgcodecs

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
  /*
  // Randomly choose an image, and remove it from the main collection
  std::srand(std::time(0));
  int rand_image_id = std::rand() % images.size();
  cv::Mat testSample = images[rand_image_id];
  int     testLabel  = labels[rand_image_id];
  images.erase(images.begin() + rand_image_id);
  labels.erase(labels.begin() + rand_image_id);
  std::cout << "Actual class    = " << testLabel << '\n';
  std::cout << " training...";
  */

  cv::Ptr<cv::face::BasicFaceRecognizer> model = cv::face::EigenFaceRecognizer::create();
  model->train(images, labels);


  //int predictedLabel = model->predict(testSample);
  //std::cout << "\nPredicted class = " << predictedLabel << '\n';
  
  
  cv::Point p1(260, 180);
  cv::Point p2(260+184, 180+224);
  cv::namedWindow(win_name);
  while (1) {
      
      
      vid_in >> frame;
      Mat roi(frame,Rect(260, 180, 184, 224));
      rectangle(frame, p1, p2, Scalar(0, 255, 0));
      //int predictedLabel = model->predict(roi);
    //  std::cout << "\nPredicted class = " << predictedLabel << '\n';
      Mat greyroi;
      cv::cvtColor(roi, greyroi, cv::COLOR_BGR2GRAY);
      Mat resized;
      resize(greyroi, resized, Size(92, 112), INTER_LINEAR);
      int predictedLabel = model->predict(resized);
      std::cout << "face is predicted to be from folder....." << predictedLabel << "\n"<<std::endl;
     // std::cout << "\nPredicted class = " << predictedLabel << '\n';
      //cv::flip(frame, flippedimage,1);
      imshow(win_name, frame);
      if (cv::waitKey(60 / fps) >= 0) // how long to wait for a key (milliseconds)
          break;
  }

  return 0;
}
