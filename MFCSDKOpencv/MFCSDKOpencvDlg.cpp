
// MFCSDKOpencvDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "MFCSDKOpencv.h"
#include "MFCSDKOpencvDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

LONG nPort = -1;
extern CMFCSDKOpencvDlg* g_pdlg;

typedef HWND(WINAPI* PROCGETCONSOLEWINDOW)();
PROCGETCONSOLEWINDOW GetConsoleWindowAPI;

int g_count = 0;


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

void onMouse(int event, int x, int y, int flags, void* param);

void CALLBACK g_ExceptionCallBack(DWORD dwType, LONG lUserID, LONG lHandle, void* pUser)

{
	char tempbuf[256] = { 0 };
	switch (dwType)
	{
	case EXCEPTION_RECONNECT: //Reconnect during live view
		printf("------reconnect-----%d\n", (int)time(NULL));
		break;
	default:
		break;
	}
}

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCSDKOpencvDlg 대화 상자



CMFCSDKOpencvDlg::CMFCSDKOpencvDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCSDKOPENCV_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCSDKOpencvDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICTURE, m_picture);
	DDX_Control(pDX, IDC_PICTURE2, m_picture2);
	DDX_Control(pDX, IDC_PICTURE3, m_picture3);
	DDX_Control(pDX, IDC_PICTURE4, m_picture4);
	DDX_Control(pDX, IDC_SLIDER_TEST, m_sldTest);
	DDX_Control(pDX, IDC_EDIT_TEST, m_edTest);
	DDX_Control(pDX, IDC_SLIDER_VERTICAL, m_sld_vertical);
	DDX_Control(pDX, IDC_EDIT_VERTICAL, m_edit_vertical);
	DDX_Control(pDX, IDC_COMBO_TEST, m_cbTest);
}

BEGIN_MESSAGE_MAP(CMFCSDKOpencvDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCSDKOpencvDlg::OnBnClickedButtonRecord)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_EDIT_TEST, &CMFCSDKOpencvDlg::OnEnChangeEditTest)
	ON_EN_CHANGE(IDC_EDIT_VERTICAL, &CMFCSDKOpencvDlg::OnEnChangeEditVertical)
	ON_CBN_SELCHANGE(IDC_COMBO_TEST, &CMFCSDKOpencvDlg::OnCbnSelchangeComboTest)
END_MESSAGE_MAP()


// CMFCSDKOpencvDlg 메시지 처리기

void CALLBACK OnTimerCB(UINT m_nTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
	g_pdlg->OnTimer(1000);
	//printf("hi");
}


BOOL CMFCSDKOpencvDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	Setparams();

	

	NET_DVR_Init();
	NET_DVR_SetConnectTime(2000, 1);
	NET_DVR_SetReconnect(10000, true);
	NET_DVR_SetExceptionCallBack_V30(0, NULL, g_ExceptionCallBack, NULL);


	//Get the window handle
	HMODULE hKernel32 = GetModuleHandle(L"kernel32");
	GetConsoleWindowAPI = (PROCGETCONSOLEWINDOW)GetProcAddress(hKernel32, "GetConsoleWindow");

	//Log in to device
	LONG lUserID[CAMERA_NUM];

	//Login parameters, including device IP address, user name, password, and so on.
	NET_DVR_DEVICEINFO_V40 struDeviceInfoV40[CAMERA_NUM] = {0};
	LONG lRealPlayHandle[CAMERA_NUM];
	NET_DVR_PREVIEWINFO struPlayInfo[CAMERA_NUM] = {0};
	HWND h_mpic;
	for (int i = 0; i < 4; i++)
	{
		m_h_mem_dc[i] = CreateCompatibleDC(m_hdc);
		m_h_mem_bmp[i] = CreateCompatibleBitmap(m_hdc, FRAME_WIDTH, FRAME_HEIGHT);
		SelectObject(m_h_mem_dc[i], m_h_mem_bmp[i]);
	}

	for (int i = 0 ; i < 4; i++)
	{
		
		m_struLoginInfo[i].bUseAsynLogin = 0; // Synchronous login mode
		switch (i)
		{
		case 0:
			h_mpic = m_picture.GetSafeHwnd();
			struPlayInfo[i].hPlayWnd = h_mpic;
			strcpy(m_struLoginInfo[i].sDeviceAddress, "192.168.0.41"); // Device IP address
			break;
		case 1:
			h_mpic = m_picture2.GetSafeHwnd();
			struPlayInfo[i].hPlayWnd = h_mpic;
			strcpy(m_struLoginInfo[i].sDeviceAddress, "192.168.0.42"); // Device IP address
			break;
		case 2:
			h_mpic = m_picture3.GetSafeHwnd();
			struPlayInfo[i].hPlayWnd = h_mpic;
			strcpy(m_struLoginInfo[i].sDeviceAddress, "192.168.0.44"); // Device IP address
			break;
		case 3:
			h_mpic = m_picture4.GetSafeHwnd();
			struPlayInfo[i].hPlayWnd = h_mpic;
			strcpy(m_struLoginInfo[i].sDeviceAddress, "192.168.0.43"); // Device IP address
			break;
		default:
			break;
		}
		m_struLoginInfo[i].wPort = 8000; // Service port No.
		strcpy(m_struLoginInfo[i].sUserName, "admin"); // User name
		strcpy(m_struLoginInfo[i].sPassword, "neuro211124!"); //Password

		//Device information, output parameter
		lUserID[i] = NET_DVR_Login_V40(&m_struLoginInfo[i], &struDeviceInfoV40[i]);

		if (lUserID[i] < 0)
		{
			printf("Login failed, error code: %d\n", NET_DVR_GetLastError());
			NET_DVR_Cleanup();
			return 0;
		}

		// Start preview and set callback data stream
		//HWND hWnd = GetConsoleWindowAPI(); // Get window handle
		//struPlayInfo.hPlayWnd = hWnd; // Set the handle as valid for SDK decoding; set the handle as null for streaming only
		//struPlayInfo[i].hPlayWnd = NULL;
		struPlayInfo[i].lChannel = 1; //Live view channel No.
		struPlayInfo[i].dwStreamType = 0; //0-Main Stream, 1-Sub stream, 2-Stream3, 3-Stream4 and so on
		struPlayInfo[i].dwLinkMode = 0; // 0- TCP Mode, 1- UDP Mode, 2- Multicast Mode, 3- RTP Mode, 4- RTP/RTSP, 5- RTSP/HTTP
		struPlayInfo[i].bBlocked = 1; //0- Non-blocking Streaming, 1- Blocking Streaming
		struPlayInfo[i].dwDisplayBufNum = 1;
		//struPlayInfo.byVideoCodingType = 0;
		//struPlayInfo.bPassbackRecord = 1;

		//out.open("out.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 30, Size(1920, 1080), true);
		lRealPlayHandle[i] = NET_DVR_RealPlay_V40(lUserID[i], &struPlayInfo[i], NULL, NULL);

		if (lRealPlayHandle < 0)
		{
			printf("NET_DVR_REALPLAY-V40 error\n");
			NET_DVR_Logout(lUserID[i]);
			NET_DVR_Cleanup();
			return 0;
		}

	}

	//SetTimer(1000, 1000/30, NULL);

	TIMECAPS timecaps;
	timeGetDevCaps(&timecaps, sizeof(TIMECAPS));

	m_nTimerID = timeSetEvent(33, timecaps.wPeriodMin, OnTimerCB, 0, TIME_PERIODIC | TIME_CALLBACK_FUNCTION);



	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}


void CMFCSDKOpencvDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFCSDKOpencvDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMFCSDKOpencvDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCSDKOpencvDlg::OnDestroy()
{
	timeKillEvent(m_nTimerID);
	::ReleaseDC(m_picture.m_hWnd, m_hdc);


	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CMFCSDKOpencvDlg::PaintMFC()
{
	//화면에 보여주기 위한 처리입니다.
	int bpp = 8 * mat_frame.elemSize();
	assert((bpp == 8 || bpp == 24 || bpp == 32));

	int padding = 0;
	//32 bit image is always DWORD aligned because each pixel requires 4 bytes
	if (bpp < 32)
		padding = 4 - (mat_frame.cols % 4);

	if (padding == 4)
		padding = 0;

	int border = 0;
	//32 bit image is always DWORD aligned because each pixel requires 4 bytes
	if (bpp < 32)
	{
		border = 4 - (mat_frame.cols % 4);
	}

	/*if (border == 4)
		border = 0;*/



	Mat mat_temp;
	if (border > 0 || mat_frame.isContinuous() == false)
	{
		// Adding needed columns on the right (max 3 px)
		cv::copyMakeBorder(mat_frame, mat_temp, 0, 0, 0, border, cv::BORDER_CONSTANT, 0);
	}
	else
	{
		mat_temp = mat_frame.clone();
	}


	RECT r;
	m_picture.GetClientRect(&r);
	//cv::Size winSize(r.right, r.bottom);
	cv::Size winSize(1920, 1080); // 억지로 바꿔버리기



	BITMAPINFO* bitInfo = (BITMAPINFO*)malloc(sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD));
	bitInfo->bmiHeader.biBitCount = bpp;
	bitInfo->bmiHeader.biWidth = mat_temp.cols;
	bitInfo->bmiHeader.biHeight = -mat_temp.rows;
	bitInfo->bmiHeader.biPlanes = 1;
	bitInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitInfo->bmiHeader.biCompression = BI_RGB;
	bitInfo->bmiHeader.biClrImportant = 0;
	bitInfo->bmiHeader.biClrUsed = 0;
	bitInfo->bmiHeader.biSizeImage = 0;
	bitInfo->bmiHeader.biXPelsPerMeter = 0;
	bitInfo->bmiHeader.biYPelsPerMeter = 0;


	//그레이스케일 인경우 팔레트가 필요
	if (bpp == 8)
	{
		RGBQUAD* palette = bitInfo->bmiColors;
		for (int i = 0; i < 256; i++)
		{
			palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;
			palette[i].rgbReserved = 0;
		}
	}


	// Image is bigger or smaller than into destination rectangle
	// we use stretch in full rect
	m_PicCtrl_dc = ::GetDC(m_picture.m_hWnd);



	if (mat_temp.cols == winSize.width && mat_temp.rows == winSize.height)
	{
		// source and destination have same size
		// transfer memory block
		// NOTE: the padding border will be shown here. Anyway it will be max 3px width
		//SetStretchBltMode(hdc, COLORONCOLOR);
		SetDIBitsToDevice(m_PicCtrl_dc,
			//destination rectangle
			0, 0, winSize.width, winSize.height,
			0, 0, 0, mat_temp.rows,
			mat_temp.data, bitInfo, DIB_RGB_COLORS);
	}
	else
	{
		// destination rectangle
		int destx = 0, desty = 0;
		int destw = winSize.width;
		int desth = winSize.height;

		// rectangle defined on source bitmap
		// using imgWidth instead of mat_temp.cols will ignore the padding border
		int imgx = 0, imgy = 0;
		int imgWidth = mat_temp.cols - border;
		int imgHeight = mat_temp.rows;
		SetStretchBltMode(m_PicCtrl_dc, COLORONCOLOR);
		StretchDIBits(m_PicCtrl_dc,
			destx, desty, destw, desth,
			imgx, imgy, imgWidth, imgHeight,
			mat_temp.data, bitInfo, DIB_RGB_COLORS, SRCCOPY);
	}


	//HDC dc = ::GetDC(m_picture.m_hWnd);


	//::ReleaseDC(m_picture.m_hWnd, dc);
	//::ReleaseDC(m_picture.m_hWnd, PicCtrl_dc);

}


void CMFCSDKOpencvDlg::ImgProc()
{
	Rotate();
	UndistortImage();
	//WarpbyMouseClick(); // 클릭으로 warping point 찍어주기
	StitchbyOpenCV(); // auto warping
	//StitchingbyDE();
	//CutandConcat(); // 사전 정보를 바탕으로 고정적 수치로 clipping 후, 스크롤 바로 재조정 가능 , 여긴 seamless는 없네
	//Stitch();
	//===========================여기까지 전처리
	//SetBgrFrame();
	//Detection(); // 이거랑 바로 밑에 거 다시 보려니까 구분 잘 안 됨, 주석 좀 더 잘 써야할 듯
	//DetectionbyBlobMerging();
	//Track();

}

void CMFCSDKOpencvDlg::Setparams()
{

	mb_record = false;

	m_prev_box.top = 0;
	m_prev_box.left = 0;
	m_prev_box.right = 0;
	m_prev_box.bottom = 0;
	frame_number = 0;
	m_bbox_update_cnt = 0;
	mb_bgrismade = false;
	for (int i = 0; i < CAMERA_NUM; i++)
	{
		mb_bgrismadeforDE[i] = false;
		mb_detectforDE[i] = false;
		m_previnfo.min_widthforDE[i] = FRAME_WIDTH;

	}
	for (int i = 0; i < CAMERA_NUM - 1; i++)
	{
		m_previnfo.updatecnt[i] = 0;
		m_previnfo.notupdatecnt[i] = 0;
		m_previnfo.enough[i] = false;
	}
	m_previnfo.row_max = 0;
	m_previnfo.col_max = 0;

	mb_detect = false;
	mvp_points.clear();
	//CRect* pWnd[4];
//GetDlgItem(IDC_PICTURE)->GetWindowRect(pWnd[0]);
//ScreenToClient(pWnd[0]);
//GetDlgItem(IDC_PICTURE2)->GetWindowRect(pWnd[1]);
//ScreenToClient(pWnd[1]);
//GetDlgItem(IDC_PICTURE3)->GetWindowRect(pWnd[2]);
//ScreenToClient(pWnd[2]);
//GetDlgItem(IDC_PICTURE4)->GetWindowRect(pWnd[3]);
//ScreenToClient(pWnd[3]);
	m_frame_num = 0;

	RECT m_rect = { 0,0,1920,1080 };
	AdjustWindowRect(&m_rect, WS_OVERLAPPEDWINDOW, FALSE);
	int rect_width = m_rect.right - m_rect.left;
	int rect_height = m_rect.bottom - m_rect.top;
	this->SetWindowPos(NULL, 2400, 0, rect_width + 300, rect_height, 0);
	 
	//this->SetWindowPos(NULL, 2400, 0, 1920, 1080, 0);


	CWnd* videoWnd[4];
	//CWnd* videoWnd;

	//videoWnd = GetDlgItem(IDC_PICTURE);
	//videoWnd->MoveWindow(0, 0, 1920, 1080);
	videoWnd[0] = GetDlgItem(IDC_PICTURE);
	videoWnd[0]->MoveWindow(0, 0, 1920 / 2, 1080 / 2);
	//videoWnd[0]->MoveWindow(0, 0, 1920, 1080);

	videoWnd[1] = GetDlgItem(IDC_PICTURE2);
	videoWnd[1]->MoveWindow(1920 / 2, 0, 1920 / 2, 1080 / 2);

	videoWnd[2] = GetDlgItem(IDC_PICTURE3);
	videoWnd[2]->MoveWindow(0, 1080 / 2, 1920 / 2, 1080 / 2);

	videoWnd[3] = GetDlgItem(IDC_PICTURE4);
	videoWnd[3]->MoveWindow(1920 / 2, 1080 / 2, 1920 / 2, 1080 / 2);


	m_cx = 960.000/2;
	m_cy = 540.000/2;
	m_fx = 936.930480/2;
	m_fy = 929.599875/2;

	//m_k1 = -0.053149;
	m_k1 = -0.053149 * 2.5 ;

	//m_k2 = -0.058032;
	m_k2 = -0.058032 * 0.05;
	m_skew_c = 0;

	int height = FRAME_HEIGHT/2;
	int width = FRAME_WIDTH/2;
	mat_mapping_table.create(height, width, CV_32SC2);

	for (int map_y = 0; map_y < height; map_y++)
	{
		for (int map_x = 0; map_x < width; map_x++)
		{
			int px = map_x;
			int py = map_y;
			double d_x = px;
			double d_y = py;

			//normalize
			d_x = (d_x - m_cx) / m_fx;
			d_y = (d_y - m_cy) / m_fy;

			//distort
			double r2 = d_x * d_x + d_y * d_y;
			//double radial_d = 1 + k1 * r2 + k2 * r2 * r2;
			double radial_d = 1 + r2 * (m_k1 + r2 * m_k2);


			d_x = radial_d * d_x;
			d_y = radial_d * d_y;

			//denormalize
			d_x = m_fx * d_x + m_cx;
			d_y = m_fy * d_y + m_cy;

			//double to integer
			px = (int)(d_x + 0.5);
			py = (int)(d_y + 0.5);

			if (px >= 0 && py >= 0 && px < width && py < height)

			{
				mat_mapping_table.at<Vec2i>(map_y, map_x)[0] = py;
				mat_mapping_table.at<Vec2i>(map_y, map_x)[1] = px;


			}
			else
			{
				mat_mapping_table.at<Vec2i>(map_y, map_x)[0] = -1;
				mat_mapping_table.at<Vec2i>(map_y, map_x)[1] = -1;

			}
		}
	}

	//initialize stitcher
	m_mode = Stitcher::PANORAMA;
	m_stitcher->create(m_mode);
	vmat_imgs.clear();

	m_hdc = ::GetDC(m_picture.m_hWnd);

	m_sldTest.SetRange(0, 100);
	m_sldTest.SetPos(0);
	m_sldTest.SetLineSize(1);

	m_sld_vertical.SetRange(0, 20);
	m_sld_vertical.SetPos(0);
	m_sld_vertical.SetLineSize(1);


	int iPos = m_sldTest.GetPos();
	CString sPos;
	sPos.Format(_T("%d"), iPos);
	m_edTest.SetWindowTextW(sPos);

	// 상하좌우 각각 조동, 미동 나눠서 총 8종류 스크롤 바 선택 가능하게, up down left right coarse fine ex) 상_조동 -> up_coarse; 우 미동 -> right_fine
	m_cbTest.InsertString(0, _T("up"));
	m_cbTest.InsertString(1, _T("down"));
	m_cbTest.InsertString(2, _T("left"));
	m_cbTest.InsertString(3, _T("right"));
	m_cbTest.SetCurSel(0);
	m_cursel = 0;

	for (int i = 0; i < CAMERA_NUM; i++)
	{
		m_sldinfo[i].cur_coarse = 10;
		m_sldinfo[i].cur_fine = 10;
	}

	// 기존엔 내가 자를 수치 스크롤바로 넣어서 만드는 거였네, 여기다가 초기값을 사전 정보를 이용해서 넣어주는 걸로 만들어두면 될 듯
// 높이 320(cm), 카메라 간격_가로(share width length) = 190(cm), fov = 81deg(hor); 43deg(ver). undistort로 인한 손실: 960 -> 892 = -68, -> 412/480이 됨
// height는 540 -> 537 (거의 변화 없음) 267/270? 
// 그럼 먼저 left와 right 초기값을 결정해보자
	double cell_height = 305; // 320cm
	double fov_hor = 81; // in degree
	double fov_ver = 43;
	double cam_dist_hor = 180; //distance between cameras horizontally
	double cam_dist_ver = 90; // undefined 이것부터 석고보드 30cm * 3

	fov_hor = fov_hor * PI / 180; // degree to radian
	fov_ver = fov_ver * PI / 180; // degree to radian

	double temp_h = cell_height * tan(fov_hor / 2); // distance between center point and rightend point in real world by using tan function
	double temp_v = cell_height * tan(fov_ver / 2);
	

	int x, y;
	x = mat_mapping_table.at<Vec2i>(FRAME_HEIGHT / 4 - 1, FRAME_WIDTH / 2 - 1)[1] + 1; // x = 892
	x = FRAME_WIDTH / 2 - x; // x = 68
	x = FRAME_WIDTH / 4 - x; // x = 412

	y = mat_mapping_table.at<Vec2i>(FRAME_HEIGHT / 2 - 1, FRAME_WIDTH / 4 - 1)[0] + 1; // y = 528
	y = FRAME_HEIGHT / 2 - y; // y = 12
	y = FRAME_HEIGHT / 4 - y; // y = 258


	double r_hor = (double)x / (FRAME_WIDTH / 4); // r = 412 / 480; clipping ratio while using undistortion mapping table
	double r_ver = (double)y / (FRAME_HEIGHT / 4); // r = 258 / 270

	temp_h *= r_hor; // clipped distance in real world ; = 234 그럼 원점에서 (234-cam_dist_hor)만큼 더 튀어나온 거고(cam_dist_hor가 234일 때 원점에서 0만큼 튀어나온 거니까)이걸 다시 픽셀 비율로 환산해서 
	// 공유 영역의 길이는 234 * 2 - 190, 이것의 픽셀 환산 비는 960 * (234*2 - 190/234 * 2) 이게 공유 영역의 픽셀 길이니까 이것 절반 길이만큼 left right에서 clip해서 합치게 하면 될 듯
	temp_v *= r_ver; // = 120.449

	//cout << temp_v << endl;

	temp_h = (FRAME_WIDTH / 2) * ((temp_h * 2 - cam_dist_hor) / (temp_h * 2));
	temp_h /= 2;

	temp_v = (FRAME_HEIGHT / 2) * ((temp_v * 2 - cam_dist_ver) / (temp_v * 2));
	temp_v /= 2;


	double unit_h = FRAME_WIDTH / 2;
	double unit_v = FRAME_HEIGHT / 2;
	unit_h /= 100;
	unit_v /= 100;


	m_sldinfo[0].cur_coarse = (int)(temp_v / unit_v + 0.5);
	m_sldinfo[1].cur_coarse = (int)(temp_v / unit_v + 0.5);
	m_sldinfo[2].cur_coarse = (int)(temp_h / unit_h + 0.5);
	m_sldinfo[3].cur_coarse = (int)(temp_h / unit_h + 0.5);

	//cout << (double)m_sldinfo[2].cur_coarse/100 << endl;
	m_sldinfo[0].cur_fine = temp_v - (FRAME_HEIGHT / 2) * ((double)m_sldinfo[0].cur_coarse / 100);
	m_sldinfo[1].cur_fine = temp_v - (FRAME_HEIGHT / 2) * ((double)m_sldinfo[1].cur_coarse / 100);
	m_sldinfo[2].cur_fine = temp_h - (FRAME_WIDTH / 2) * ((double)m_sldinfo[2].cur_coarse / 100);
	m_sldinfo[3].cur_fine = temp_h - (FRAME_WIDTH / 2) * ((double)m_sldinfo[3].cur_coarse / 100);



}


void CMFCSDKOpencvDlg::UndistortImage()
{


	for (int i = 0; i < 4; i++)
	{
		int w = mat_cam[i].cols;
		int h = mat_cam[i].rows;
		int c = mat_cam[i].channels();


		if (c > 1)
		{
			mat_buffer_frame.create(FRAME_HEIGHT / 2, FRAME_WIDTH / 2, CV_8UC3);
			mat_buffer_frame.setTo(Scalar(0, 0, 0));
			for (int y = 0; y < h; y++)
			{
				for (int x = 0; x < w; x++)
				{
					int px = x;
					int py = y;

					py = mat_mapping_table.at<Vec2i>(y, x)[0];
					px = mat_mapping_table.at<Vec2i>(y, x)[1];

					if (px == -1 || py == -1)
					{

						mat_buffer_frame.at<Vec3b>(y, x)[0] = 0;
						mat_buffer_frame.at<Vec3b>(y, x)[1] = 0;
						mat_buffer_frame.at<Vec3b>(y, x)[2] = 0;
					}
					else
					{
						mat_buffer_frame.at<Vec3b>(y, x)[0] = mat_cam[i].at<Vec3b>(py, px)[0];
						mat_buffer_frame.at<Vec3b>(y, x)[1] = mat_cam[i].at<Vec3b>(py, px)[1];
						mat_buffer_frame.at<Vec3b>(y, x)[2] = mat_cam[i].at<Vec3b>(py, px)[2];
					}


				}
			}

		}
		else
		{
			mat_buffer_frame.create(FRAME_HEIGHT / 2, FRAME_WIDTH / 2, CV_8UC1);

			for (int y = 0; y < h; y++)
			{
				for (int x = 0; x < w; x++)
				{
					int px = x;
					int py = y;
					double d_x = px;
					double d_y = py;

					//normalize
					d_x = (d_x - m_cx) / m_fx;
					d_y = (d_y - m_cy) / m_fy;

					//distort
					double r2 = d_x * d_x + d_y * d_y;
					//double radial_d = 1 + k1 * r2 + k2 * r2 * r2;
					double radial_d = 1 + r2 * (m_k1 + r2 * m_k2);


					d_x = radial_d * d_x;
					d_y = radial_d * d_y;

					//denormalize
					d_x = m_fx * d_x + m_cx;
					d_y = m_fy * d_y + m_cy;

					//double to integer
					px = (int)(d_x + 0.5);
					py = (int)(d_y + 0.5);

					if (px >= 0 && py >= 0 && px < w && py < h)
					{
						mat_buffer_frame.at<unsigned char>(y, x) = mat_cam[i].at<unsigned char>(py, px);

					}
					else
					{
						mat_buffer_frame.at<unsigned char>(y, x) = 0;


					}


				}
			}

		}

		mat_cam[i] = mat_buffer_frame.clone();

	}
}

void CMFCSDKOpencvDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//::InvalidateRect(m_picture.m_hWnd, NULL, TRUE);
	//::UpdateWindow(m_picture.m_hWnd);
	HBITMAP h_bitmap = (HBITMAP)::GetCurrentObject(m_hdc, OBJ_BITMAP);
	//HBITMAP h_bitmap = (HBITMAP)::GetCurrentObject(m_h_mem_dc[0], OBJ_BITMAP);

	BITMAP bmp_info;
	::GetObject(h_bitmap, sizeof(BITMAP), &bmp_info);
	//HBITMAP h_bitmap11 = m_picture.GetBitmap();

	int width = bmp_info.bmWidth;
	int height = bmp_info.bmHeight;
	//int width = 1920;
	//int height = 1080;


	int nChannels = bmp_info.bmBitsPixel == 1 ? 1 : bmp_info.bmBitsPixel / 8;
	int depth = bmp_info.bmBitsPixel == 1 ? 1 : 8;

	BYTE* pBuffer = new BYTE[bmp_info.bmHeight * bmp_info.bmWidth * nChannels];
	GetBitmapBits(h_bitmap, height * width * nChannels, pBuffer);

	Mat Channel4Mat(height, width, CV_8UC4);
	memcpy(Channel4Mat.data, pBuffer, height * width * nChannels);
	Mat Channel3Mat(height, width , CV_8UC3);

	cvtColor(Channel4Mat, Channel3Mat, COLOR_BGRA2BGR);
	delete[] pBuffer;
	Mat cropped;
	//cropped = Channel3Mat.clone();
	cropped = Channel3Mat(Range(38, 1118), Range(9, 1929));
	//imshow("cropped", cropped);

	//imshow("test", Channel3Mat);
	//imshow("cropped", cropped);


	mat_cam[0] = cropped(Range(0, FRAME_HEIGHT / 2), Range(0, FRAME_WIDTH / 2));
	mat_cam[1] = cropped(Range(0, FRAME_HEIGHT / 2), Range(FRAME_WIDTH / 2, FRAME_WIDTH));
	mat_cam[2] = cropped(Range(FRAME_HEIGHT / 2, FRAME_HEIGHT), Range(0, FRAME_WIDTH / 2));
	mat_cam[3] = cropped(Range(FRAME_HEIGHT / 2, FRAME_HEIGHT), Range(FRAME_WIDTH / 2, FRAME_WIDTH));



	//bool b = false;
	//b = mat_cam[0].empty();
	//b = mat_cam[1].empty();
	//b = mat_cam[2].empty();
	//b = mat_cam[3].empty();
	//imshow("Channel4Mat", Channel4Mat);

	//putText()
	//putText(cropped, "cnt :" + to_string(g_count), Point(20, 100), FONT_HERSHEY_SIMPLEX, 2, Scalar(0, 255, 255), 3);
	//g_count++;

	//imshow("cropped", cropped);
	//waitKey(1);

	//imshow("cam1", mat_cam[0]);
	//imshow("cam2", mat_cam[1]);
	//imshow("cam3", mat_cam[2]);
	//imshow("cam4", mat_cam[3]);



	if (m_frame_num < 5)
	{
		m_frame_num++;
	}
	else
	{
		ImgProc();
		waitKey(1);
	}
	if (m_previnfo.enough[0] && m_previnfo.enough[1] && m_previnfo.enough[2] && !mat_frame.empty())
	{
		imshow("mat_frame", mat_frame);

		if (mb_record)
		{
			( *videoWriter ) << mat_frame;
		}
	}
	


	//cvtColor(mat_cam[0], mat_cam[0], COLOR_BGR2GRAY);
	//imshow("cam1", mat_cam[0]);
	//imshow("cam2", mat_cam[1]);
	//imshow("cam3", mat_cam[2]);
	//imshow("cam4", mat_cam[3]);
	

	

	//Rectangle(h_dc, 50, 50, 220, 150); //이걸로 h_dc에 그리는 건 할 수 있게 됨


	CDialogEx::OnTimer(nIDEvent);
}



void CMFCSDKOpencvDlg:: Rotate()
{
	int width = mat_cam[2].cols;
	int height = mat_cam[2].rows;
	Mat M;
	M = getRotationMatrix2D(Point(width / 2, height / 2), 1, 1);
	warpAffine(mat_cam[2], mat_cam[2], M, Size(width, height));
	warpAffine(mat_cam[3], mat_cam[3], M, Size(width, height));
	//warpAffine(mat_cam[0], mat_cam[0], M, Size(width, height));
	//M = getRotationMatrix2D(Point(width / 2, height / 2), -1, 1);
	//warpAffine(mat_cam[1], mat_cam[1], M, Size(width, height));

}

void CMFCSDKOpencvDlg::CutandConcat()
{
	int margin[CAMERA_NUM];
	double ratio[CAMERA_NUM];

	int up, down, left, right;

	for (int i = 0; i < CAMERA_NUM; i++)
	{
		ratio[i] = (double)m_sldinfo[i].cur_coarse / 100;
	}
	margin[0] = mat_cam[0].rows * ratio[0];
	margin[1] = mat_cam[0].rows * ratio[1];
	margin[2] = mat_cam[0].cols * ratio[2];
	margin[3] = mat_cam[0].cols * ratio[3];

	up = margin[0] + m_sldinfo[0].cur_fine;
	down = margin[1] + m_sldinfo[1].cur_fine;
	left = margin[2] + m_sldinfo[2].cur_fine;
	right = margin[3] + m_sldinfo[3].cur_fine;
	
	mat_cam[0] = mat_cam[0](Range(0, 540 - up), Range(0, 960 - left));
	mat_cam[1] = mat_cam[1](Range(0, 540 - up), Range(right, 960));
	mat_cam[2] = mat_cam[2](Range(down, 540), Range(0, 960 - left));
	mat_cam[3] = mat_cam[3](Range(down, 540), Range(right, 960));

	hconcat(mat_cam[0], mat_cam[1], mat_cam[0]);
	hconcat(mat_cam[2], mat_cam[3], mat_cam[2]);
	vconcat(mat_cam[0], mat_cam[2], mat_frame);

	//Mat avg_kernel = Mat::ones(Size(5, 5), CV_32F) / 25;
	//filter2D(mat_frame, mat_frame, -1, avg_kernel, Point(-1, -1), (0, 0), 4);
	imshow("mat_frame", mat_frame);

}

void CMFCSDKOpencvDlg::SetBgrFrame()
{
	if (mat_frame.empty())
	{
		return;
	}
	if (!mb_bgrismade)
	{
		if (vecmat_bgr_frames.size() < 32)
		{

			vecmat_bgr_frames.push_back(mat_frame);
			if (vecmat_bgr_frames.size() == 1)
			{
				mat_avgImg.create(vecmat_bgr_frames[0].rows, vecmat_bgr_frames[0].cols, CV_8UC3);
				mat_avgImg.setTo(Scalar(0, 0, 0));
			}
			MakeBgr();

			if (vecmat_bgr_frames.size() == 32)
			{
				mat_background_frame = mat_avgImg.clone();
				cvtColor(mat_background_frame, mat_background_frame_gray, COLOR_BGR2GRAY);
				mb_bgrismade = true;
				vecmat_bgr_frames.clear();
			}
		}
	}
	else
	{
			//imshow("mat_background_frame", mat_background_frame);
		if (!mb_detect)
		{
			if (vecmat_bgr_frames.size() < 32)
			{

				vecmat_bgr_frames.push_back(mat_frame);
				if (vecmat_bgr_frames.size() == 1)
				{
					mat_avgImg.setTo(Scalar(0, 0, 0));
				}
				MakeBgr();

				if (vecmat_bgr_frames.size() == 32)
				{
					mat_background_frame = mat_avgImg.clone();
					cvtColor(mat_background_frame, mat_background_frame_gray, COLOR_BGR2GRAY);
					vecmat_bgr_frames.clear();
				}
			}

		}
	}
	return;
}

void CMFCSDKOpencvDlg::SetBgrFrameforDE()
{
	for (int i = 0; i < CAMERA_NUM; i++)
	{
		if (!mb_bgrismadeforDE[i])
		{
			if (vecmat_bgr_framesforDE[i].size() < 32)
			{
				vecmat_bgr_framesforDE[i].push_back(mat_cam[i]);
				if (vecmat_bgr_framesforDE[i].size() == 1)
				{
					mat_avgImgforDE[i].create(vecmat_bgr_framesforDE[i][0].rows, vecmat_bgr_framesforDE[i][0].cols, CV_8UC3);
					mat_avgImgforDE[i].setTo(Scalar(0, 0, 0));
				}
				
				//  MakeBgrforDE(); 의 내용을 아래에 씀 내부 변수 i를 공유할 수 없다

				int frame_row = vecmat_bgr_framesforDE[i][0].rows;
				int frame_col = vecmat_bgr_framesforDE[i][0].cols;

				for (int row = 0; row < frame_row; row++)
				{
					for (int col = 0; col < frame_col; col++)
					{
						mat_avgImgforDE[i].at<Vec3b>(row, col)[0] += (vecmat_bgr_framesforDE[i][vecmat_bgr_framesforDE[i].size() - 1].at<Vec3b>(row, col)[0] + 16) >> 5;
						mat_avgImgforDE[i].at<Vec3b>(row, col)[1] += (vecmat_bgr_framesforDE[i][vecmat_bgr_framesforDE[i].size() - 1].at<Vec3b>(row, col)[1] + 16) >> 5;
						mat_avgImgforDE[i].at<Vec3b>(row, col)[2] += (vecmat_bgr_framesforDE[i][vecmat_bgr_framesforDE[i].size() - 1].at<Vec3b>(row, col)[2] + 16) >> 5;
					}
				}

				if (vecmat_bgr_framesforDE[i].size() == 32)
				{
					mat_background_frameforDE[i] = mat_avgImgforDE[i].clone();
					cvtColor(mat_background_frameforDE[i], mat_background_frame_grayforDE[i], COLOR_BGR2GRAY);
					mb_bgrismadeforDE[i] = true;
					vecmat_bgr_framesforDE[i].clear();
				}
			}
		}

		else
		{
			if (!mb_detectforDE[i])
			{
				if (vecmat_bgr_framesforDE[i].size() < 32)
				{
					vecmat_bgr_framesforDE[i].push_back(mat_cam[i]);
					if (vecmat_bgr_framesforDE[i].size() == 1)
					{
						mat_avgImgforDE[i].setTo(Scalar(0, 0, 0));
					}

					//  MakeBgrforDE(); 의 내용을 아래에 씀 내부 변수 i를 공유할 수 없다

					int frame_row = vecmat_bgr_framesforDE[i][0].rows;
					int frame_col = vecmat_bgr_framesforDE[i][0].cols;

					for (int row = 0; row < frame_row; row++)
					{
						for (int col = 0; col < frame_col; col++)
						{
							mat_avgImgforDE[i].at<Vec3b>(row, col)[0] += (vecmat_bgr_framesforDE[i][vecmat_bgr_framesforDE[i].size() - 1].at<Vec3b>(row, col)[0] + 16) >> 5;
							mat_avgImgforDE[i].at<Vec3b>(row, col)[1] += (vecmat_bgr_framesforDE[i][vecmat_bgr_framesforDE[i].size() - 1].at<Vec3b>(row, col)[1] + 16) >> 5;
							mat_avgImgforDE[i].at<Vec3b>(row, col)[2] += (vecmat_bgr_framesforDE[i][vecmat_bgr_framesforDE[i].size() - 1].at<Vec3b>(row, col)[2] + 16) >> 5;
						}
					}

					if (vecmat_bgr_framesforDE[i].size() == 32)
					{
						mat_background_frameforDE[i] = mat_avgImgforDE[i].clone();
						cvtColor(mat_background_frameforDE[i], mat_background_frame_grayforDE[i], COLOR_BGR2GRAY);
						vecmat_bgr_framesforDE[i].clear();
					}
				}
			}
		}
	}
}

void CMFCSDKOpencvDlg::MakeBgr()
{
	int frame_row = vecmat_bgr_frames[0].rows;
	int frame_col = vecmat_bgr_frames[0].cols;
	for (int row = 0; row < frame_row; row++)
	{
		for (int col = 0; col < frame_col; col++)
		{
			mat_avgImg.at<Vec3b>(row, col)[0] += (vecmat_bgr_frames[vecmat_bgr_frames.size() - 1].at<Vec3b>(row, col)[0] + 16) >> 5;
			mat_avgImg.at<Vec3b>(row, col)[1] += (vecmat_bgr_frames[vecmat_bgr_frames.size() - 1].at<Vec3b>(row, col)[1] + 16) >> 5;
			mat_avgImg.at<Vec3b>(row, col)[2] += (vecmat_bgr_frames[vecmat_bgr_frames.size() - 1].at<Vec3b>(row, col)[2] + 16) >> 5;



		}

	}

}


void CMFCSDKOpencvDlg::Detection()
{
	if (!mb_bgrismade)
	{
		return;
	}
	cvtColor(mat_frame, mat_buffer_frame, COLOR_BGR2GRAY);
	mat_buffer_frame2 = mat_buffer_frame.clone();
	Canny(mat_buffer_frame2, mat_buffer_frame2, 1, 200, 3, false);
	dilate(mat_buffer_frame2, mat_buffer_frame2, Mat(), Point(-1, -1), 1);

	Mat canny_frame = mat_buffer_frame2.clone();
	//imshow("gray version of original image", mat_buffer_frame);

	absdiff(mat_buffer_frame, mat_background_frame_gray, mat_buffer_frame);
	//imshow("output of absdiff", mat_buffer_frame);

	threshold(mat_buffer_frame, mat_buffer_frame, 30, 255, THRESH_BINARY);

	erode(mat_buffer_frame, mat_buffer_frame, Mat(), Point(-1, -1), 3);
	dilate(mat_buffer_frame, mat_buffer_frame, Mat(), Point(-1, -1), 1);
	

	//imshow("output of thresholding", mat_buffer_frame);
	int NumofLabels = connectedComponentsWithStats(mat_buffer_frame, mat_buffer_frame2, mat_stats, mat_centroids, 8, CV_32S);

	int area = 0;
	int top = 0;
	int left = 0;
	int width = 0;
	int height = 0;
	int numofBigBlobs = 0;
	//mui_MaxArea = 0, mui_MaxLabelIdx = 0;

	int max_left = mat_buffer_frame.cols;
	int max_right = 0;
	int max_top = mat_buffer_frame.rows;
	int max_bottom = 0;

	for (int labelIdx = 1; labelIdx < NumofLabels; labelIdx++)
	{
		area = mat_stats.at<int>(labelIdx, CC_STAT_AREA);
		if (area < 3000)
		{

		}
		else
		{
			left = mat_stats.at<int>(labelIdx, CC_STAT_LEFT);
			top = mat_stats.at<int>(labelIdx, CC_STAT_TOP);
			width = mat_stats.at<int>(labelIdx, CC_STAT_WIDTH);
			height = mat_stats.at<int>(labelIdx, CC_STAT_HEIGHT);
			numofBigBlobs++;
			max_left = left < max_left ? left : max_left;
			max_right = (left + width) > max_right ? (left + width) : max_right;
			max_top = top < max_top ? top : max_top;
			max_bottom = (top + height) > max_bottom ? (top + height) : max_bottom;


			//rectangle(mat_frame, Point(left, top), Point(left + width, top + height), Scalar(0, 255, 255), 3);
			//rectangle(mat_buffer_frame, Point(left, top), Point(left + width, top + height), Scalar(255, 0, 0), 3);
			//rectangle(mat_buffer_frame, Point(left + width / 2, top + height / 2), Point(left + width / 2 + 1, top + height / 2 + 1), Scalar(255, 0, 255), 3);

			//putText(mat_frame, "area :" + to_string(area), Point(left, top -20), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 255), 3);
			//putText(mat_buffer_frame, "area :" + to_string(area), Point(left, top -20), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 3);
			//putText(mat_frame, "labelIdx :" + to_string(labelIdx), Point(left, top + 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 3);
			//putText(mat_buffer_frame, "labelIdx :" + to_string(labelIdx), Point(left, top + 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 3);


		}

	}
	if (numofBigBlobs)
	{
		mb_detect = true;
		//putText(mat_frame, "# of Bigblobs :" + to_string(numofBigBlobs), Point(20, 980), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 3);
		putText(mat_buffer_frame, "# of Bigblobs :" + to_string(numofBigBlobs), Point(20, 980), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 3);
		//rectangle(mat_buffer_frame, Point(max_left, max_top), Point(max_right, max_bottom), Scalar(255, 0, 0), 10);
		Mat temp(canny_frame.size(), CV_8U, Scalar(0));
		Mat temp2;
		Mat roi;
		roi = temp(Rect(max_left, max_top, max_right - max_left, max_bottom - max_top));
		roi = Scalar(255);
		bitwise_or(mat_buffer_frame, canny_frame, temp2, temp);
		// ("mat_buffer_frame2", mat_buffer_frame2);
		//imshow("canny_frame", canny_frame);
		//imshow("temp", temp);
		imshow("temp2", temp2);
		numofBigBlobs = 0;

		NumofLabels = connectedComponentsWithStats(temp2, mat_buffer_frame2, mat_stats, mat_centroids, 8, CV_32S);
		int maxIdx = 0;
		int maxArea = 0;
		for (int labelIdx = 1; labelIdx < NumofLabels; labelIdx++)
		{
			area = mat_stats.at<int>(labelIdx, CC_STAT_AREA);

			if (area < 4500)
			{

			}
			else
			{
				numofBigBlobs++;
				if (area > maxArea)
				{
					maxArea = area;
					maxIdx = labelIdx;
				}

			}
		}
		if (numofBigBlobs)
		{
			left = mat_stats.at<int>(maxIdx, CC_STAT_LEFT);
			top = mat_stats.at<int>(maxIdx, CC_STAT_TOP);
			width = mat_stats.at<int>(maxIdx, CC_STAT_WIDTH);
			height = mat_stats.at<int>(maxIdx, CC_STAT_HEIGHT);
			if (!m_prev_box.left && !m_prev_box.right && !m_prev_box.top && !m_prev_box.bottom)
			{
				m_prev_box.left = left;
				m_prev_box.right = left + width;
				m_prev_box.top = top;
				m_prev_box.bottom = top + height;
				m_prev_box.pt.x = (int)(mat_centroids.at<double>(maxIdx, 0));
				m_prev_box.pt.y = (int)(mat_centroids.at<double>(maxIdx, 1));
				//line(mat_frame, prev_box.pt, prev_box.pt, Scalar(0, 255, 255), 10);
				mvp_points.push_back(m_prev_box.pt);
				rectangle(mat_buffer_frame, Point(left, top), Point(left + width, top + height), Scalar(255, 0, 0), 3);
				putText(mat_buffer_frame, "area :" + to_string(maxArea), Point(left, top - 20), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 3);

			}
			else
			{
				int right = left + width;
				int bottom = top + height;
				int threshold = 60;

				int leftdiff = m_prev_box.left - left;
				int rightdiff = m_prev_box.right - right;
				int topdiff = m_prev_box.top - top;
				int bottomdiff = m_prev_box.bottom - bottom;

				if (leftdiff > -1 * threshold && leftdiff < threshold // 비슷한 bbox 있을 시 갱신
					&& rightdiff > -1 * threshold && rightdiff < threshold
					&& topdiff > -1 * threshold && topdiff < threshold
					&& bottomdiff > -1 * threshold && bottomdiff < threshold)
				//if (false)
				{
					rectangle(mat_buffer_frame, Point(left, top), Point(left + width, top + height), Scalar(255, 0, 0), 3);
					putText(mat_buffer_frame, "area :" + to_string(maxArea), Point(left, top - 20), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 3);
					m_prev_box.left = left;
					m_prev_box.right = left + width;
					m_prev_box.top = top;
					m_prev_box.bottom = top + height;

					m_prev_box.pt.x = (int)(mat_centroids.at<double>(maxIdx, 0));
					m_prev_box.pt.y = (int)(mat_centroids.at<double>(maxIdx, 1));
					//line(mat_frame, prev_box.pt, prev_box.pt, Scalar(0, 255, 255), 10);
					mvp_points.push_back(m_prev_box.pt);


				}
				else     // 없으면 이전 것 그대로 쓰기, 너무 오래 계속 이것만 쓰면 새로 갈아야함
				{
					
					if (m_bbox_update_cnt < 20)
					{
						rectangle(mat_buffer_frame, Point(m_prev_box.left, m_prev_box.top), Point(m_prev_box.right, m_prev_box.bottom), Scalar(255, 0, 0), 3);
						putText(mat_buffer_frame, "area :" + to_string(maxArea), Point(m_prev_box.left, m_prev_box.top - 20), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 3);
						
						//line(mat_frame, prev_box.pt, prev_box.pt, Scalar(0, 255, 255), 10);
						mvp_points.push_back(m_prev_box.pt);


						m_bbox_update_cnt++;
					}
					else
					{
						m_bbox_update_cnt = 0;
						rectangle(mat_buffer_frame, Point(left, top), Point(left + width, top + height), Scalar(255, 0, 0), 3);
						putText(mat_buffer_frame, "area :" + to_string(maxArea), Point(left, top - 20), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 3);
						m_prev_box.left = left;
						m_prev_box.right = left + width;
						m_prev_box.top = top;
						m_prev_box.bottom = top + height;

						m_prev_box.pt.x = (int)(mat_centroids.at<double>(maxIdx, 0));
						m_prev_box.pt.y = (int)(mat_centroids.at<double>(maxIdx, 1));
						//line(mat_frame, prev_box.pt, prev_box.pt, Scalar(0, 255, 255), 10);
						mvp_points.push_back(m_prev_box.pt);



					}


				}
			}
		}


	}
	else
	{
		mb_detect = false;
	}
	String str = mb_detect ? "true" : "false";
	//putText(mat_frame, "mb_detect :" + str, Point(20, mat_frame.rows - 80), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 255), 3);
	putText(mat_buffer_frame, "mb_detect :" + str, Point(20, mat_buffer_frame.rows - 80), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 3);
	//putText(mat_frame, "frame_number :" + to_string(frame_number), Point(20, mat_frame.rows - 180), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 255), 3);
	//frame_number++;
}

void CMFCSDKOpencvDlg::DetectionforDE()
{

	int NumofLabels[CAMERA_NUM] = { 0 };
	vector<int> bigblobIdx[CAMERA_NUM];

	if (!mb_bgrismadeforDE[0] && !mb_bgrismadeforDE[1] && !mb_bgrismadeforDE[2] && !mb_bgrismadeforDE[3])
	{
		return;
	}

	Point2d Avgxy[CAMERA_NUM];
	for (int i = 0; i < CAMERA_NUM; i++)
	{
		Avgxy[i].x = 0;
		Avgxy[i].y = 0;
	}

	for (int i = 0; i < CAMERA_NUM; i++)
	{
		cvtColor(mat_cam[i], mat_buffer_frameforDE[i], COLOR_BGR2GRAY);
		mat_cam_gray[i] = mat_buffer_frameforDE[i].clone();
		absdiff(mat_buffer_frameforDE[i], mat_background_frame_grayforDE[i], mat_buffer_frameforDE[i]);
		threshold(mat_buffer_frameforDE[i], mat_buffer_frameforDE[i], 35, 255, THRESH_BINARY);
		//dilate(mat_buffer_frameforDE[i], mat_buffer_frameforDE[i], Mat(), Point(-1, -1), 1);
		//erode(mat_buffer_frameforDE[i], mat_buffer_frameforDE[i], Mat(), Point(-1, -1), 3);


		NumofLabels[i] = connectedComponentsWithStats(mat_buffer_frameforDE[i], mat_buffer_frame2forDE[i], mat_statsforDE[i], mat_centroidsforDE[i], 8, CV_32S);
		int area = 0;
		int top = 0;
		int left = 0;
		int width = 0;
		int height = 0;
		int numofBigBlobs = 0;
		int area_sum = 0;
		int left_max = mat_buffer_frameforDE[i].cols;
		int right_max = 0;
		for (int labelIdx = 1; labelIdx < NumofLabels[i]; labelIdx++)
		{
			area = mat_statsforDE[i].at<int>(labelIdx, CC_STAT_AREA);
			if (area < 1500)
			{

			}
			else
			{
				numofBigBlobs++;
				bigblobIdx[i].push_back(labelIdx);

				left = mat_statsforDE[i].at<int>(labelIdx, CC_STAT_LEFT);
				left_max = left < left_max ? left : left_max;
				top = mat_statsforDE[i].at<int>(labelIdx, CC_STAT_TOP);
				width = mat_statsforDE[i].at<int>(labelIdx, CC_STAT_WIDTH);
				right_max = right_max > left + width ? right_max : left + width;
				height = mat_statsforDE[i].at<int>(labelIdx, CC_STAT_HEIGHT);

				double x, y;
				x = mat_centroidsforDE[i].at<double>(labelIdx, 0);
				y = mat_centroidsforDE[i].at<double>(labelIdx, 1);
				Avgxy[i].x += x * area;
				Avgxy[i].y += y * area;
				area_sum += area;

				if (area > 7000)
				{
					int x_center = mat_buffer_frameforDE[i].cols / 2;
					int diff_x = x_center - (int)x;
					int threshold = 75;
					if (diff_x >= threshold * (-1) && diff_x < threshold)
					{
						m_previnfo.min_widthforDE[i] = width < m_previnfo.min_widthforDE[i] ? width : m_previnfo.min_widthforDE[i];
					}

				}
				//rectangle(mat_buffer_frameforDE[i], Point(left, top), Point(left + width, top + height), Scalar(255, 0, 0), 3);
				//putText(mat_buffer_frameforDE[i], "area :" + to_string(area), Point(left, top - 20), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 3);



			}
		}
		//putText(mat_buffer_frameforDE[i], "min_w :" + to_string(m_previnfo.min_widthforDE[i]), Point(20, 500), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 3);

		Avgxy[i].x /= area_sum;
		Avgxy[i].x = (int)Avgxy[i].x;
		Avgxy[i].y /= area_sum;
		Avgxy[i].y = (int)Avgxy[i].y;

		if (numofBigBlobs)
		{
			mb_detectforDE[i] = true;
			line(mat_cam[i], Point(Avgxy[i].x, Avgxy[i].y), Point(Avgxy[i].x, Avgxy[i].y), Scalar(0, 255, 255), 10, 8, 0);
			line(mat_cam[i], Point(left_max, Avgxy[i].y), Point(left_max, Avgxy[i].y), Scalar(0, 0, 255), 10, 8, 0);
			line(mat_cam[i], Point(left_max+20, Avgxy[i].y), Point(left_max + 20, Avgxy[i].y), Scalar(0, 255, 0), 10, 8, 0);
			line(mat_cam[i], Point(right_max, Avgxy[i].y), Point(right_max, Avgxy[i].y), Scalar(0, 0, 255), 10, 8, 0);
			line(mat_cam[i], Point(right_max-20, Avgxy[i].y), Point(right_max - 20, Avgxy[i].y), Scalar(0, 255, 0), 10, 8, 0);


		}
		else
		{
			mb_detectforDE[i] = false;
		}
		switch (i)
		{
		case 0:
			//bitwise_not(mat_buffer_frameforDE[i], mat_buffer_frameforDE[i]);
			//mat_cam[i].copyTo(mat_buffer_frameforDE[i], mat_buffer_frameforDE[i]);
			//imshow("buffer0", mat_buffer_frameforDE[i]);
			break;
		case 1:
			//bitwise_not(mat_buffer_frameforDE[i], mat_buffer_frameforDE[i]);
			//mat_cam[i].copyTo(mat_buffer_frameforDE[i], mat_buffer_frameforDE[i]);
			//imshow("buffer1", mat_buffer_frameforDE[i]);
			break;
		case 2:
			//bitwise_not(mat_buffer_frameforDE[i], mat_buffer_frameforDE[i]);
			//mat_cam[i].copyTo(mat_buffer_frameforDE[i], mat_buffer_frameforDE[i]);
			//imshow("buffer2", mat_buffer_frameforDE[i]);
			break;
		case 3:
			//bitwise_not(mat_buffer_frameforDE[i], mat_buffer_frameforDE[i]);
			//mat_cam[i].copyTo(mat_buffer_frameforDE[i], mat_buffer_frameforDE[i]);
			//imshow("buffer3", mat_buffer_frameforDE[i]);
			break;
		default:
			break;
		}
		

	} // 전체 for문 끝
	int dist0 = 0, dist1 = 0;
	Mat clipped_h0, clipped_h1, clipped_h2, clipped_h3;
	int border_vertical = 570;
	int line_left, line_right;
	line_left = 380;
	line_left = 380 + (mat_buffer_frameforDE[0].cols - 380) / 2;
	line_right = 560;
	line_right = line_right / 2;

	if (mb_detectforDE[0] && mb_detectforDE[1]) // cam0과 cam1에 모두 탐지됐을 때 어떤 기준으로 나눠서 합칠 지 
	{
		dist0 = sqrt(pow(Avgxy[0].x - mat_cam[0].cols / 2, 2) + pow(Avgxy[0].y - mat_cam[0].rows / 2, 2));
		dist1 = sqrt(pow(Avgxy[1].x - mat_cam[1].cols / 2, 2) + pow(Avgxy[1].y - mat_cam[1].rows / 2, 2));
		int dist_gap = dist0 - dist1;
		int dist_threshold = 100;
		if (dist_gap > dist_threshold * (-1) && dist_gap < dist_threshold)
		{
			if (dist0 < dist1)
			{

				for (int i = 0; i < bigblobIdx[0].size(); i++)
				{
					int left = mat_statsforDE[0].at<int>(bigblobIdx[0][i], CC_STAT_LEFT);
					int top = mat_statsforDE[0].at<int>(bigblobIdx[0][i], CC_STAT_TOP);
					int width = mat_statsforDE[0].at<int>(bigblobIdx[0][i], CC_STAT_WIDTH);
					int height = mat_statsforDE[0].at<int>(bigblobIdx[0][i], CC_STAT_HEIGHT);
					int area = mat_statsforDE[0].at<int>(bigblobIdx[0][i], CC_STAT_AREA);
					rectangle(mat_cam[0], Point(left, top), Point(left + width, top + height), Scalar(0, 0, 255), 4);
					rectangle(mat_buffer_frameforDE[0], Point(left, top), Point(left + width, top + height), Scalar(255), 4);
					putText(mat_cam[0], "area : " + to_string(area), Point(left, top - 20), FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0, 255, 255), 3);

					// 이게 mat_cam[0]부분의 찾은 블록이고 아래에 mat_cam[1]에서의 카운터 파트 블록 좌표로 바꿔준다 -> left만 바꾸고 나머지는 그대로

					//mat_cam[1]의 left는 mat_cam[0]의 right 좌표의 좌우 대칭

					left = left + width; // right
					left = mat_buffer_frameforDE[0].cols - left; // 좌우 대칭
					rectangle(mat_cam[1], Point(left, top), Point(left + width, top + height), Scalar(0, 0, 255), 4);


					//이제 이 roi(left, top, width, height)에 0이 아닌 픽셀이 몇 개 있는 지를 세서 좌우가 비슷하면 둘은 비슷한 높이에 있다라는 판단을 할 것임
					// 그 전에 블럭 잘 뽑았나 눈으로 확인좀 해보자 < 확인 완
					// 이제 mat_buffer_frameforDE[1]에서 이 블록 함 쳐봄 이게 dist1이랑 dist2가 서로 비슷할 때 해야되는 거네, 차이가 클 때는 한쪽만 보여주면 됨, 비슷할 때에 대한 고민이어야 한다.

					rectangle(mat_buffer_frameforDE[1], Point(left, top), Point(left + width, top + height), Scalar(255), 4);
					Mat roi = mat_buffer_frameforDE[1](Range(top, top + height), Range(left, left + width));
					int nonzeropixel = 0;
					for (int row = 0; row < roi.rows; row++)
					{
						for (int col = 0; col < roi.cols; col++)
						{
							if (roi.at<uchar>(row, col))
							{
								nonzeropixel++;
							}
						}
					}
					putText(mat_cam[1], "c: " + to_string(nonzeropixel), Point(left, top - 20), FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0, 255, 255), 3);

				}
			}
			else // 바로 위 if문과 좌우 반대 처리 cam1 ->cam0
			{
				for (int i = 0; i < bigblobIdx[1].size(); i++)
				{
					int left = mat_statsforDE[1].at<int>(bigblobIdx[1][i], CC_STAT_LEFT);
					int top = mat_statsforDE[1].at<int>(bigblobIdx[1][i], CC_STAT_TOP);
					int width = mat_statsforDE[1].at<int>(bigblobIdx[1][i], CC_STAT_WIDTH);
					int height = mat_statsforDE[1].at<int>(bigblobIdx[1][i], CC_STAT_HEIGHT);
					int area = mat_statsforDE[1].at<int>(bigblobIdx[1][i], CC_STAT_AREA);
					rectangle(mat_cam[1], Point(left, top), Point(left + width, top + height), Scalar(0, 0, 255), 4);
					rectangle(mat_buffer_frameforDE[1], Point(left, top), Point(left + width, top + height), Scalar(255), 4);
					putText(mat_cam[1], "area : " + to_string(area), Point(left, top - 20), FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0, 255, 255), 3);


					left = left + width; // right
					left = mat_buffer_frameforDE[1].cols - left; // 좌우 대칭
					rectangle(mat_cam[0], Point(left, top), Point(left + width, top + height), Scalar(0, 0, 255), 4);



					rectangle(mat_buffer_frameforDE[0], Point(left, top), Point(left + width, top + height), Scalar(255), 4);
					Mat roi = mat_buffer_frameforDE[0](Range(top, top + height), Range(left, left + width));
					int nonzeropixel = 0;
					for (int row = 0; row < roi.rows; row++)
					{
						for (int col = 0; col < roi.cols; col++)
						{
							if (roi.at<uchar>(row, col))
							{
								nonzeropixel++;
							}
						}
					}
					putText(mat_cam[0], "c: " + to_string(nonzeropixel), Point(left, top - 20), FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0, 255, 255), 3);

				}

			}

		}
		else
		{


			if (dist0 < dist1)
			{
				clipped_h1 = mat_cam[1](Range(0, mat_cam[1].rows), Range(border_vertical, mat_cam[1].cols));
				hconcat(mat_cam[0], clipped_h1, clipped_h0);
			}
			else
			{
				clipped_h0 = mat_cam[0](Range(0, mat_cam[0].rows), Range(0, mat_cam[0].cols - border_vertical));
				hconcat(clipped_h0, mat_cam[1], clipped_h0);
			}
			imshow("merged image", clipped_h0);

		}

	}
	imshow("mbffDE[0]", mat_buffer_frameforDE[0]);
	imshow("mbffDE[1]", mat_buffer_frameforDE[1]);


	clipped_h0 = mat_buffer_frameforDE[0](Range(0, mat_buffer_frameforDE[0].rows), Range(0, line_left));
	clipped_h1 = mat_buffer_frameforDE[1](Range(0, mat_buffer_frameforDE[1].rows), Range(line_right, mat_buffer_frameforDE[1].cols));

	clipped_h2 = mat_buffer_frameforDE[2](Range(0, mat_buffer_frameforDE[2].rows), Range(0, line_left));
	clipped_h3 = mat_buffer_frameforDE[3](Range(0, mat_buffer_frameforDE[3].rows), Range(line_right, mat_buffer_frameforDE[3].cols));


	hconcat(clipped_h0, clipped_h1, clipped_h0);
	hconcat(clipped_h2, clipped_h3, clipped_h2);

	//imshow("clipped_h0", clipped_h0);
	//imshow("clipped_h2", clipped_h2);

	clipped_h0 = mat_buffer_frameforDE[0](Range(0, mat_buffer_frameforDE[0].rows), Range(line_left, mat_buffer_frameforDE[0].cols));
	clipped_h1 = mat_buffer_frameforDE[1](Range(0, mat_buffer_frameforDE[1].rows), Range(0, line_right));

	//imshow("res of h0", clipped_h0);
	//imshow("res of h1", clipped_h1);



}

void CMFCSDKOpencvDlg::DetectionbyBlobMerging()
{


	if (!mb_bgrismade)
	{
		return;
	}



	cvtColor(mat_frame, mat_buffer_frame, COLOR_BGR2GRAY);

	absdiff(mat_buffer_frame, mat_background_frame_gray, mat_buffer_frame);

	threshold(mat_buffer_frame, mat_buffer_frame, 30, 255, THRESH_BINARY);

	erode(mat_buffer_frame, mat_buffer_frame, Mat(), Point(-1, -1), 3);
	//dilate(mat_buffer_frame, mat_buffer_frame, Mat(), Point(-1, -1), 1);


	int NumofLabels = connectedComponentsWithStats(mat_buffer_frame, mat_buffer_frame2, mat_stats, mat_centroids, 8, CV_32S);

	int area = 0;
	int top = 0;
	int left = 0;
	int width = 0;
	int height = 0;
	int numofBigBlobs = 0;


	for (int labelIdx = 1; labelIdx < NumofLabels; labelIdx++)
	{
		area = mat_stats.at<int>(labelIdx, CC_STAT_AREA);
		if (area < 1000)
		{

		}
		else
		{
			left = mat_stats.at<int>(labelIdx, CC_STAT_LEFT);
			top = mat_stats.at<int>(labelIdx, CC_STAT_TOP);
			width = mat_stats.at<int>(labelIdx, CC_STAT_WIDTH);
			height = mat_stats.at<int>(labelIdx, CC_STAT_HEIGHT);
			numofBigBlobs++;

			rectangle(mat_buffer_frame, Point(left, top), Point(left + width, top + height), Scalar(255, 0, 0), 3);



		}
	}
	if (numofBigBlobs)
	{
		mb_detect = true;
		//putText(mat_frame, "# of Bigblobs :" + to_string(numofBigBlobs), Point(20, 980), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 3);
		putText(mat_buffer_frame, "# of Bigblobs :" + to_string(numofBigBlobs), Point(20, mat_buffer_frame.rows - 300), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 3);
		//rectangle(mat_buffer_frame, Point(max_left, max_top), Point(max_right, max_bottom), Scalar(255, 0, 0), 10);
	}
	else
	{
		mb_detect = false;
	}
	String str = mb_detect ? "true" : "false";
	putText(mat_buffer_frame, "mb_detect :" + str, Point(20, mat_buffer_frame.rows - 80), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 3);

}

void CMFCSDKOpencvDlg::Track()
{

	//detection 진행 중 아닐 때는 점 하나씩 지우는 것도 코드에 넣어야됨
	if (mb_detect)
	{
		if (mvp_points.size() < 30)
		{
			for (int i = 0; i < mvp_points.size(); i++)
			{
				line(mat_frame, mvp_points[i], mvp_points[i], Scalar(0, 255, 255), 10);
			}
		}
		else
		{
			mvp_points.erase(mvp_points.begin());
			for (int i = 0; i < mvp_points.size(); i++)
			{
				line(mat_frame, mvp_points[i], mvp_points[i], Scalar(0, 255, 255), 10);
			}
		}
	}
	else
	{
		if (mvp_points.size() > 0)
		{
			mvp_points.erase(mvp_points.begin());
		}
		for (int i = 0; i < mvp_points.size(); i++)
		{
			line(mat_frame, mvp_points[i], mvp_points[i], Scalar(0, 255, 255), 10);
		}
	}

	if (!mat_frame.empty())
	{
		//imshow("mat_frame", mat_frame);
		imshow("mat_buffer_frame", mat_buffer_frame);
	}

}



void CMFCSDKOpencvDlg::OnBnClickedButtonRecord()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CEdit* pBtnRecord;

	pBtnRecord = (CEdit*)GetDlgItem(IDC_BUTTON1);

	if (!mb_record)
	{
		videoWriter = new VideoWriter;

		pBtnRecord->SetWindowText(_T("Stop"));
		mb_record = !mb_record;

		time_t timer = time(NULL);
		tm* t = localtime(&timer);
		String hour, min, sec;
		hour = t->tm_hour < 10 ? "0" + to_string(t->tm_hour) : to_string(t->tm_hour);
		min = t->tm_min < 10 ? "0" + to_string(t->tm_min) : to_string(t->tm_min);
		sec = t->tm_sec < 10 ? "0" + to_string(t->tm_sec) : to_string(t->tm_sec);

		float fps = 30.0;
		int width = m_previnfo.col_max;
		int height = m_previnfo.row_max;
		String filepath, outputvideoname;
		filepath = "C:/videos/4in1/";
		outputvideoname = filepath + hour + min + sec + "_" + to_string(width) + "x" + to_string(height) + ".yuv";

		//videoWriter->open(outputvideoname, videoWriter->fourcc('M', 'J', 'P', 'G'), fps, Size(width, height), true);
		videoWriter->open(outputvideoname, videoWriter->fourcc('N', 'V', '2', '1'), fps, Size(width, height), true);


	}
	else
	{
		pBtnRecord->SetWindowTextW(_T("Record"));
		mb_record = !mb_record;
		delete videoWriter;
	}

}

void CMFCSDKOpencvDlg::StitchbyOpenCV()
{
	// EstimateH와 MakePanorama의 3번째 파라미터 Idx 0: mat_cam[1] -> mat_cam[0]으로 warp; 1: mat_cam[3] -> mat_cam[2]로 warp; 2: mat2 -> mat1로 warp.


	Mat mat1, mat2, mat3;
	if (m_previnfo.enough[0] && m_previnfo.enough[1] && m_previnfo.enough[2])
	{
		mat1 = MakePanorama(mat_cam[0], mat_cam[1], 0);
		mat2 = MakePanorama(mat_cam[2], mat_cam[3], 1);
		rotate(mat1, mat1, ROTATE_90_COUNTERCLOCKWISE);
		rotate(mat2, mat2, ROTATE_90_COUNTERCLOCKWISE);
		mat3 = MakePanorama(mat1, mat2, 2);
		rotate(mat3, mat3, ROTATE_90_CLOCKWISE);
		mat_frame = mat3.clone();
		if (m_previnfo.row_max == 0 && m_previnfo.col_max == 0)
		{
			Mat gray;
			cvtColor(mat_frame, gray, COLOR_BGR2GRAY);

			int row_max, col_max;
			row_max = col_max = 0;
			for (int row = 0; row < gray.rows; row++)
			{
				for (int col = 0; col < gray.cols; col++)
				{
					if (gray.at<uchar>(row, col))
					{
						if (row > row_max) row_max = row;
						if (col > col_max) col_max = col;
					}
				}
			}
			m_previnfo.row_max = row_max+1;
			m_previnfo.col_max = col_max+1;


			cout << "row_max : " << row_max << endl << "col_max : " << col_max << endl;
		}

		if (m_previnfo.row_max % 4)
		{
			m_previnfo.row_max -= m_previnfo.row_max % 4;
		}
		if (m_previnfo.col_max % 4)
		{
			m_previnfo.col_max -= m_previnfo.col_max % 4;
		}

		mat_frame = mat_frame(Range(0, m_previnfo.row_max), Range(0, m_previnfo.col_max));
		cout << "mat_frame.rows : " << mat_frame.rows << endl << "mat_frame.cols : " << mat_frame.cols << endl;



		//imshow("mat3", mat3);

		//cout << "finished" << endl;
		return;
	}




	EstimateH(mat_cam[0], mat_cam[1], 0);
	EstimateH(mat_cam[2], mat_cam[3], 1);
	//imshow("mat1", mat1);
	//imshow("mat2", mat2);
	double k1, k2, k3;
	k1 = 0.2;
	k2 = 0.0001;
	k3 = 1;


	for (int idx = 0; idx < 2; idx++)
	{


		if (m_previnfo.BestMat[idx].empty())
		{
			m_previnfo.BestMat[idx] = m_commondata.h[idx].clone();
		}
		else
		{
			Mat a = m_previnfo.BestMat[idx].clone(); // 이전 최적 transform amtrix
			Mat b = m_commondata.h[idx].clone(); // estimateH로부터 계산된 matrix가 들어가 있음

			//double cost_a = a.at<double>(0, 0) * a.at<double>(0, 0) + a.at<double>(0, 1) * a.at<double>(0, 1);
			//double cost_b = b.at<double>(0, 0) * b.at<double>(0, 0) + b.at<double>(0, 1) * b.at<double>(0, 1);

			double cost_a = abs(1 - a.at<double>(0, 0)) + k1 * abs(a.at<double>(0, 1)) + k2 * abs(a.at<double>(1, 2)) + k3 * ( abs(1 - pow(( pow(a.at<double>(0, 0), 2) + pow(a.at<double>(0, 1), 2) ), 0.5)) );
			double cost_b = abs(1 - b.at<double>(0, 0)) + k1 * abs(b.at<double>(0, 1)) + k2 * abs(b.at<double>(1, 2)) + k3 * ( abs(1 - pow(( pow(b.at<double>(0, 0), 2) + pow(b.at<double>(0, 1), 2) ), 0.5)) );
			//cost_a = abs(1 - cost_a);
			//cost_b = abs(1 - cost_b);
			//cout << "matrix a, idx = " << idx << endl;
			//cout << "|1-a| = " << abs(1 - a.at<double>(0, 0)) << endl;
			//cout << "|b| = " << abs(a.at<double>(0, 1)) << endl;
			//cout << "|d| = " << abs(a.at<double>(1, 2)) << endl;
			//cout << "|1 - root of a^2 + b^2 | = " << abs(1 - pow(( pow(a.at<double>(0, 0), 2) + pow(a.at<double>(0, 1), 2) ), 0.5)) << endl;
			//cout << "cost_a = " << cost_a << endl;
			//cout << "========================" << endl;

			//cout << "matrix b, idx = " << idx <<  endl;
			//cout << "|1-a| = " << abs(1 - b.at<double>(0, 0)) << endl;
			//cout << "|b| = " << abs(b.at<double>(0, 1)) << endl;
			//cout << "|d| = " << abs(b.at<double>(1, 2)) << endl;
			//cout << "|1 - root of a^2 + b^2 | = " << abs(1 - pow(( pow(b.at<double>(0, 0), 2) + pow(b.at<double>(0, 1), 2) ), 0.5)) << endl;
			//cout << "cost_b = " << cost_b << endl;
			//cout << "========================" << endl;


			if (cost_a * 1.1 < cost_b)
			{
				m_commondata.h[idx] = m_previnfo.BestMat[idx].clone();
				m_previnfo.notupdatecnt[idx] ++;
				if (m_previnfo.notupdatecnt[idx] > 30)
				{
					m_previnfo.enough[idx] = true;
				}
			}
			else
			{
				for (int row = 0; row < m_previnfo.BestMat[idx].rows; row++)
				{
					for (int col = 0; col < m_previnfo.BestMat[0].cols; col++)
					{
						m_previnfo.BestMat[idx].at<double>(row, col) = ( m_previnfo.BestMat[idx].at<double>(row, col) * 19 + m_commondata.h[idx].at<double>(row, col) ) / 20;
						//m_previnfo.BestMat[idx].at<double> (row, col) = m_commondata.h[idx].at<double>(row, col);
					}
				}
				m_commondata.h[idx] = m_previnfo.BestMat[idx].clone();
				m_previnfo.updatecnt[idx]++;
				m_previnfo.notupdatecnt[idx] = 0;
				if (m_previnfo.updatecnt[idx] > 50)
				{
					m_previnfo.enough[idx] = true;
				}
			}
		}
	}

	mat1 = MakePanorama(mat_cam[0], mat_cam[1], 0);
	mat2 = MakePanorama(mat_cam[2], mat_cam[3], 1);

	//imshow("mat1", mat1);
	//imshow("mat2", mat2);

	rotate(mat1, mat1, ROTATE_90_COUNTERCLOCKWISE);
	rotate(mat2, mat2, ROTATE_90_COUNTERCLOCKWISE);


	EstimateH(mat1, mat2, 2);

	if (m_previnfo.BestMat[2].empty())
	{
		m_previnfo.BestMat[2] = m_commondata.h[2].clone();
	}
	else
	{
		Mat a = m_previnfo.BestMat[2].clone();
		Mat b = m_commondata.h[2].clone();

		double cost_a = abs(1 - a.at<double>(0, 0)) + k1 * abs(a.at<double>(0, 1)) + k2 * abs(a.at<double>(1, 2)) + k3 * ( abs(1 - pow(( pow(a.at<double>(0, 0), 2) + pow(a.at<double>(0, 1), 2) ), 0.5)) );
		double cost_b = abs(1 - b.at<double>(0, 0)) + k1 * abs(b.at<double>(0, 1)) + k2 * abs(b.at<double>(1, 2)) + k3 * ( abs(1 - pow(( pow(b.at<double>(0, 0), 2) + pow(b.at<double>(0, 1), 2) ), 0.5)) );
		//cost_a = abs(1 - cost_a);
		//cost_b = abs(1 - cost_b);
		//cout << "matrix a, idx = " << 2 << endl;
		//cout << "|1-a| = " << abs(1 - a.at<double>(0, 0)) << endl;
		//cout << "|b| = " << abs(a.at<double>(0, 1)) << endl;
		//cout << "|d| = " << abs(a.at<double>(1, 2)) << endl;
		//cout << "|1 - root of a^2 + b^2 | = " << abs(1 - pow(( pow(a.at<double>(0, 0), 2) + pow(a.at<double>(0, 1), 2) ), 0.5)) << endl;
		//cout << "cost_a = " << cost_a << endl;
		//cout << "========================" << endl;

		//cout << "matrix b, idx = " << 2 << endl;
		//cout << "|1-a| = " << abs(1 - b.at<double>(0, 0)) << endl;
		//cout << "|b| = " << abs(b.at<double>(0, 1)) << endl;
		//cout << "|d| = " << abs(b.at<double>(1, 2)) << endl;
		//cout << "|1 - root of a^2 + b^2 | = " << abs(1 - pow(( pow(b.at<double>(0, 0), 2) + pow(b.at<double>(0, 1), 2) ), 0.5)) << endl;
		//cout << "cost_b = " << cost_b << endl;
		//cout << "========================" << endl;

		if (cost_a  * 1.1 < cost_b)
		{
			m_commondata.h[2] = m_previnfo.BestMat[2].clone();
			m_previnfo.notupdatecnt[2]++;
			if (m_previnfo.notupdatecnt[2] > 30)
			{
				m_previnfo.enough[2] = true;
			}
		}
		else
		{
			for (int row = 0; row < m_previnfo.BestMat[2].rows; row++)
			{
				for (int col = 0; col < m_previnfo.BestMat[0].cols; col++)
				{
					m_previnfo.BestMat[2].at<double>(row, col) = ( m_previnfo.BestMat[2].at<double>(row, col) * 19 + m_commondata.h[2].at<double>(row, col) ) / 20;
				}
			}
			m_commondata.h[2] = m_previnfo.BestMat[2].clone();
			m_previnfo.updatecnt[2]++;
			m_previnfo.notupdatecnt[2] = 0;
			if (m_previnfo.updatecnt[2] > 50)
			{
				m_previnfo.enough[2] = true;
			}

		}
	}
	for (int i = 0; i < 3; i++)
	{
		cout << "m_previnfo.updatecnt[" << i << "] : " << m_previnfo.updatecnt[i] << endl;
	}

	mat3 = MakePanorama(mat1, mat2, 2);

	rotate(mat3, mat3, ROTATE_90_CLOCKWISE);

	//imshow("mat3", mat3);

	//if (!mat3.empty())
	//{
	//	imshow("mat3", mat3);
	//}
	//imshow("mat1", mat1);
	//imshow("mat2", mat2);
	return;

}



void CMFCSDKOpencvDlg::EstimateH(Mat matLeftImage, Mat matRightImage, int idx)
{
	const int MAX_FEATURES = 1000;
	const float GOOD_MATCH_PERCENT = 0.3f;

	Mat ret;

	// Convert images to grayscale
	Mat im1Gray, im2Gray;
	cvtColor(matLeftImage, im1Gray, cv::COLOR_BGR2GRAY);
	cvtColor(matRightImage, im2Gray, cv::COLOR_BGR2GRAY);

	// Variables to store keypoints and descriptors
	std::vector<KeyPoint> keypoints1, keypoints2;
	Mat descriptors1, descriptors2;

	// Detect ORB features and compute descriptors.
	Ptr<Feature2D> orb = ORB::create(MAX_FEATURES, 1.2f, 8, 31, 0, 2, ORB::HARRIS_SCORE, 31, 20);
	Mat mask, roi;
	mask.create(im1Gray.rows, im1Gray.cols, CV_8UC1);
	mask.setTo(Scalar(0));
	roi = mask(Rect(mask.cols / 2, 0, mask.cols / 2, mask.rows));
	roi.setTo(Scalar(255));

	orb->detectAndCompute(im1Gray, mask, keypoints1, descriptors1);
	bitwise_not(mask, mask);
	orb->detectAndCompute(im2Gray, mask, keypoints2, descriptors2);

	// Match features.
	std::vector<DMatch> matches;
	//Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce-Hamming");
	Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::BRUTEFORCE_HAMMING);
	if (!descriptors1.empty() && !descriptors2.empty())
	{
		matcher->match(descriptors1, descriptors2, matches, Mat());
	}
	else
	{
		return;
	}

	// Sort matches by score
	std::sort(matches.begin(), matches.end());

	// Remove not so good matches
	const int numGoodMatches = matches.size() * GOOD_MATCH_PERCENT;
	matches.erase(matches.begin() + numGoodMatches, matches.end());

	// Draw top matches
	Mat imMatches;
	drawMatches(matLeftImage, keypoints1, matRightImage, keypoints2, matches, imMatches);
	//imshow("matches.jpg", imMatches);

	// Extract location of good matches
	std::vector<Point2f> points1, points2;

	for (size_t i = 0; i < matches.size(); i++)
	{
		points1.push_back(keypoints1[matches[i].queryIdx].pt);
		points2.push_back(keypoints2[matches[i].trainIdx].pt);
	}

	// Find homography
	//Mat h = findHomography(points2, points1, RANSAC);
	Mat h = estimateRigidTransform(points2, points1, false);
	m_commondata.h[idx] = h.clone();

	
	//for (int row = 0; row < h.rows; row++)
	//{
	//	for (int col = 0; col < h.cols; col++)
	//	{
	//		cout << h.at<double>(row, col) << " " ;
	//	}
	//	cout << endl;
	//}
	//cout << "----------" << endl;

	
	ret.create(matLeftImage.rows, matLeftImage.cols * 2, CV_8UC3);
	// Use homography to warp image
	//warpPerspective(matRightImage, ret, h, Size(matLeftImage.cols * 2, matLeftImage.rows));
	warpAffine(matRightImage, ret, h, Size(matLeftImage.cols * 2, matLeftImage.rows));

	//Mat roi2(ret, Rect(0, 0, matLeftImage.cols * clipping_coeff, matLeftImage.rows));
	Mat roi2(ret, Rect(0, 0, matLeftImage.cols, matLeftImage.rows));
	//left_clip.copyTo(roi2);
	mask.create(matLeftImage.rows, matLeftImage.cols, CV_8UC1);
	cvtColor(roi2, mask, COLOR_BGR2GRAY);
	threshold(mask, mask, 1, 255, THRESH_BINARY);
	int mask_row = mask.rows / 2;
	int mask_x = 0, x_range = 25;
	for (int col = 0; col < mask.cols; col++)
	{
		if (mask.at<uchar>(mask_row, col))
		{
			mask_x = col;
			break;
		}
	}

	//m_commondata.mask_x[idx] = mask_x;
	m_commondata.mask_x[idx] = h.at<double>(0, 2);


	//for (int row = 0; row < h.rows; row++)
	//{
	//	for (int col = 0; col < h.cols; col++)
	//	{
	//		cout << h.at<double>(row, col) << "|" ;
	//	}
	//	cout << endl;
	//}
	//cout << endl;

	return;
}

Mat CMFCSDKOpencvDlg::MakePanorama(Mat matLeftImage, Mat matRightImage, int Idx)
{
	Mat ret;

	Mat h, temp;
	h.create(2, 3, CV_64FC1);
	h.at<double>(0, 0) = 1;
	h.at<double>(0, 1) = 0;
	h.at<double>(0, 2) = m_commondata.mask_x[Idx];// *m_commondata.h[Idx].at<double>(0, 0);
	//h.at<double>(0, 2) = m_commondata.h[Idx].at<double>(0, 2);
	h.at<double>(1, 0) = 0;
	h.at<double>(1, 1) = 1;
	h.at<double>(1, 2) = 0;

	//for (int row = 0; row < temp.rows; row++)
	//{
	//	for (int col = 0; col < temp.cols; col++)
	//	{
	//		cout << temp.at<double>(row, col) << " ";
	//	}
	//	cout << endl;
	//}
	//cout << "mask_x : " << m_commondata.mask_x[Idx] << endl;
	//cout << "----------" << endl;



	//int mask_x = m_commondata.mask_x[Idx];
	int mask_x = m_commondata.h[Idx].at<double>(0, 2);
	int mask_x_temp = mask_x;

	ret.create(matLeftImage.rows, matLeftImage.cols + mask_x, CV_8UC3);
	//warpAffine(matRightImage, ret, h, Size(matLeftImage.cols + mask_x, matLeftImage.rows));
	warpAffine(matRightImage, ret, m_commondata.h[Idx], Size(matLeftImage.cols * 2 , matLeftImage.rows));

	mask_x = matLeftImage.cols - ( ( matLeftImage.cols - mask_x ) / 2 );

	Mat roi(ret, Rect(0, 0, mask_x, ret.rows));
	Mat clipped(matLeftImage, Rect(0, 0, mask_x, matLeftImage.rows));
	clipped.copyTo(roi);

	Mat matLeft_clip;
	int x_range = 15;

	if (mask_x > x_range && mask_x < matLeftImage.cols - x_range)
	{
		matLeft_clip = matLeftImage(Range(0, matLeftImage.rows), Range(mask_x - x_range, mask_x + x_range)).clone();

		Mat mask;
		mask.create(matLeft_clip.rows, matLeft_clip.cols, CV_8UC1);
		mask.setTo(Scalar(255));

		seamlessClone(matLeft_clip, ret, mask, Point(mask_x, matLeftImage.rows / 2), ret, NORMAL_CLONE);

	}

	return ret;
}


void CMFCSDKOpencvDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (IDC_SLIDER_TEST == pScrollBar->GetDlgCtrlID())
	{
		int iPos = m_sldTest.GetPos();
		m_sldinfo[m_cursel].cur_coarse = iPos;
		CString sPos;
		sPos.Format(_T("%d"), iPos);
		m_edTest.SetWindowTextW(sPos);
	}
	else if(IDC_SLIDER_VERTICAL == pScrollBar->GetDlgCtrlID())
	{
		int iPos = m_sld_vertical.GetPos();
		m_sldinfo[m_cursel].cur_fine = iPos;
		CString sPos;
		sPos.Format(_T("%d"), iPos);
		m_edit_vertical.SetWindowTextW(sPos);
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CMFCSDKOpencvDlg::OnEnChangeEditTest()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_cursel > -1 && m_cursel < 4)
	{
		CString sPos;
		m_edTest.GetWindowTextW(sPos);
		int iPos = _ttoi(sPos);

		m_sldTest.SetPos(iPos);
		m_sldinfo[m_cursel].cur_coarse = iPos;
	}
}


void CMFCSDKOpencvDlg::OnEnChangeEditVertical()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString sPos;
	m_edit_vertical.GetWindowTextW(sPos);
	int iPos = _ttoi(sPos);
	m_sld_vertical.SetPos(iPos);
	m_sldinfo[m_cursel].cur_fine = iPos;
}



void CMFCSDKOpencvDlg::OnCbnSelchangeComboTest()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_cursel = m_cbTest.GetCurSel();
	if (m_cursel < 0 || m_cursel > 3)
	{
		m_cursel = 0;
	}

	if (CB_ERR != m_cursel)
	{
		CString sSel;

		m_cbTest.GetLBText(m_cursel, sSel);
	}
	int iPos;
	CString sPos;

	iPos = m_sldinfo[m_cursel].cur_coarse;
	m_sldTest.SetPos(iPos);
	sPos.Format(_T("%d"), iPos);
	m_edTest.SetWindowTextW(sPos);


	iPos = m_sldinfo[m_cursel].cur_fine;
	m_sld_vertical.SetPos(iPos);
	sPos.Format(_T("%d"), iPos);
	m_edit_vertical.SetWindowTextW(sPos);


}

void CMFCSDKOpencvDlg::WarpbyMouseClick()
{
	for (int i = 0; i < CAMERA_NUM; i++)
	{
		imshow("mat_cam[" + to_string(i) + "]", mat_cam[i]);
	}
	setMouseCallback("mat_cam[0]", onMouse, reinterpret_cast<void*>(&mat_cam[0]));
	setMouseCallback("mat_cam[1]", onMouse, reinterpret_cast<void*>(&mat_cam[1]));
	setMouseCallback("mat_cam[2]", onMouse, reinterpret_cast<void*>(&mat_cam[2]));
	setMouseCallback("mat_cam[3]", onMouse, reinterpret_cast<void*>(&mat_cam[3]));
	if (!mat_wmcdst[0].empty())
	{
		setMouseCallback("dst[0]", onMouse, reinterpret_cast<void*>(&mat_wmcdst[0]));
	}
	if (!mat_wmcdst[1].empty())
	{
		setMouseCallback("dst[1]", onMouse, reinterpret_cast<void*>(&mat_wmcdst[1]));
	}


	//timeKillEvent(m_nTimerID);
	//waitKey(0);
	Mat M;
	Mat dst_gray;
	if (m_pts_src[0].size() == 3 && m_pts_dst[0].size() == 3)
	{
		M = getAffineTransform(m_pts_src[0], m_pts_dst[0]);
		warpAffine(mat_cam[1], mat_wmcdst[0], M, Size(mat_cam[0].cols * 2, mat_cam[0].rows));
		int mid_row = mat_cam[0].rows / 2;
		int min_col = 0;
		cvtColor(mat_wmcdst[0], dst_gray, COLOR_BGR2GRAY);
		for (int col = 0; col < dst_gray.cols; col++)
		{
			if (dst_gray.at<uchar>(mid_row, col))
			{
				min_col = col;
				break;
			}
		}
		int x = mat_cam[0].cols - (mat_cam[0].cols - min_col) / 2;
		int x_range = 25;
		Mat src_clip = mat_cam[0](Range(0, mat_cam[0].rows), Range(0, x));
		Mat mask;
		Mat roi = mat_wmcdst[0](Rect(0, 0, src_clip.cols, src_clip.rows));
		src_clip.copyTo(roi);
		src_clip = mat_cam[0](Range(0, mat_cam[0].rows), Range(x - x_range, x + x_range)).clone();
		mask.create(src_clip.rows, src_clip.cols, CV_8UC1);
		mask.setTo(Scalar(255));
		seamlessClone(src_clip, mat_wmcdst[0], mask, Point(x, src_clip.rows / 2), mat_wmcdst[0], NORMAL_CLONE);
		x = mat_cam[0].cols - min_col;
		mat_wmcdst[0] = mat_wmcdst[0](Range(0, mat_wmcdst[0].rows), Range(0, mat_wmcdst[0].cols - x));
		imshow("dst[0]", mat_wmcdst[0]);
	}
	if (m_pts_src[1].size() == 3 && m_pts_dst[1].size() == 3)
	{
		M = getAffineTransform(m_pts_src[1], m_pts_dst[1]);
		warpAffine(mat_cam[3], mat_wmcdst[1], M, Size(mat_cam[2].cols * 2, mat_cam[2].rows));
		int mid_row = mat_cam[2].rows / 2;
		int min_col = 0;
		cvtColor(mat_wmcdst[1], dst_gray, COLOR_BGR2GRAY);
		for (int col = 0; col < dst_gray.cols; col++)
		{
			if (dst_gray.at<uchar>(mid_row, col))
			{
				min_col = col;
				break;
			}
		}
		int x = mat_cam[2].cols - (mat_cam[2].cols - min_col) / 2;
		int x_range = 25;
		Mat src_clip = mat_cam[2](Range(0, mat_cam[2].rows), Range(0, x));
		Mat mask;
		Mat roi = mat_wmcdst[1](Rect(0, 0, src_clip.cols, src_clip.rows));
		src_clip.copyTo(roi);
		src_clip = mat_cam[2](Range(0, mat_cam[2].rows), Range(x - x_range, x + x_range)).clone();
		mask.create(src_clip.rows, src_clip.cols, CV_8UC1);
		mask.setTo(Scalar(255));
		seamlessClone(src_clip, mat_wmcdst[1], mask, Point(x, src_clip.rows / 2), mat_wmcdst[1], NORMAL_CLONE);
		x = mat_cam[2].cols - min_col;
		mat_wmcdst[1] = mat_wmcdst[1](Range(0, mat_wmcdst[1].rows), Range(0, mat_wmcdst[1].cols - x));
		imshow("dst[1]", mat_wmcdst[1]);
	}
	if (m_pts_src[2].size() == 3 && m_pts_dst[2].size() == 3)
	{
		M = getAffineTransform(m_pts_src[2], m_pts_dst[2]);
		warpAffine(mat_wmcdst[1], mat_wmcdst[2], M, Size(mat_wmcdst[0].cols, mat_wmcdst[0].rows * 2));
		int mid_col = mat_wmcdst[0].cols / 2;
		int min_row = 0;
		cvtColor(mat_wmcdst[2], dst_gray, COLOR_BGR2GRAY);
		for (int row = 0; row < dst_gray.rows; row++)
		{
			if (dst_gray.at<uchar>(row, mid_col))
			{
				min_row = row;
				break;
			}
		}
		int y = mat_wmcdst[0].rows - (mat_wmcdst[0].rows - min_row) / 2;
		int y_range = 20;
		Mat src_clip = mat_wmcdst[0](Range(0, y), Range(0, mat_wmcdst[0].cols));
		Mat mask;
		Mat roi = mat_wmcdst[2](Rect(0, 0, src_clip.cols, src_clip.rows));
		src_clip.copyTo(roi);
		src_clip = mat_wmcdst[0](Range(y - y_range, y + y_range), Range(0, mat_wmcdst[0].cols)).clone();
		mask.create(src_clip.rows, src_clip.cols, CV_8UC1);
		mask.setTo(Scalar(255));
		seamlessClone(src_clip, mat_wmcdst[2], mask, Point(src_clip.cols / 2, y), mat_wmcdst[2], NORMAL_CLONE);
		y = mat_wmcdst[0].rows - min_row;
		mat_wmcdst[2] = mat_wmcdst[2](Range(0, mat_wmcdst[2].rows - y), Range(0, mat_wmcdst[2].cols));
		imshow("dst[2]", mat_wmcdst[2]);
	}
}

void onMouse(int event, int x, int y, int flags, void* param)
{
	Mat* im = reinterpret_cast<Mat*>(param);

	if (event == EVENT_LBUTTONDOWN)
	{
		if (im == &(g_pdlg->mat_cam[0]) && g_pdlg->m_pts_dst[0].size() < 3)
		{
			g_pdlg->m_pts_dst[0].push_back(Point(x, y));
		}
		else if (im == &(g_pdlg->mat_cam[1]) && g_pdlg->m_pts_src[0].size() < 3)
		{
			g_pdlg->m_pts_src[0].push_back(Point(x, y));
		}
		else if (im == &(g_pdlg->mat_cam[2]) && g_pdlg->m_pts_dst[1].size() < 3)
		{
			g_pdlg->m_pts_dst[1].push_back(Point(x, y));
		}
		else if (im == &(g_pdlg->mat_cam[3]) && g_pdlg->m_pts_src[1].size() < 3)
		{
			g_pdlg->m_pts_src[1].push_back(Point(x, y));
		}
		else if (im == &(g_pdlg->mat_wmcdst[0]) && g_pdlg->m_pts_dst[2].size() < 3)
		{
			g_pdlg->m_pts_dst[2].push_back(Point(x, y));
		}
		else if (im == &(g_pdlg->mat_wmcdst[1]) && g_pdlg->m_pts_src[2].size() < 3)
		{
			g_pdlg->m_pts_src[2].push_back(Point(x, y));
		}
	}
}