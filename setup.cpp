#pragma GCC diagnostic ignored "-Wwrite-strings"

#include <windows.h>
#include <synchapi.h>

int startup(char* lpApplicationName)
{
    SHELLEXECUTEINFO info;
    memset(&info, 0, sizeof(SHELLEXECUTEINFO));
    info.cbSize = sizeof(SHELLEXECUTEINFO);
    info.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
    info.lpVerb = "open";
    info.lpFile = lpApplicationName;
    info.nShow = SW_SHOWNORMAL;
     
    BOOL mybool = ShellExecuteEx(&info);
    if(mybool == TRUE) {
    	// Wait until child process exits.
		WaitForSingleObject(info.hProcess, INFINITE);
		CloseHandle(info.hProcess);
		return 0;
	} else if(mybool == FALSE){
		return -1;
	}
}

int run_setup(char *process, char *program, char *path_1, char *path_2)
{
    int strslt = startup(process);
    
    const char *prgrm1 = "packages\\AdobeAIRInstaller.exe";
    const char *prgrm2 = "packages\\ShadowBoxPresentation.air";
	
    /* if the program is Adobe AIR */
    if(strcmp(process, prgrm1) == 0) {
    	
	    // for 64-bit programs
	    unsigned long type = REG_SZ, size = 1024;
	    char res[1024] = "";
	    HKEY hKey;
	    
	    RegOpenKeyEx(HKEY_LOCAL_MACHINE, path_1, 0, KEY_READ, &hKey);
	    RegQueryValueEx(hKey, "DisplayName", NULL, &type, (LPBYTE)&res[0], &size);
	    RegCloseKey(hKey);
	    
	    // for 32-bit programs
	    unsigned long type_2 = REG_SZ, size_2 = 1024;
	    char res_2[1024] = "";
	    HKEY hKey_2;
	    
	    RegOpenKeyEx(HKEY_LOCAL_MACHINE, path_2, 0, KEY_READ, &hKey_2);
	    RegQueryValueEx(hKey_2, "DisplayName", NULL, &type_2, (LPBYTE)&res_2[0], &size_2);
	    RegCloseKey(hKey_2);
    	
    	int n = strlen(program);
	    /* installer ran successfully and program is installed */
	    if((strncmp(res, program, n) == 0 || strncmp(res_2, program, n) == 0) && strslt == 0)
	    {
	        return 0;
	    }
	    /* installer ran successfully, but program is NOT installed */
	    else if((strncmp(res, program, n) != 0 && strncmp(res_2, program, n) != 0) && strslt == 0)
	    {
	        return -1;
	    }
	    /* installer did NOT run successfully, but program is already installed */
	    else if((strncmp(res, program, n) == 0 || strncmp(res_2, program, n) == 0) && strslt == -1)
	    {
	    	return -2;
		}
		/* installer did NOT run successfully and program is NOT installed */
		else if((strncmp(res, program, n) != 0 && strncmp(res_2, program, n) != 0) && strslt == -1)
	    {
	    	return -3;
		}
	}
	/* else if the program is Shadowbox */
	else if(strcmp(process, prgrm2) == 0) {
		
		/* installer ran successfully and program is installed */
	    if(strslt == 0)
	    {
	        return 0;
	    }
	    /* installer did NOT run successfully and we consider program to be NOT installed */
		else if(strslt == -1)
	    {
	    	return -1;
		}
	}
}


/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
char szClassName[ ] = "WindowsApp";

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nFunsterStil)

{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default color as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                                         /* Extended possibilites for variation */
           szClassName,                               /* Classname */
           "Package Builder",                  		  /* Title Text */
           WS_OVERLAPPEDWINDOW,                       /* default window */
           -500,                                      /* We decide the position */
           -500,                                      /* where the window ends up on the screen */
           0,                                         /* The programs width */
           0,                                         /* and height in pixels */
           HWND_DESKTOP,                              /* The window is a child-window to desktop */
           NULL,                                      /* No menu */
           hThisInstance,                             /* Program Instance handler */
           NULL                                       /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nFunsterStil);
    
    int msgboxID = MessageBox(
        hwnd,
        "The setup wizard will install the following\nproducts on your system. Continue?\n\n1) Adobe AIR.\n2) Shadowbox.",
        "Package Builder",
        MB_ICONINFORMATION | MB_OKCANCEL | MB_APPLMODAL | MB_DEFBUTTON2 | MB_TOPMOST
    );

    switch (msgboxID)
    {
    case IDOK:
    	/* Run the message loop. It will run until GetMessage() returns 0 */
	    while (GetMessage (&messages, NULL, 0, 0))
	    {
	    	/* launch Adobe AIR installation */
	    	int adobe_air_setup_status = run_setup("packages\\AdobeAIRInstaller.exe", "Adobe AIR",
	           "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Adobe AIR",
	           "SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Adobe AIR");
	        
	        if(adobe_air_setup_status == 0)
	        {
	            MessageBox(hwnd, "Adobe AIR is installed!\nNow launching Shadowbox installation ...",
	                       "Package Builder: Installation successful!", MB_OK | MB_TOPMOST);
	    	}
			else if(adobe_air_setup_status == -1)
			{
				MessageBox(hwnd, "The installation of Adobe AIR has failed.\nAn error occured during the setup process.\nNow exiting installation ...",
	                       "Package Builder: Installation error!", MB_OK | MB_ICONERROR | MB_TOPMOST);
	            return 0;
			}
			else if(adobe_air_setup_status == -2)
			{
				MessageBox(hwnd, "Could not launch Adobe AIR installer.\nBut it seems you already have it\ninstalled on your system.\nContinuing installation normally ...",
	                       "Package Builder", MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST);
			}
			else if(adobe_air_setup_status == -3)
			{
				MessageBox(hwnd, "Could not launch Adobe AIR installer.\nAn error occured during the setup process.\nNow exiting installation ...",
	                       "Package Builder: Installation error!", MB_OK | MB_ICONERROR | MB_TOPMOST);
	            return 0;
			}
			
			/* launch Shadowbox installation */
			int shadowbox_setup_status = run_setup("packages\\ShadowBoxPresentation.air", NULL, NULL, NULL);
	        
	        if(shadowbox_setup_status == 0)
	        {
	            MessageBox(hwnd, "Installation complete!",
	                       "Package Builder", MB_OK | MB_TOPMOST);
				return 0;
	    	}
			else if(shadowbox_setup_status == -1)
			{
				MessageBox(hwnd, "Could not launch Shadowbox installer.\nAn error occured during the setup process.\nNow exiting installation ...",
	                       "Package Builder: Installation error!", MB_OK | MB_ICONERROR | MB_TOPMOST);
	            return 0;
			}
	        
	        /* Translate virtual-key messages into character messages */
	        TranslateMessage(&messages);
	        /* Send message to WindowProcedure */
	        DispatchMessage(&messages);
	    }
        break;
    case IDCANCEL:
        PostQuitMessage (0);
        break;
    default:
	    PostQuitMessage (0);
	    break;
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                   /* handle the messages */
    {
    	case WM_CREATE:
    		SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadImage(NULL, "resources\\Tonev-Windows-7-Windows-7-software.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE));
    		break;
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                       /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

