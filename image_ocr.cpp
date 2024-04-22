
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
    // �̹��� ���� ��� ����
    std::string image_path = "test.png";

    // �̹��� ���� �о����
    cv::Mat original_image = cv::imread(image_path);

    // �̹����� ���������� �о����� Ȯ��
    if (original_image.empty()) {
        std::cerr << "�̹����� ���� �� �����ϴ�." << std::endl;
        return -1;
    }

    // �̹��� ũ�� ����
    cv::resize(original_image, original_image, cv::Size(), 2.0, 2.0); // ���÷� �̹����� 2�� Ȯ���߽��ϴ�.

    // �̹����� �׷��̽����Ϸ� ��ȯ
    cv::Mat gray_image;
    cv::cvtColor(original_image, gray_image, cv::COLOR_BGR2GRAY);

    // �̹��� ����ȭ
    cv::Mat binary_image;
    cv::threshold(gray_image, binary_image, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    // MSER ��ü ����
    cv::Ptr<cv::MSER> mser = cv::MSER::create();

    // MSER �˰����� �̿��Ͽ� �ؽ�Ʈ ���� ����
    std::vector<std::vector<cv::Point>> regions;
    std::vector<cv::Rect> rects;
    mser->detectRegions(binary_image, regions, rects);

    // ����� �ؽ�Ʈ ������ �׸�ڽ� �׸���
    for (size_t i = 0; i < rects.size(); ++i) {
        cv::rectangle(original_image, rects[i], cv::Scalar(0, 255, 0), 2);
    }

    // �ؽ�Ʈ ������ ǥ�õ� �̹��� ���
    cv::imshow("�ؽ�Ʈ ���� ���� ���", original_image);

    // Tesseract OCR �ʱ�ȭ �� �ؽ�Ʈ ����
    const char* input_image = "test.png";
    const char* dataPath = "C:/Program Files/Tesseract-OCR/tessdata";

    // tesseract api ����
    tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
    if (api->Init(dataPath, "kor")) {
        return -1;
    }

    // �̹��� ����
    Pix* image = pixRead(input_image);
    api->SetImage(image);

    std::string utf_text = api->GetUTF8Text();
    std::string text = UTF8ToANSI(utf_text.c_str());

    std::cout << text << std::endl;

    // ���⼭ �ؽ�Ʈ ����� �����ϸ� �˴ϴ�.
    // ����� �ؽ�Ʈ�� ���ϴ� ������� ����Ͽ� ����ϸ� �˴ϴ�.

    // Ű �Է� ���
    cv::waitKey(0);

    return 0;
}
