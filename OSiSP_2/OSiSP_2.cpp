// OSiSP_2.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "OSiSP_2.h"

#define MAX_LOADSTRING 100
#define IDM_CODE_SAMPLES 101

#define N_COLUMNS 3
#define WORD_LENGTH 130
#define N_ROWS 3

TCHAR colNames[N_COLUMNS][WORD_LENGTH] = { L"ID", L"Name", L"Quotation"};
TCHAR data[N_ROWS][N_COLUMNS][WORD_LENGTH] = { {L"0", L"Dima Grach", L"Ich sage dich thus Ich wirde sage dich thus Ich wirde mache es jetzt. Unt zie hast sagen thus zie wirst mich antworten nicht"},
    {L"1", L"Vova Putin", L"I say you that I will say you that I will do it now. And she has said that she will not answer me."},
    {L"2", L"Sergey Sobyanin", L"Я говорю тебе что я буду говорить тебе что я сделаю это. А она сказала, что не ответит мне"}
};

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

TCHAR** memoryReplace(TCHAR arr[N_COLUMNS][WORD_LENGTH]);
TCHAR*** memoryReplace(int length, TCHAR arr[][N_COLUMNS][WORD_LENGTH]);

void listViewProcess(int notification);
void drawTableData(Table* table, LPNMLVCUSTOMDRAW  lplvcd);
void fitTableColumnsByWidth(Table* table, int width);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_OSISP2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OSISP2));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OSISP2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_OSISP2);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
       0, 0, 1600, 850, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

Table* table;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        editMode = false;
        table = createTable(hWnd, N_COLUMNS, memoryReplace(colNames));
        setTableData(table, N_ROWS, memoryReplace(N_ROWS, data));
        break;
    case WM_NOTIFY:
        if(editMode) listViewProcess(((LPNMHDR)lParam)->code);
        else {
            LPNMLISTVIEW  pnm = (LPNMLISTVIEW)lParam;

            switch (pnm->hdr.code)
            {
            case NM_CUSTOMDRAW:
                LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)lParam;

                switch (lplvcd->nmcd.dwDrawStage)
                {
                case CDDS_PREPAINT:
                    return CDRF_NOTIFYITEMDRAW;

                case CDDS_ITEMPREPAINT:
                    return CDRF_NOTIFYSUBITEMDRAW;

                case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
                    if(!lplvcd->iSubItem && !lplvcd->nmcd.lItemlParam)
                        drawTableData(table, lplvcd);
                    return CDRF_SKIPDEFAULT;
                }
            }
        }
        break;
    case WM_PAINT:
        {
            RECT rc;
            GetClientRect(hWnd, &rc);
            fitTableToScreen(table, &rc);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

//Обработчик сообщений от таблицы
void listViewProcess(int notification) {
    switch (notification) {
        case LVN_ENDLABELEDIT:
            MessageBox(0, L"Поздравляю. Что делать будешь23456789p?", L"Custom", MB_OK);
            break;
        case LVN_BEGINLABELEDIT:
            MessageBox(0, L"Поздравляю. Что делать будешь?", L"Custom", MB_OK);
            break;
    }
}

Table* createTable(HWND hWnd, int nColumns,TCHAR** columnNames) {
    Table* table = new Table;

    /*Растянeм таблицу по ширине окна*/
    RECT rc;
    GetClientRect(hWnd, &rc);

    /*Создаем таблицу*/
    table->hwndTable = CreateWindowEx(0L, WC_LISTVIEW, L"",
        WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT |
        LVS_EDITLABELS,
        0, 0, rc.right, rc.bottom,
        hWnd, NULL, hInst, NULL);

    /*Заполняем структуру*/
    table->nColumns = nColumns;
    table->columnNames = columnNames;
    table->nRows = 0;

    /*Добавляем столбцы*/
    LV_COLUMN lvc;

    memset(&lvc, 0, sizeof(lvc));
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvc.fmt = LVCFMT_LEFT;
    lvc.cx = 40;

    for (int i = 0; i < table->nColumns; i++) {
        lvc.iSubItem = i;
        lvc.pszText = (LPWSTR)table->columnNames[i];
        ListView_InsertColumn(table->hwndTable, i, &lvc);
    }

    //ListView_SetExtendedListViewStyle(table->hwndTable, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    fitTableToScreen(table, &rc);
    return table;
}

void setTableData(Table* table, int nLines, TCHAR*** data) {
    table->nRows = nLines;
    table->rowsInfo = data;

    LVITEM lvi;//Строки

             // Вставляем строки
    memset(&lvi, 0, sizeof(lvi));

    lvi.mask = LVIF_TEXT | LVIF_TEXT;

    for (int i = 0; i < table->nRows; i++)
    {
        lvi.iItem = i;
        ListView_InsertItem(table->hwndTable, &lvi);
    }

    for (int i = 0; i < table->nRows; i++) {
        for (int j = 0; j < table->nColumns; j++)
            ListView_SetItemText(table->hwndTable, i, j, table->rowsInfo[i][j]);
    }
}

void drawTableData(Table* table, LPNMLVCUSTOMDRAW  lplvcd) {
    RECT myrect; int top = 20;
    HBRUSH br = CreateSolidBrush(RGB(0, 255, 0));
    SelectObject(lplvcd->nmcd.hdc, br);
    for (int i = 0; i < N_ROWS; i++) { // i instead of LPNMLVCUSTOMDRAW lplvcd->nmcd.lItemlParam
        int bottom = top;
        for (int j = 0; j < N_COLUMNS; j++) {//finding neccesary height
            ListView_GetSubItemRect(table->hwndTable, i, j, LVIR_BOUNDS, &myrect);
            myrect.top = top;
            DrawText(lplvcd->nmcd.hdc, table->rowsInfo[i][j], WORD_LENGTH, &myrect, DT_CALCRECT | DT_WORDBREAK);
            if (myrect.bottom > bottom)
                bottom = myrect.bottom;
        }
        for (int j = 0; j < N_COLUMNS; j++) {//drawing cells
            ListView_GetSubItemRect(table->hwndTable, i, j, LVIR_BOUNDS, &myrect);
            myrect.top = top;
            myrect.bottom = bottom;
            
            Rectangle(lplvcd->nmcd.hdc, myrect.left, myrect.top, myrect.right, myrect.bottom);
            DrawText(lplvcd->nmcd.hdc, table->rowsInfo[i][j], WORD_LENGTH, &myrect, DT_WORDBREAK);
            //MessageBox(0, table->rowsInfo[lplvcd->nmcd.lItemlParam][lplvcd->iSubItem], L"Custom", MB_YESNO | MB_ICONASTERISK | MB_DEFBUTTON2);
        }
        top = bottom;
    }
    DeleteObject(br);
}

//Fitting table to screen

void fitTableToScreen(Table* table, RECT* screenRect) {
    fitTableColumnsByWidth(table, screenRect->right - screenRect->left);
}

void fitTableColumnsByWidth(Table* table, int width) {
    for (int i = 0; i < table->nColumns; i++) {
        ListView_SetColumnWidth(table->hwndTable, i, width / table->nColumns);
    }
}

//Replacement of array in memory
TCHAR** memoryReplace(TCHAR arr[N_COLUMNS][WORD_LENGTH]) {
    TCHAR** res = new TCHAR * [N_COLUMNS];
    for (int i = 0; i < N_COLUMNS; i++) {
        res[i] = arr[i];
    }
    return res;
}

TCHAR*** memoryReplace(int length, TCHAR arr[][N_COLUMNS][WORD_LENGTH]) {
    TCHAR*** res = new TCHAR * *[length];
    for (int j = 0; j < length; j++) {
        res[j] = memoryReplace(arr[j]);
    }
    return res;
}