#include "main.h"

const int TrayIcon = WM_USER + 1, N = 8, M = 6;
const double K = 0.3, L = 0.4;
HWND WindowHandle;
bool Start = false, Show = true;
PROCESS_INFORMATION *ArOfProducers = new PROCESS_INFORMATION[N], *ArOfConsumers = new PROCESS_INFORMATION[M];
DWORD ProcID;
int NumOfElemInBuf = 0;
char **ArOfProd, **ArOfCons;

//Главная функция 
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance, 
	LPSTR lpszCmdParam,int nCmdShow) 
{ 
	hInst=hInstance; 
	WNDCLASS WndClass; 
	MSG Msg; 
	char ClassName[]="Manager"; 
	HWND hWnd; 
	WndClass.style=CS_HREDRAW | CS_VREDRAW; 
	WndClass.lpfnWndProc=WIN32PROC; 
	WndClass.cbClsExtra=0; 
	WndClass.cbWndExtra=0; 
	WndClass.hInstance=hInstance; 
	WndClass.hIcon=LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON1));
	WndClass.hCursor=LoadCursor(NULL,IDC_ARROW); 
	WndClass.hbrBackground=(HBRUSH)NULL; 
	WndClass.lpszMenuName=NULL; 
	WndClass.lpszClassName=ClassName; 
	if (!RegisterClass(&WndClass)) 
	{ 
		MessageBox(NULL,"Cannot register class","Error",MB_OK | MB_ICONERROR); 
		return 0; 
	} 
	hWnd=CreateWindowEx(0, ClassName,"Решение задачи производителя и потребителя", 
		WS_OVERLAPPEDWINDOW, 
		300,100,350,340, 
		NULL,NULL,hInstance,NULL); 
	if (hWnd==NULL) 
	{ 
		MessageBox(NULL,"Cannot create window","Error",MB_OK | MB_ICONERROR); 
		return 0; 
	} 
	ShowWindow(hWnd,nCmdShow); 
	UpdateWindow(hWnd); 
	MSG msg; 
	while (1) 
	{ 
		while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == TRUE) 
		{ 
			if (GetMessage(&msg, NULL, 0, 0) ) 
			{ 
				TranslateMessage(&msg); 
				DispatchMessage(&msg); 
			} 
			else 
			{ 
				return TRUE; 
			} 
		} 
	} 
	return Msg.wParam; 
} 

//функция обработки сообщения диалогового окна
LRESULT CALLBACK WIN32DIAL(HWND hWnd,UINT Message, 
	UINT wParam,LONG lParam) 
{ 
	return DefWindowProc(hWnd,Message,wParam,lParam); 
}

//ф-ция добавления системной иконки
void AddTrayIcon(HWND hWnd)
{
	NOTIFYICONDATA NID;
	NID.cbSize = sizeof(NOTIFYICONDATA); 
	NID.hWnd = hWnd; 
	NID.uID = 1; 
	NID.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP; 
	NID.uCallbackMessage = TrayIcon;//указатель на создаваемое событие от иконки 
	NID.hIcon = LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON1)); 
	strcpy(NID.szTip,"Решение задачи производителя и потребителя");
    Shell_NotifyIcon(NIM_ADD,&NID);
}

//функция обработки сообщений от иконки 
int OnTray(HWND hWnd,UINT wParam,LONG lParam) 
{ 
	switch(lParam) 
	{ 
	case WM_LBUTTONDBLCLK: 
		ShowWindow(hWnd,SW_SHOWNORMAL); 
		SetForegroundWindow(hWnd);
		if (Show==false)
			Show = true;
		break;
	case WM_RBUTTONDOWN:
		HMENU hMenu = CreatePopupMenu();
		AppendMenu( hMenu, MFT_STRING, 3, "Скрыть/Показать" );
		AppendMenu( hMenu, MFT_STRING, 4, "Выход" );
		SetForegroundWindow(hWnd);
		POINT pt;
		GetCursorPos(&pt);
		TrackPopupMenu( hMenu, TPM_LEFTALIGN, 
			pt.x, pt.y, 0, hWnd, NULL );
		break;
	} 
	return 0; 
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	DWORD pid;
	GetWindowThreadProcessId(hwnd, &pid);
	if (pid == ProcID)
	{
		WindowHandle = hwnd;
		return false;
	}
	return true;
}

void StartModel()
{
	int i, j;
	COPYDATASTRUCT cds;
	char message[256];
	STARTUPINFO * arOfCif;
	arOfCif = new STARTUPINFO[M];
	for(i = 0; i<M; i++)
	{
		ZeroMemory(&arOfCif[i],sizeof(STARTUPINFO));
		CreateProcess("F:\\Дисциплины\\ОС\\лб_10_ос\\Копия lb_1_os\\Release\\lb_1_os.exe",NULL, 
			NULL,NULL,FALSE,NULL,NULL,NULL,&arOfCif[i],&ArOfConsumers[i]);
		ProcID = ArOfConsumers[i].dwProcessId;
		Sleep(1000);
		EnumWindows(EnumWindowsProc,NULL);
		cds.dwData = 0;
		itoa((i+1)*L*1000,message,10);
		cds.cbData = strlen(message)+1;
		cds.lpData = message;
		SendMessage(WindowHandle,WM_COPYDATA,0,(LPARAM)(&cds));
	}
	arOfCif = new STARTUPINFO[N];
	for(i = 0; i<N; i++)
	{
		ZeroMemory(&arOfCif[i],sizeof(STARTUPINFO));
		CreateProcess("F:\\Дисциплины\\ОС\\лб_10_ос\\Копия Копия lb_1_os\\Release\\lb_1_os.exe",NULL, 
			NULL,NULL,FALSE,NULL,NULL,NULL,&arOfCif[i],&ArOfProducers[i]);
		ProcID = ArOfProducers[i].dwProcessId;
		Sleep(1000);
		EnumWindows(EnumWindowsProc,NULL);
		cds.dwData = 0;
		itoa((i+1)*K*1000,message,10);
		cds.cbData = strlen(message)+1;
		cds.lpData = message;
		SendMessage(WindowHandle,WM_COPYDATA,0,(LPARAM)(&cds));
	}
}

void StopModel()
{
	int i;
	for(i = 0; i<N; i++)
		TerminateProcess(ArOfProducers[i].hProcess,NO_ERROR);
	for(i = 0; i<M; i++)
		TerminateProcess(ArOfConsumers[i].hProcess,NO_ERROR);
}

void DelTrayIcon(HWND hWnd)
{
	NOTIFYICONDATA NID;
	NID.cbSize = sizeof(NOTIFYICONDATA); 
	NID.hWnd = hWnd; 
	NID.uID = 1;
	Shell_NotifyIcon(NIM_DELETE,&NID);
}

void CreateStr(char *str, char *arOfStr[], int n)
{
	int i;
	char new_str[3]={ 0x0d, 0x0a,0 }, buf[256];
	for(i = 0; i<n; i++)
	{
		if (i!=0)
			strcat(str,new_str);
		itoa(i+1,buf,10);
		strcat(str,buf);
		strcat(str," - ");
		strcat(str,arOfStr[i]);
	}
}

//функция обработки сообщений 
LRESULT CALLBACK WIN32PROC(HWND hWnd,UINT Message, 
	UINT wParam,LONG lParam) 
{
	int i, number;
	UINT wID;
	ofstream out;
	DWORD procID;
	char message[5256];
	switch(Message)
	{ 
	case WM_CREATE:
		i = AddMenu(hWnd);
		if (i==1)
			return 1;
		hEdit=CreateDialog(hInst,MAKEINTRESOURCE(IDD_DIALOG1),hWnd,(DLGPROC) WIN32DIAL);
		if (hEdit==NULL) 
			return 1;
		SizeEdit(hWnd);
		AddTrayIcon(hWnd);
		break;

	case WM_COMMAND:
		wID = LOWORD(wParam);
		switch(wID)
		{
		case 2:
			if (Start)
			{
				Start = false;
				StopModel();
			}
			else
			{
				Start = true;
				NumOfElemInBuf = 0;
				out.open("F:\\Дисциплины\\ОС\\лб_10_ос\\lb_1_os\\lb_1_os\\buffer.txt",ios::out);
				out.close();
				ArOfProd = new char*[N];
				for(i = 0; i < N; i++)
					ArOfProd[i] = new char[256];
				for(i = 0; i < N; i++)
					strcpy(ArOfProd[i],"0");
				ArOfCons = new char*[M];
				for(i = 0; i < M; i++)
					ArOfCons[i] = new char[256];
				for(i = 0; i < M; i++)
					strcpy(ArOfCons[i],"0");
				message[0] = '\0';
				CreateStr(message,ArOfProd,N);
				SetDlgItemText(hEdit,IDC_EDIT2,message);
				message[0] = '\0';
				CreateStr(message,ArOfCons,M);
				SetDlgItemText(hEdit,IDC_EDIT1,message);
				StartModel();
			}
			break;
		case 3:
			if (!Show)
			{
				ShowWindow(hWnd, SW_SHOW);
				ShowWindow(hWnd, SW_RESTORE);
				Show = true;
			}
			else
			{
				ShowWindow(hWnd, SW_MINIMIZE);
				ShowWindow(hWnd, SW_HIDE);
				Show = false;
			}
			SetForegroundWindow(hWnd);
			break;
		case 15:
		case 4:
			DestroyWindow(hWnd);
			break;
		}
		break;

	case WM_COPYDATA:
		strcpy(message,(char*)(((PCOPYDATASTRUCT)lParam)->lpData));
		procID = atoi(message);
		message[0] = '\0';
		for(i = 0; i<N; i++)
			if (ArOfProducers[i].dwProcessId==procID)
			{
				NumOfElemInBuf++;
				number = atoi(ArOfProd[i]);
				number++;
				itoa(number,ArOfProd[i],10);
				CreateStr(message,ArOfProd,N);
				SetDlgItemText(hEdit,IDC_EDIT2,message);
				break;
			}
			if (i==N)
			{
				for(i = 0; i<M; i++)
					if (ArOfConsumers[i].dwProcessId==procID)
					{
						NumOfElemInBuf--;
						number = atoi(ArOfCons[i]);
						number++;
						itoa(number,ArOfCons[i],10);
						CreateStr(message,ArOfCons,M);
						SetDlgItemText(hEdit,IDC_EDIT1,message);
						break;
					}
			}
			itoa(NumOfElemInBuf,message,10);
			SetDlgItemText(hEdit,IDC_EDIT3,message);
		break;

	case WM_DESTROY:
		StopModel();
		DelTrayIcon(hWnd);
		PostQuitMessage(0); 
		break; 

	case TrayIcon: 
		OnTray(hWnd,wParam,lParam); 
		break;
	} 
	return DefWindowProc(hWnd,Message,wParam,lParam); 
} 