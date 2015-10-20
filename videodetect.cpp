#include<opencv2/opencv.hpp>
#include<iostream>
#include<vector>

int main(int argc, char *argv[])
{
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " in.file out.file" << std::endl;
        return -1;
    }

    cv::Mat frame;
    cv::Mat back;
    cv::Mat fore;
    std::cerr << "opening " << argv[1] << std::endl;
    cv::VideoCapture cap(argv[1]);
    cv::BackgroundSubtractorMOG2 bg;
    //bg.nmixtures = 3;
    //bg.bShadowDetection = false;

    cv::VideoWriter output;
    //int ex = static_cast<int>(cap.get(CV_CAP_PROP_FOURCC));
    int ex = CV_FOURCC('P','I','M','1');
    cv::Size size = cv::Size((int) cap.get(CV_CAP_PROP_FRAME_WIDTH),
                             (int) cap.get(CV_CAP_PROP_FRAME_HEIGHT));
    std::cerr << "saving to " << argv[2] << std::endl;
    output.open(argv[2], ex, cap.get(CV_CAP_PROP_FPS), size, true);

    std::vector<std::vector<cv::Point> > contours;

    cv::namedWindow("Frame");
    cv::namedWindow("Fore");
    cv::namedWindow("Background");


    cv::SimpleBlobDetector::Params params;
    params.minThreshold = 40;
    params.maxThreshold = 60;
    params.thresholdStep = 5;
    params.minArea = 100; 
    params.minConvexity = 0.3;
    params.minInertiaRatio = 0.01;
    params.maxArea = 8000;
    params.maxConvexity = 10;
    params.filterByColor = false;
    params.filterByCircularity = false;


    cv::SimpleBlobDetector blobDtor(params);
    blobDtor.create("SimpleBlob");

    std::vector<std::vector<cv::Point> >    blobContours;
    std::vector<cv::KeyPoint>               keyPoints;
    cv::Mat                                 out;

    cv::HOGDescriptor hog;
    hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());


    for(;;)
    {
        cap >> frame;

        bg.operator ()(frame, fore);

        bg.getBackgroundImage(back);
        cv::erode(fore, fore, cv::Mat());
        cv::dilate(fore, fore, cv::Mat());

        blobDtor.detect(fore, keyPoints, cv::Mat());

        //cv::imshow("Fore", fore);

        cv::findContours(fore, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
        cv::drawContours(frame, contours, -1, cv::Scalar(0,0,255), 2);

        std::vector<std::vector<cv::Point> >::const_iterator it = contours.begin();
        std::vector<std::vector<cv::Point> >::const_iterator end = contours.end();
        while (it != end) {
            cv::Rect bounds = cv::boundingRect(*it);
            cv::rectangle(frame, bounds, cv::Scalar(0,255,0), 2);

            ++it;
        }

        cv::drawKeypoints(fore, keyPoints, out, CV_RGB(0,255,0), cv::DrawMatchesFlags::DEFAULT);
        cv::imshow("Fore", out);


        std::vector<cv::Rect> found, found_filtered;
        hog.detectMultiScale(frame, found, 0, cv::Size(8,8), cv::Size(32,32), 1.05, 2);
        for (int i = 0; i < found.size(); ++i) {
            cv::Rect r = found[i];
            int j = 0;
            for (; j < found.size(); ++j) {
                if (j != i && (r & found[j]) == r) {
                    break;
                }
            }
            if (j == found.size()) {
                found_filtered.push_back(r);
            }
        }

        for (int i = 0; i < found_filtered.size(); ++i) {
            cv::Rect r = found_filtered[i];
            cv::rectangle(frame, r.tl(), r.br(), cv::Scalar(255,0,0), 3);
        }


        output << frame;

        cv::resize(frame, frame, cv::Size(1280, 720));
        cv::imshow("Frame", frame);

        cv::resize(back, back, cv::Size(1280, 720));
        cv::imshow("Background", back);



        if(cv::waitKey(30) >= 0) break;
    }
    return 0;
}