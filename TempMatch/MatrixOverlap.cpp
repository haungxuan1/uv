#include "pch.h"
#include "MatrixOverlap.h"
#include <vector>
#include <algorithm>

namespace MatrixOverlap
{
    bool MatchPoint(
        double &dSame,
        cv::Mat& cTar,
        cv::Mat& cTmp,
        std::vector<cv::Point2f>& vTar,
        std::vector<cv::Point2f>& vTmp);

    void MatchPoint(
        cv::Mat& cTar,
        cv::Mat& cTmp,
        std::vector<cv::KeyPoint>& vTar,
        std::vector<cv::KeyPoint>& vTmp,
        std::vector<std::vector<cv::DMatch>>& vMatche);

    bool ChooseFeaturePoint(
        double &dSame,
        std::vector<cv::KeyPoint>& vTarKey,
        std::vector<cv::KeyPoint>& vTmpKey,
        std::vector<std::vector<cv::DMatch>>& vMatche,
        std::vector<cv::DMatch>& vGood,
        std::vector<cv::Point2f>& vTar,
        std::vector<cv::Point2f>& vTmp);

    bool Overlap(
        std::vector<cv::Point2f>& cTar,
        std::vector<cv::Point2f>& cTmp,
        POINT& ptTopLeft);

    cv::Ptr<cv::SIFT> g_lpSIFT; // ORB

    cv::Ptr<cv::FlannBasedMatcher> g_lpMatcher; // BFMatcher

    std::vector<cv::KeyPoint> g_vTarKey;

    cv::Mat g_cTar, g_cTarDescriptor;

    void Reg()
    {
        g_lpSIFT = cv::SIFT::create();

        g_lpMatcher = cv::FlannBasedMatcher::create();
    }

    void Unreg()
    {

    }

    void SetTar(cv::Mat& cTar, int nMaxFeatures)
    {
        g_vTarKey.clear();

        g_cTar = cTar;

        g_lpSIFT->setNFeatures(nMaxFeatures);

        g_lpSIFT->detectAndCompute(cTar, cv::Mat(), g_vTarKey, g_cTarDescriptor);

        g_cTarDescriptor.convertTo(g_cTarDescriptor, CV_32F);
    }

	bool Overlap(cv::Mat& cTemp, POINT& ptTopLeft, double &dSame)
	{
        std::vector<cv::Point2f> vTar, vTemp; // ∆•≈‰Ãÿ’˜µ„Œª÷√

        return (MatchPoint(dSame, g_cTar, cTemp, vTar, vTemp) && Overlap(vTar, vTemp, ptTopLeft));
	}

    bool Overlap(
        std::vector<cv::Point2f>& cTar,
        std::vector<cv::Point2f>& cTmp,
        POINT& ptTopLeft)
    {
        if (cTar.size() < 4 || cTmp.size() < 4) return false;

        cv::Mat H = findHomography(cTar, cTmp, cv::RANSAC);

        if (H.empty()) return false;

        cv::Vec3d vHomogeneous = { 0.0, 0.0, 1.0 };

        auto temp = (cv::Mat)(H * vHomogeneous);

        cv::Vec3d point = temp.col(0);

        auto& x = point[0];

        auto& y = point[1];

        auto& z = point[2];

        ptTopLeft.x = cvRound(x / z);

        ptTopLeft.y = cvRound(y / z);

        return true;
    }

    bool MatchPoint(
        double &dSame,
        cv::Mat& cTar,
        cv::Mat& cTmp,
        std::vector<cv::Point2f>& vTar,
        std::vector<cv::Point2f>& vTmp)
    {
        std::vector<cv::DMatch> vGood;

        std::vector<std::vector<cv::DMatch>> vMatche;

        std::vector<cv::KeyPoint> vTmpKey;

        MatchPoint(cTar, cTmp, g_vTarKey, vTmpKey, vMatche);

        return ChooseFeaturePoint(dSame, g_vTarKey, vTmpKey, vMatche, vGood, vTar, vTmp);
    }

    void MatchPoint(
        cv::Mat& cTar,
        cv::Mat& cTmp,
        std::vector<cv::KeyPoint>& vTar,
        std::vector<cv::KeyPoint>& vTmp,
        std::vector<std::vector<cv::DMatch>>& vMatche)
    {
        cv::Mat vTempDescriptor;

        g_lpSIFT->detectAndCompute(cTmp, cv::Mat(), vTmp, vTempDescriptor);

        vTempDescriptor.convertTo(vTempDescriptor, CV_32F);

        g_lpMatcher->knnMatch(g_cTarDescriptor, vTempDescriptor, vMatche, 2);
    }

    bool ChooseFeaturePoint(
        double &dSame,
        std::vector<cv::KeyPoint>& vTarKey,
        std::vector<cv::KeyPoint>& vTmpKey,
        std::vector<std::vector<cv::DMatch>>& vMatche,
        std::vector<cv::DMatch> &vGood,
        std::vector<cv::Point2f>& vTar,
        std::vector<cv::Point2f>& vTmp)
    {
        if (vMatche.empty()) return false;

        int nGood = 0;

        for (auto& iMatche : vMatche)
        {
            if (iMatche[0].distance < iMatche[1].distance * 0.75) nGood++;

            vGood.push_back(iMatche[0]);

            vTar.push_back(vTmpKey[iMatche[0].trainIdx].pt);

            vTmp.push_back(vTarKey[iMatche[0].queryIdx].pt);
        }

        dSame = (double)nGood / vMatche.size();

        return true;
    }
}