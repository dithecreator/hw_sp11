#include "Windows.h"
#include "tchar.h"
#include <fstream>
#include <string>
#include "resource.h"
using namespace std;
CRITICAL_SECTION cs; //создаем критическую секцию

void CreateFile(int n) {
	string defaultData = "sefesf";
	for (int i = 1; i <= n; i++) {
		string fileName = "data_" + to_string(i) + ".txt";
		FILE* file;
		errno_t err = fopen_s(&file, fileName.c_str(), "w");
		if (file == nullptr) {
			MessageBox(0, TEXT("Ошибка создания файла"), TEXT("Ошибка"), MB_OK);
		}
		else {
			fwrite(defaultData.c_str(), 1, defaultData.length(), file);
			fclose(file);
		}
	}

}

DWORD WINAPI WriteToFiles(LPVOID lp) {
	EnterCriticalSection(&cs);//захватываем критическую секцию
	CreateFile(5);
	LeaveCriticalSection(&cs);//оставляем критическую секцию
	return 1;
}



DWORD WINAPI ReadFromFiles(LPVOID lp) { 
	EnterCriticalSection(&cs); //захватываем критическую секцию
	FILE* result;
	errno_t err = fopen_s(&result, "result.txt", "a");

	for (int i = 1; i <= 5; i++) {
		FILE* file;
		string fileName = "data_" + to_string(i) + ".txt";
		err = fopen_s(&file, fileName.c_str(), "r");

		if (file != nullptr) {
	

		
	
			char buffer[4096]; // Буфер для копирования данных

			// Чтение и запись содержимого файла в 
			size_t bytesRead; // это переменная, предназначенная для хранения количества байтов, считанных или записанных функцией ввода/вывода
			while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) { // пока байты на чтение есть
				fwrite(buffer, 1, bytesRead, result); //записываем
			}

			fclose(file);
		}
	}
	fclose(result);
	LeaveCriticalSection(&cs); //оставляем критическую секцию
	return 1;
}




INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{


	switch (uMsg)
	{
	case WM_INITDIALOG: {
		InitializeCriticalSection(&cs); //инициализируем критическую секцию
		HANDLE hThread = CreateThread(nullptr, 0, WriteToFiles, nullptr, 0, nullptr); //создаем первый поток (создание файлов)
		HANDLE hThread1 = CreateThread(nullptr, 0, ReadFromFiles, nullptr, 0, nullptr);//создаем второй поток(чтение из файлов и запись в один)
		if (hThread != nullptr) {
			WaitForSingleObject(hThread, INFINITE); //ждем, пока поток выполнится
			CloseHandle(hThread);//освобождаем дескриптор
		}

		if (hThread1 != nullptr) {
			WaitForSingleObject(hThread1, INFINITE); //ждем, пока поток выполнится
			CloseHandle(hThread1);//освобождаем дескриптор
		}


	}
		break;

	case WM_COMMAND:


		break;





	case WM_CLOSE:
		DeleteCriticalSection(&cs);//удаляем критическую секцию
		EndDialog(hwnd, 0);
		return TRUE;

	}
	return FALSE;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DLGPROC(DlgProc));



}