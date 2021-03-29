#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <windows.h>
#include <stdio.h>

#include "struct.h"
#include "write.h"
#include "menu.h"
#include "caret.h"
#include "save.h"

#include "debug.h"

// start parameters
#define FILENAME "very_small.txt"
#define MODE DEFAULT

// 1 - caret
// 7 - line
#define CARET_SIZE_COEFF 7

#define CARET_LENGTH 0.9

// GetStockObject(WHITE_BRUSH)
// COLOR_BACKGROUND
#define MY_BACK_COLOR GetStockObject(WHITE_BRUSH)

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */

int WINAPI WinMain (HINSTANCE hThisInstance,  // ��������� ����� ���������
                     HINSTANCE hPrevInstance, // ��������� ��������� ���������, ������� ��� �������
                     LPSTR lpszArgument,  // ��������� ��������� ������
                     int nCmdShow)  // ����� ������ ��������� ���� (SW_SHOWNORMAL)
{
    HWND hwnd;          // ���������� �������� ���� ���������
    MSG messages;       // ��������� ��� �������� ���������
    WNDCLASSEX wincl;   // ����� ����

    char hMenu[]="MainMenu";

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = "TextEdit";
    wincl.lpfnWndProc = WindowProcedure;    // ������� ����
    wincl.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;    // ����� ���� // TODO ������� ������
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* ���������� ������ � ��������� ���� �� ��������� */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);     // ����������� ������
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);   // ����������� ������
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);       // ����������� ������
    wincl.lpszMenuName = hMenu;                // ����
    wincl.cbClsExtra = 0;                      // ��� �������������� ������ ������
    wincl.cbWndExtra = 0;                      // ��� �������������� ������ ����
    /* ���������� ���� �� ��������� Windows � �������� ���� ����. */
    wincl.hbrBackground = (HBRUSH) MY_BACK_COLOR;

    /* ����������� ������ ���� */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (         // ��� ������ ����
               0,                   // ������������� �����������
               "TextEdit",         // ��� ������
               ("TextEdit"),    // ��������� ����
               WS_OVERLAPPEDWINDOW|WS_VSCROLL|WS_HSCROLL, // ����� ����
               CW_USEDEFAULT,       // x
               CW_USEDEFAULT,       // y ������� ����
               544,                 // Width
               375,                 // Height
               HWND_DESKTOP,        // ���������� ������������� ����
               NULL,                // ��� ����
               hThisInstance,       // ���������� ����������
               lpszArgument         // �������������� ����������
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

void ShowDialog(HWND hwnd,TCHAR* sfile)
{

    OPENFILENAME ofn;

    ZeroMemory(&ofn, sizeof(ofn));
    ZeroMemory(sfile, sizeof(TCHAR)*MAX_PATH);
    ofn.lStructSize         = sizeof(ofn);
    ofn.hwndOwner           = NULL;
    ofn.lpstrFile           = sfile;
    ofn.nMaxFile            = MAX_PATH;
    ofn.lpstrFilter         = NULL;
    ofn.nFilterIndex        = 1;
    ofn.lpstrTitle          = TEXT("Please Select a File");
    ofn.lpstrInitialDir     = NULL;
    ofn.lpstrCustomFilter   = NULL;
    ofn.nMaxCustFilter      = 0;
    ofn.lpstrFileTitle      = NULL;
    ofn.nMaxFileTitle       = 0;
    ofn.nFileOffset         = 0;
    ofn.nFileExtension      = 0;
    ofn.lpstrDefExt         = NULL;
    ofn.lCustData           = 0;
    ofn.lpfnHook            = 0;
    ofn.lpTemplateName      = 0;
    ofn.Flags               = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

    GetOpenFileName(&ofn);
}

/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    static TEXTMETRIC tm;
    static HFONT hFont;
    static HMENU hMenu;
    static int mode;
    static char file_name[MAX_PATH] = FILENAME;
    static data_t data = {NULL, 0, NULL, 0, 0}; // TODO ��������� ����� ����������
    static caret_t caret;
    static pos_t pos;
    static RECT rect;
    static rsize_t rSize;

    switch (message)                  /* handle the messages */
    {
        case WM_CREATE:
            // ��������� ���������� ����, ���������� � �������� �����
            hMenu = GetMenu(hwnd);

            // ������� ���������� ����� � ��������� ����������������
            hFont = CreateFont(30, // ������ ������ � ���������� �������� //TODO set 20
                            18,  // ������ ������ � ���������� ��������  //TODO set 12
                            0,  // ���� ��������� ������ � ���� ����� ������� ������ ������ � ������������ (������� ���� �������)
                            0,  // ������ �������� (������� ���� �������)
                            FW_DONTCARE, // ���� �� ������, ������������ �������� ������ � �������� �� 0 �� 1000 (�� ���������)
                            FALSE, // ��������� �������� �������� ������
                            FALSE, // ��������� �������� �������� �������������
                            FALSE, // ��������� �������� �������� ������������� �����
                            DEFAULT_CHARSET, //���� �� ������, �������� ��������� ������ (�� ���������)
                            OUT_OUTLINE_PRECIS, // ���� �� �����, ������������ ������������ �������������� ������ � ���������� � �������
                            CLIP_DEFAULT_PRECIS, // ���� �� ������, ������������ ������ ����������� ����������� ����� ��� �� ������ �� ������� ������� ������������� ������
                            DEFAULT_QUALITY, // ���� �� ������, ������������ �������� ��������������� ������
                            VARIABLE_PITCH,TEXT("Lucida Console")); // ��� ������

            hdc = GetDC(hwnd);

            SelectObject(hdc,hFont); // ������������� �����

            // ��������� �������� ����� ��������� �������� ���������� ������
            GetTextMetrics(hdc,&tm);

            // ��������� ���������� ������� ������� ����
            GetClientRect(hwnd,&rect);

            // ����� �������
            rSize.width = rect.right / tm.tmAveCharWidth;
            rSize.height = rect.bottom / tm.tmHeight;

            // ������� ����� DEFAULT
            mode = MODE;
            CheckMenuItem(hMenu,MODE,MF_CHECKED);

            // ������� �������
            CreateCaret(hwnd, NULL, tm.tmAveCharWidth / CARET_SIZE_COEFF, tm.tmHeight * CARET_LENGTH);


            FileRead(hwnd, FILENAME, &data); // TODO to delete
            FirstCountDefault(hwnd, &data);

            NewCaret(&data, &caret, tm);

            // ������� � ������ ����� �� ���������
            ZeroMemory(&pos, sizeof(pos));

            // ������������� �������� ����������
            SetScrollRange(hwnd, SB_VERT, 0, SCROLL_CONST, TRUE);
            // ���������� �������
            SetCaretPos(0, 0);
            ShowCaret(hwnd);


            return 0;

        case WM_SIZE: // ������������ ���� ����� ����, ��� ��������� ��� ������
        {
            rSize.width = LOWORD(lParam) / tm.tmAveCharWidth; // ������� ����������
            rSize.height = HIWORD(lParam) / tm.tmHeight; // ������� ������� �������

            SetScrollRange(hwnd, SB_HORZ, 0, fmax(1, data.longestSize - rSize.width), TRUE);

            // InvalidateRect(hwnd,NULL,TRUE);
            return 0;
        }
        case WM_VSCROLL: // ������������ � ����, ����� ������� ��������� ���������� � ����������� ������������ ������ ��������� ����
        {
            switch (LOWORD(wParam))
            {
                case SB_TOP :
                    pos.v = -5;
                break;

                case SB_BOTTOM :
                    pos.v = data.linesSize + 5;
                break;
                case SB_LINEUP: // ��������� �� ���� ������ �����
                    pos.v += -1;
                    break;
                case SB_LINEDOWN: // ��������� �� ���� ������ ����
                    pos.v += 1;
                    break;
                case SB_PAGEUP: // ��������� �� ���� �������� �����
                    pos.v += - rSize.height;
                    break;
                case SB_PAGEDOWN: // ��������� �� ���� �������� ����
                    pos.v += rSize.height;
                    break;
                case SB_THUMBTRACK: // ��������� �� ���������� �������. ������� ������� ������������ ������� ������
                    pos.v = ScrollToPos(HIWORD(wParam), data, pos, rSize.height);
                    break;
                case SB_ENDSCROLL:
                    break;
                default:
                    pos.v = pos.v;
                    break;
            }
            // ���������� ������� � ������ � ����� �����

            pos.v = fmax(0, fmin(data.linesSize - rSize.height, pos.v));
            SetScrollPos(hwnd, SB_VERT, PosToScroll(pos.v, data, pos, rSize.height), TRUE);
            SetCaretPos((caret.sym - pos.h) * caret.w, (caret.line->number - pos.v) * caret.h);

            InvalidateRect(hwnd,NULL,TRUE);

            return 0;
        }
        case WM_HSCROLL: // ������������ � ����, ����� ������� ��������� ���������� � ����������� �������������� ������ ��������� ����
        {
            switch (LOWORD(wParam))
            {
                case SB_LINEUP:
                    pos.h -= 1;
                    break;
                case SB_LINEDOWN:
                    pos.h += 1;
                    break;
                case SB_THUMBTRACK:
                    pos.h = fmax(0, HIWORD(wParam));
                    break;
                default:
                    pos.h += 0;
                    break;
            }
            pos.h = fmax(0,fmin(pos.h, data.longestSize - pos.h + 1));
            SetScrollPos(hwnd, SB_HORZ, pos.h, TRUE);

            SetCaretPos((caret.sym - pos.h) * caret.w, (caret.line->number - pos.v) * caret.h);
            // ������ �� ���������
            InvalidateRect(hwnd,NULL,TRUE);
            return 0;
        }

        case WM_COMMAND: // ������������ �����, ����� ������������ �������� ��������� ����� �� ����
        {
            switch(LOWORD(wParam))
            {
                case ID_FILE_OPEN:
                {
                    ZeroMemory(&file_name, sizeof(file_name));
                    ShowDialog(hwnd,file_name);
                    freeData(&data);
                    ZeroMemory(&pos, sizeof(pos_t));

                    //���� ���� ������� ������ � ������ ����������
                    if ((FileRead(hwnd, file_name, &data) == 1)){
                        DestroyCaret();
                        CreateCaret(hwnd, NULL, tm.tmAveCharWidth / CARET_SIZE_COEFF, tm.tmHeight * CARET_LENGTH);
                        pos.abs = data.text;

                        if (mode==DEFAULT) {
                            FirstCountDefault(hwnd, &data);
                            SetScrollRange(hwnd, SB_VERT, 0, SCROLL_CONST, FALSE);
                            SetScrollRange(hwnd, SB_HORZ, 0, fmax(1, data.longestSize - rSize.width), TRUE);

                            SetScrollPos(hwnd, SB_VERT, PosToScroll(pos.v, data, pos, rSize.height), TRUE);
                            SetScrollPos(hwnd, SB_HORZ, pos.h, TRUE);
                            InvalidateRect(hwnd,NULL,TRUE);
                        }

                        NewCaret(&data, &caret, tm);
                        SetCaretPos(0, 0);
                        ShowCaret(hwnd);
                    }
                    break;

                }
                case SAVE:
                {
                    SaveFile(hwnd, &data, &file_name);
                    break;
                }
                case IDM_EXIT:
                {
                    freeData(&data);
                    SendMessage(hwnd, WM_DESTROY, 0, 0L);
                    break;
                }
                default:
                    break;
            }
            return 0;
        }

        case WM_KEYDOWN: // �������� � ������� ���� � ������� ���������� �����, ����� ������ ����������� �������
        {
            switch (wParam)
            {
            case VK_UP:
                CaretUp(&caret, &data, &pos);
                //SendMessage(hwnd, WM_VSCROLL, MAKELONG(SB_LINEUP, 0), 0L);
                //SendMessage(hwnd, WM_VSCROLL, MAKELONG(SB_ENDSCROLL, 0), 0L);
                break;
            case VK_DOWN:
                CaretDown(&caret,  &data, &pos);
                //SendMessage(hwnd, WM_VSCROLL, MAKELONG(SB_LINEDOWN, 0), 0L);
                //SendMessage(hwnd, WM_VSCROLL, MAKELONG(SB_ENDSCROLL, 0), 0L);
                break;

            case VK_RIGHT:
                CaretRight(&caret, &data, &pos);
                //SendMessage(hwnd, WM_HSCROLL, MAKELONG(SB_LINERIGHT, 0), 0L);
                break;
            case VK_LEFT:
                CaretLeft(&caret, &data, &pos);
                //SendMessage(hwnd, WM_HSCROLL, MAKELONG(SB_LINELEFT, 0), 0L);
                break;
            case VK_PRIOR:
                // SendMessage(hwnd, WM_VSCROLL, MAKELONG(SB_PAGEUP, 0), 0L);
                CaretPageUp(&caret, &data, &pos, &rSize);
                break;
            case VK_NEXT:
                //SendMessage(hwnd, WM_VSCROLL, MAKELONG(SB_PAGEDOWN, 0), 0L);
                CaretPageDown(&caret, &data, &pos, &rSize);
                break;
            case VK_HOME:
                // SendMessage(hwnd, WM_VSCROLL, MAKELONG(SB_TOP, 0), 0L);
                CaretBegin(&caret, &data);
                break;
            case VK_END:
                //SendMessage(hwnd, WM_VSCROLL, MAKELONG(SB_BOTTOM, 0), 0L);
                CaretEnd(&caret, &data);
                break;
            default:
                break;
            }

            CaretDraw(&caret, &data, &pos, &tm, &rSize);
            // printf("%i ", pos.v);
            SetCaretPos((caret.sym - pos.h) * caret.w, (caret.line->number - pos.v) * caret.h);

            SetScrollPos(hwnd, SB_VERT, PosToScroll(pos.v, data, pos, rSize.height), TRUE);
            SetScrollPos(hwnd, SB_HORZ, pos.h, TRUE);
            //pos.abs = data.starts_of_lines[pos.v] - data.text;

            //printf("%i %i\n", caret.line->size, caret.sym);

            //SendMessage(hwnd, WM_VSCROLL, NULL, 0L);

            InvalidateRect(hwnd,NULL,TRUE);
            return 0;
        }

        case WM_CHAR:
            switch(wParam) {
                case '\b' :
                    DeleteSym(&data, &caret, &pos, mode);
                    CaretDraw(&caret, &data, &pos, &tm, &rSize);
                    SetCaretPos((caret.sym - pos.h) * caret.w, (caret.line->number - pos.v) * caret.h);

                    SetScrollPos(hwnd, SB_VERT, PosToScroll(pos.v, data, pos, rSize.height), TRUE);
                    SetScrollPos(hwnd, SB_HORZ, pos.h, TRUE);

                    //PrintBlocks(caret.line);
                    //PrintWindow(&data, &pos, rSize.height);

                    InvalidateRect(hwnd,NULL,TRUE);

                    break;


                case '\r' :
                    AddEnter(&data, &caret, &pos, mode, (char)wParam);
                    caret.sym = 0;
                    CaretDraw(&caret, &data, &pos, &tm, &rSize);
                    SetCaretPos((caret.sym - pos.h) * caret.w, (caret.line->number - pos.v) * caret.h);

                    SetScrollPos(hwnd, SB_VERT, PosToScroll(pos.v, data, pos, rSize.height), TRUE);
                    SetScrollPos(hwnd, SB_HORZ, pos.h, TRUE);

                    //PrintBlocks(caret.line);
                    //PrintWindow(&data, &pos, rSize.height);

                    InvalidateRect(hwnd,NULL,TRUE);

                    break;

                default:
                    AddSym(&data, &caret, &pos, mode, (char)wParam);
                    RecountAdd(&data, &caret, &pos, mode);
                    CaretDraw(&caret, &data, &pos, &tm, &rSize);
                    SetCaretPos((caret.sym - pos.h) * caret.w, (caret.line->number - pos.v) * caret.h);

                    SetScrollPos(hwnd, SB_VERT, PosToScroll(pos.v, data, pos, rSize.height), TRUE);
                    SetScrollPos(hwnd, SB_HORZ, pos.h, TRUE);

                    //PrintBlocks(caret.line);
                    //PrintWindow(&data, &pos, rSize.height);

                    InvalidateRect(hwnd,NULL,TRUE);

                    break;
            }
            // PrintWindow(&data, &pos, rSize.height);
            return 0;

        case WM_PAINT:
            ZeroMemory(&ps,sizeof(PAINTSTRUCT));
            hdc = BeginPaint(hwnd, &ps);
            MyTextOut(hdc, &data, &pos, &rSize, &tm);
            EndPaint(hwnd, &ps);
            return 0;



        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            freeData(&data);
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
