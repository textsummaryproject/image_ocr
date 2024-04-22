
#include <opencv2/opencv.hpp>
#include <windows.h>
#include <iostream>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX

char* UTF8ToANSI(const char* pszCode)
{
    BSTR    bstrWide;
    char* pszAnsi;
    int     nLength;

    // Get nLength of the Wide Char buffer
    nLength = MultiByteToWideChar(CP_UTF8, 0, pszCode, strlen(pszCode) + 1,
        NULL, NULL);
    bstrWide = SysAllocStringLen(NULL, nLength);

    // Change UTF-8 to Unicode (UTF-16)
    MultiByteToWideChar(CP_UTF8, 0, pszCode, strlen(pszCode) + 1, bstrWide, nLength);

    // Get nLength of the multi byte buffer
    nLength = WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, NULL, 0, NULL, NULL);
    pszAnsi = new char[nLength];

    // Change from unicode to mult byte
    WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, pszAnsi, nLength, NULL, NULL);
    SysFreeString(bstrWide);

    return pszAnsi;
}

int main() {
    // 이미지 파일 경로 설정
    std::string image_path = "test.png";

    // 이미지 파일 읽어오기
    cv::Mat original_image = cv::imread(image_path);

    // 이미지를 성공적으로 읽었는지 확인
    if (original_image.empty()) {
        std::cerr << "이미지를 읽을 수 없습니다." << std::endl;
        return -1;
    }

    // 이미지 크기 조정
    cv::resize(original_image, original_image, cv::Size(), 2.0, 2.0); // 예시로 이미지를 2배 확대했습니다.

    // 이미지를 그레이스케일로 변환
    cv::Mat gray_image;
    cv::cvtColor(original_image, gray_image, cv::COLOR_BGR2GRAY);

    // 이미지 이진화
    cv::Mat binary_image;
    cv::threshold(gray_image, binary_image, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    // MSER 객체 생성
    cv::Ptr<cv::MSER> mser = cv::MSER::create();

    // MSER 알고리즘을 이용하여 텍스트 영역 검출
    std::vector<std::vector<cv::Point>> regions;
    std::vector<cv::Rect> rects;
    mser->detectRegions(binary_image, regions, rects);

    // 검출된 텍스트 영역에 네모박스 그리기
    for (size_t i = 0; i < rects.size(); ++i) {
        cv::rectangle(original_image, rects[i], cv::Scalar(0, 255, 0), 2);
    }

    // 텍스트 영역이 표시된 이미지 출력
    cv::imshow("텍스트 영역 검출 결과", original_image);

    // Tesseract OCR 초기화 및 텍스트 추출
    const char* input_image = "test.png";
    const char* dataPath = "C:/Program Files/Tesseract-OCR/tessdata";

    // tesseract api 설정
    tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
    if (api->Init(dataPath, "kor")) {
        return -1;
    }

    // 이미지 설정
    Pix* image = pixRead(input_image);
    api->SetImage(image);

    std::string utf_text = api->GetUTF8Text();
    std::string text = UTF8ToANSI(utf_text.c_str());

    std::cout << text << std::endl;

    // 여기서 텍스트 요약을 수행하면 됩니다.
    // 추출된 텍스트를 원하는 방식으로 요약하여 출력하면 됩니다.

    // 키 입력 대기
    cv::waitKey(0);

    return 0;
}
