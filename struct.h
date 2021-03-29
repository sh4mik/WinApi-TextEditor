#ifndef STRUCT_H_INCLUDED
#define STRUCT_H_INCLUDED

#include <windows.h>

#define SCROLL_CONST 1000

#define OLD_DATA 0
#define NEW_DATA 1

typedef struct block {
    char * start;   // ��������� �� ������ � ������� ������
    int size;       // ������ �����
    int type;       // ��� ������(������ ��� �����) - �������� ���� �� �����
    struct block * next;
    struct block * prev;
} block_t;

typedef struct line {
    int number;     // ����� � ������ �����
    char * start;   // ��������� �� ������ ������
    int size;
    int isGlobal;   // �������� �� ������ ����� � ���������������� (� ������ LAYOUT)
    struct line * next;
    struct line * prev;
    block_t * first;    // ������ ���� � ������
} line_t;

typedef struct data {
    char * text;
    int textSize;  // ������ ������ ��� �������� ������
    char * buff;    // ��������� �� ������ ������ ��� �������������� ������
    char * buffPos; // ������� ������� ��� ������ � �����
    int buffSize;  // ������ ������ ��� ��� ������
    int linesSize;  // ���������� �����
    int longestSize;  // ������ ����� ������� ������
    line_t * firstLine;  // ������ ������
} data_t;

typedef struct pos {
    int v; //������� �� ���������
    int h; // �� �����������

    char * abs; // ����� ������� (�������� ��������)
} pos_t;

// ������ ������ � ��������� ����������� ��������
typedef struct size_s {
    int width;
    int height;
} rsize_t;


void FirstCountDefault(HWND hwnd, data_t * data);

void RecountDefault(HWND hwnd, data_t * data);

void FindNearestLines(data_t * data, pos_t * pos);

int PosToScroll(int val, data_t data, pos_t pos, int lines);

int ScrollToPos(int val, data_t data, pos_t pos, int lines);

void freeData(data_t * data);

#endif // STRUCT_HINCLUDED
