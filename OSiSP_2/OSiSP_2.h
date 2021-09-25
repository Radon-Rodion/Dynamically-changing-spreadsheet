#pragma once

#include "resource.h"
bool editMode;

struct Table {
	HWND hwndTable;
	int nColumns;
	TCHAR** columnNames;
	int nRows;
	TCHAR*** rowsInfo;
};
typedef struct Table Table;

Table* createTable(HWND hWnd, int nColumns,TCHAR** columnNames);
void setTableData(Table* table, int nLines, TCHAR*** data);
void fitTableToScreen(Table* table, RECT* screenRect);