# WinApi-TextEditor

## Описание
Данный проект является усложенным вариантом курсовой работы по предмету "Разработка ползовательских интерфейсов" и представляет из себя примитивный текстовый редактор, написанный с использованием C и WinApi

## Архитектура
 
+ Структурно приложение представляет из себя множество блоков (строк) и фрагментов, на которые эти строки разбиты. 
+ Изначально (после чтения) весь текст хранится в ```char *``` буфере. В строках содержатся фрагементы строки. В фрагментах сожержатся указатели на позицию в этом буфере.
+ Так же изначально в строках находится один фрагмент.
+ Редактирование текста осуществляется добавлением новых фрагентов, удалением или склеиванием их. Сами символы записываются в новый буфер (или продолжение исзодного), на позицию в котором потом указывают фрагменты. 
+ При сохранении текста, программа пробегает по всем строкам и фрагментам и последовательно их сохраняет в файле.
