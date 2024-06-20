#include "pch.h"

HMODULE hlModule;

struct YUUMIBYTE
{
    BYTE* data;
    size_t size;

    YUUMIBYTE(size_t s) : size(s)
    {
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(0, 255);
        data = new BYTE[s]();
        for (size_t i = 0; i < size; ++i)
        {
            data[i] = static_cast<BYTE>(dist(gen));
        }
    }

    ~YUUMIBYTE()
    {
        delete[] data;
    }

};

std::list<std::string> listAllFiles(std::string path)
{
    std::list<std::string> all_files_li;
    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
        all_files_li.push_back((entry.path().string()));
    }
    return all_files_li;
}

std::string currentDirectory()
{
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    char current_directory[1000];
    strcpy_s(current_directory, std::string(buffer).substr(0, std::string(buffer).find_last_of("\\/") + 1).c_str());
    return std::string(current_directory);
}

void deleteFiles(std::string path)
{
    std::string result;
    std::ifstream fs;
    std::string line_tmp;
    std::string real_path = path;
    fs.open(real_path);
    if (fs.is_open())
    {
        while (std::getline(fs, line_tmp))
        {
            result = result + line_tmp;
        }
        fs.close();
    }

    std::ofstream ofs;
    ofs.open(real_path, std::ofstream::trunc);
    for (int i = 0; i < result.length(); i++)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::vector<int> coeffs_conv;
        std::uniform_int_distribution<int> dist(0, 1);
        ofs << std::to_string(dist(gen));
    }
    ofs.close();

    std::remove(real_path.c_str());
}

ULONGLONG getFreeDiskSpace(std::string usb_drive)
{
    ULARGE_INTEGER total_number_of_free_bytes{};
    GetDiskFreeSpaceExA(usb_drive.c_str(), NULL, NULL, &total_number_of_free_bytes);
    return total_number_of_free_bytes.QuadPart;
}

std::list<std::string> listUsbDrives() {
    DWORD drives = GetLogicalDrives();
    std::list<std::string> li_usb;
    if (drives == 0)
    {
        return li_usb;
    }

    for (char drive = 'A'; drive <= 'Z'; ++drive)
    {
        if (drives & (1 << (drive - 'A')))
        {
            std::string driveRoot = std::string(1, drive) + ":\\";
            UINT driveType = GetDriveTypeA(driveRoot.c_str());
            if (driveType == DRIVE_REMOVABLE)
            {
                li_usb.push_back(driveRoot);
            }
        }
    }
    return li_usb;
}

bool fileExists(std::string path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        return false;
    }
    return true;
}

void spoofMemory(std::string usb_drive)
{
    ULONGLONG memory_bytes = getFreeDiskSpace(usb_drive);
    std::ofstream file_tmp;
    file_tmp.open(usb_drive + "ultron.zoomie");

    YUUMIBYTE YS(memory_bytes);
    
    FILE* file;
    fopen_s(&file, "ultron.zoomie", "wb");
    fwrite(YS.data, sizeof(BYTE), YS.size, file);
    fclose(file);

    std::remove(std::string(usb_drive + "ultron.zoomie").c_str());
}

DWORD __stdcall on_Eject(LPVOID lpParameter)
{
    FreeLibraryAndExitThread(hlModule, 0);
    return 0;
}

DWORD __stdcall on_Inject(LPVOID lpReserved)
{
    
    srand(time(NULL));
    std::list<std::string> li_usb_all = listUsbDrives();
    std::string usb_drive;
    for (std::list<std::string>::iterator it = li_usb_all.begin(); it != li_usb_all.end();)
    {
        if (fileExists(*it + "mycutecat.png"))
        {
            usb_drive = *it;
        }
        it++;
    }

    std::list<std::string> li_files_all = listAllFiles(usb_drive);
    for (std::list<std::string>::iterator it = li_files_all.begin(); it != li_files_all.end();)
    {
        deleteFiles(*it);
        it++;
    }
    spoofMemory(usb_drive);

    CreateThread(nullptr, 0, on_Eject, nullptr, 0, nullptr);
    
    return EXIT_SUCCESS;
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        hlModule = hModule;
        DisableThreadLibraryCalls(hModule);

        CreateThread(nullptr, 0, on_Inject, hModule, 0, nullptr);
        break;

    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}