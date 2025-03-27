#include <windows.h>
#include <iostream>
#include <string>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

void PrintError(const string& message) {
    cerr << message << " エラーコード: " << GetLastError() << endl;
}

class HandleWrapper {
public:
    HandleWrapper(HANDLE handle) : handle_(handle) {}
    ~HandleWrapper() {
        if (handle_ != NULL && handle_ != INVALID_HANDLE_VALUE) {
            CloseHandle(handle_);
        }
    }
    HANDLE get() const { return handle_; }
private:
    HANDLE handle_;
};

bool InjectDLL(DWORD pid, const string& dllPath) {
    HandleWrapper hProcess(OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid));
    if (hProcess.get() == NULL) {
        PrintError("プロセスが無効です。");
        return false;
    }

    if (dllPath.empty() || !fs::exists(dllPath)) {
        PrintError("DLLのパスが無効です。");
        return false;
    }

    LPVOID remoteMemory = VirtualAllocEx(hProcess.get(), NULL, dllPath.length() + 1, MEM_COMMIT, PAGE_READWRITE);
    if (remoteMemory == NULL) {
        PrintError("メモリの割り当てに失敗しました。");
        return false;
    }

    if (!WriteProcessMemory(hProcess.get(), remoteMemory, dllPath.c_str(), dllPath.length() + 1, NULL)) {
        PrintError("プロセスへの書き込みに失敗しました。");
        VirtualFreeEx(hProcess.get(), remoteMemory, 0, MEM_RELEASE);
        return false;
    }

    HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    if (hKernel32 == NULL) {
        PrintError("kernel32.dllのハンドル取得に失敗しました。");
        VirtualFreeEx(hProcess.get(), remoteMemory, 0, MEM_RELEASE);
        return false;
    }

    LPVOID loadLibraryAddr = (LPVOID)GetProcAddress(hKernel32, "LoadLibraryA");
    if (loadLibraryAddr == NULL) {
        PrintError("LoadLibraryAのアドレス取得に失敗しました。");
        VirtualFreeEx(hProcess.get(), remoteMemory, 0, MEM_RELEASE);
        return false;
    }

    HandleWrapper hThread(CreateRemoteThread(hProcess.get(), NULL, 0, (LPTHREAD_START_ROUTINE)loadLibraryAddr, remoteMemory, 0, NULL));
    if (hThread.get() == NULL) {
        PrintError("リモートスレッドの作成に失敗しました。");
        VirtualFreeEx(hProcess.get(), remoteMemory, 0, MEM_RELEASE);
        return false;
    }

    WaitForSingleObject(hThread.get(), INFINITE);
    VirtualFreeEx(hProcess.get(), remoteMemory, 0, MEM_RELEASE);

    return true;
}

int main() {
    DWORD pid;
    cout << "対象のプロセスIDを入力してください。" << endl;
    cin >> pid;
    cout << "入力されたプロセスID: " << pid << endl;

    string dllPath;
    cout << "DLLのパスを入力してください。" << endl;
    cin >> dllPath;
    cout << "指定されたDLLのパス: " << dllPath << endl;

    if (InjectDLL(pid, dllPath)) {
        cout << "DLLが正常に注入されました。" << endl;
    }
    else {
        cerr << "DLLの注入に失敗しました。" << endl;
    }

    return 0;
}
