#include <windows.h>
#include <stdio.h>
#include "struct.h"

int FileRead(HWND hwnd, const char * filename, data_t * data) {
    int i;
    int realFileSize;
    int length_of_file;
    char * pos;
    FILE * fp = fopen(filename,"rb");
    if (fp == NULL){
        MessageBox(hwnd,
                   "File could not be opened",
                   "File error",
                   MB_OK); // ������� ��
        return 0;
    }

    fseek(fp,0,SEEK_END); // ��������� �� ������ ��������, ���-�� ���� ��������, �������, ������������ ������� ��������
    length_of_file = ftell(fp); // ���������� ������� ��������� ������
    rewind(fp); // ���������� � ������

    // ������������� ������
    realFileSize = length_of_file + 2000;
    data->text = (char*)malloc((realFileSize) * sizeof(char));       // ������ ��������
                                                                // ����� ����� ����� ����� ������� ��, ������� � ��������
    // ���� ��� �����, ������ ������, ���� ���������
    if (data->text == NULL){
        MessageBox(hwnd,
                   "Not enough memory is available",
                   "Memory error",
                   MB_OK);
        fclose(fp);
        free(data->text);
        ZeroMemory(data,sizeof(data)); // ��������� ���� ������(��������� �� ���� ������, ������ ����� ������)
        return 0;
    }

    fread(data->text,sizeof(char),length_of_file + 1,fp); // ������� ������� � �����
    data->text[length_of_file] = '\0'; // ����-�������������� ������ �������� (����� ���� ����������)
    fclose(fp); //�������� �����

    data->textSize = length_of_file;
    data->buff = data->text + length_of_file + 1;
    data->buffPos = data->buff;
    data->buffSize = realFileSize - data->textSize;

    return 1;
}
