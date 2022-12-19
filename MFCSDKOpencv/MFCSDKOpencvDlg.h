
// MFCSDKOpencvDlg.h: 헤더 파일
//

#pragma once

#include "HCNetSDK.h"
#include "opencv2/opencv.hpp"
#include "plaympeg4.h"
#include "mmsystem.h"
#include "xfeatures2d.hpp"
#pragma comment(lib, "winmm.lib")

#define FRAME_WIDTH 1920
#define FRAME_HEIGHT 1080
#define CAMERA_NUM 4
#define OPENCV_ENABLE_NONFREE
#define PI 3.1415926
using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;


typedef struct slider_ctrl_information
{
	int cur_coarse;
	int cur_fine;
} SldInfo;

typedef struct box_information
{
	int left, right, top, bottom;
	Point pt;
} BoxInfo;

typedef struct previous_information // 이전 프레임에서 얻을 수 있는 정보 이것저것 여기 다 넣자
{
	int min_widthforDE[CAMERA_NUM];
	Mat BestMat[CAMERA_NUM - 1];
	int updatecnt[CAMERA_NUM - 1];
	int notupdatecnt[CAMERA_NUM - 1];
	bool enough[CAMERA_NUM - 1];
	int row_max, col_max;
} PrevInfo;

typedef struct common_data
{
	int mask_x[CAMERA_NUM - 1];
	Mat h[CAMERA_NUM - 1];

}CommonData;

// CMFCSDKOpencvDlg 대화 상자
class CMFCSDKOpencvDlg : public CDialogEx
{
	// 생성입니다.
public:
	CMFCSDKOpencvDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCSDKOPENCV_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

	double m_cx, m_cy, m_fx, m_fy, m_k1, m_k2, m_skew_c;
	int frame_number;
	uchar m_frame_num;
	uchar m_bbox_update_cnt;

	bool mb_bgrismade;
	bool mb_bgrismadeforDE[CAMERA_NUM];
	bool mb_detect;
	bool mb_detectforDE[CAMERA_NUM];
	bool mb_record;

	VideoWriter* videoWriter;

	CStatic m_picture;
	CStatic m_picture2;
	CStatic m_picture3;
	CStatic m_picture4;
	HDC m_PicCtrl_dc;
	HDC m_hdc;
	HDC m_h_mem_dc[4];
	HBITMAP m_h_mem_bmp[4];

	Mat mat_frame;
	Mat mat_mapping_table;
	Mat mat_buffer_frame, mat_buffer_frame2;
	Mat mat_buffer_frameforDE[CAMERA_NUM], mat_buffer_frame2forDE[CAMERA_NUM];
	Mat mat_cam[CAMERA_NUM];
	Mat mat_cam_gray[CAMERA_NUM];
	Mat mat_avgImg;
	Mat mat_avgImgforDE[CAMERA_NUM];
	Mat mat_background_frame, mat_background_frame_gray;
	Mat mat_background_frameforDE[CAMERA_NUM], mat_background_frame_grayforDE[CAMERA_NUM];;
	Mat mat_stats, mat_centroids;
	Mat mat_statsforDE[CAMERA_NUM], mat_centroidsforDE[CAMERA_NUM];
	Mat mat_wmcdst[CAMERA_NUM - 1];

	vector<Mat> vmat_imgs;
	vector<Mat> vecmat_bgr_frames;
	vector<Mat> vecmat_bgr_framesforDE[CAMERA_NUM];
	vector<Point> mvp_points;
	Stitcher::Mode m_mode;
	Ptr<Stitcher> m_stitcher;
	Stitcher::Status m_status;
	BoxInfo m_prev_box;
	PrevInfo m_previnfo;
	NET_DVR_USER_LOGIN_INFO m_struLoginInfo[CAMERA_NUM] = { 0 };
	CommonData m_commondata;

	UINT m_nTimerID;
	int m_cursel;
	SldInfo m_sldinfo[CAMERA_NUM];
	vector<Point2f> m_pts_src[CAMERA_NUM-1], m_pts_dst[CAMERA_NUM-1];



	void ImgProc();
	void PaintMFC();
	void Setparams();
	void UndistortImage();
	void Rotate();
	void CutandConcat();

	void SetBgrFrame();
	void SetBgrFrameforDE();
	void MakeBgr();

	void Detection();
	void DetectionforDE();
	void DetectionbyBlobMerging();
	void Track();
	void StitchbyOpenCV();

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();

	afx_msg void OnBnClickedButtonRecord();
	void EstimateH(Mat matLeftImage, Mat matRightImage, int Idx);
	Mat MakePanorama(Mat matLeftImage, Mat matRightImage, int Idx);

	CSliderCtrl m_sldTest;
	CEdit m_edTest;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEnChangeEditTest();
	CSliderCtrl m_sld_vertical;
	afx_msg void OnEnChangeEditVertical();
	CEdit m_edit_vertical;
	CComboBox m_cbTest;

	afx_msg void OnCbnSelchangeComboTest();

	void WarpbyMouseClick();
};

