//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   native_stuff.cpp
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include <cstdio>

#include <shlobj.h>

#define _WIN32_DCOM
#include <AtlBase.h>
#include <comdef.h>
#include <Wbemidl.h>
# pragma comment(lib, "wbemuuid.lib")

#include "license.h"
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// License stuff
DWORD rdtsc_low;

akey_check* checkers;
unsigned int checkers_count;

char rkey[RKEY_SIZE];
char akey[AKEY_SIZE];

void create_license(void*)
{
    delete[] checkers;
    checkers_count = 16; // need random !
    checkers = new akey_check[checkers_count];
}

//Gimme some template magic !
template <int I>
class SystemInfoReader
{
    int r;
public:

   SystemInfoReader():r(I),locator(NULL), services(NULL)
   {
      HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
      if(FAILED(hr)) throw hr;

      hr = CoInitializeSecurity(
         NULL,
         -1,                          // COM authentication
         NULL,                        // Authentication services
         NULL,                        // Reserved
         RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
         RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
         NULL,                        // Authentication info
         EOAC_NONE,                   // Additional capabilities 
         NULL                         // Reserved
         );

      if(FAILED(hr))
      {
         clean();
         throw hr;
      }

      hr = CoCreateInstance( CLSID_WbemLocator, 0,
            CLSCTX_INPROC_SERVER, 
            IID_IWbemLocator, (LPVOID *) &locator);

      if(FAILED(hr))
      {
         clean();
         throw hr;
      }

      hr = locator->ConnectServer(
      _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
      NULL,                    // User name. NULL = current user
      NULL,                    // User password. NULL = current
      0,                       // Locale. NULL indicates current
      NULL,                    // Security flags.
      0,                       // Authority (e.g. Kerberos)
      0,                       // Context object 
      &services                // pointer to IWbemServices proxy
      );

      if(FAILED(hr))
      {
         clean();
         throw hr;
      }

      hr = CoSetProxyBlanket(
         services,                    // Indicates the proxy to set
         RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
         RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
         NULL,                        // Server principal name 
         RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
         RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
         NULL,                        // client identity
         EOAC_NONE                    // proxy capabilities 
         );

      if(FAILED(hr))
      {
         clean();
         throw hr;
      }
   }

    inline ~SystemInfoReader(){ clean(); }

    void set(char buffer[128])
    {
        IEnumWbemClassObject* enumerator;
   
        // For example, get the name of the operating system
        services->ExecQuery(
        bstr_t("WQL"), bstr_t("SELECT * FROM Win32_BIOS" ),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL, &enumerator);

        IWbemClassObject* obj;
        ULONG uReturn = 0;

        USES_CONVERSION;

        while (enumerator)
        {
            enumerator->Next(WBEM_INFINITE, 1, &obj, &uReturn);

            if(0 == uReturn) break;

            VARIANT prop;
            VariantInit(&prop);
            // Get the value of the Name property
            obj->Get(L"SerialNumber", 0, &prop, 0, 0);
            if(prop.vt != VT_NULL)
            {
                strcpy_s(buffer, 128, OLE2A(prop.bstrVal));
            }
            VariantClear(&prop);
            obj->Release();
        }

        enumerator->Release();
    }

private:
    void clean()
    {
        if(services) services->Release();
        if(locator) locator->Release();
        CoUninitialize();
    }

    IWbemLocator* locator;
    IWbemServices* services;
};

void License::mbid(char buf[128])
{
    SystemInfoReader<1> reader;
    reader.set(buf);
}

void License::osid(char buf[35])
{
    DWORD size = 35;
    HKEY key;
    RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_QUERY_VALUE, &key);
    RegQueryValueEx(key, "ProductId", NULL, NULL, (LPBYTE)buf, &size);
    RegCloseKey(key);
}

void License::hdid(char buf[4])
{
    DWORD max;
    char g[128];
    GetVolumeInformation("C:\\", g, 128, (LPDWORD)buf, &max, &max, g, 128);
}

void License::time(char buf[8])
{
    GetSystemTimeAsFileTime((LPFILETIME)buf);
}

void License::print_info()
{
    char mbid[128]={'\0'};
    License::mbid(mbid);
    printf("mb id: %s\n", mbid);

    char osid[35]={'\0'};
    License::osid(osid);
    printf("os id: %s\n", osid);


    int hdid = 0;
    License::hdid((char*)&hdid);
    printf("hd id: %i\n", hdid);

    __int64 time = -1;
    License::time((char*)&time);
    
//    time &= 0xFFFFFFFF00000000;
    printf("local time: %I64X\n", time);



    AKey akey("FFFFFFFF0DD0110EE0");
    
    AKey b = akey;
    
    printf("akey: %I64X\n", akey);


    printf("%c: %X\n", 0x40, 0x40);


    printf("%c: %X\n", 'A', 'A');
    printf("%c: %X\n", 'B', 'B');
    printf("%c: %X\n", 'C', 'C');
    printf("%c: %X\n", 'X', 'X');
    printf("%c: %X\n", 'Z', 'Z');
    printf("%c: %X\n", '0', '0');
    printf("%c: %X\n", '9', '9');
    printf("%c: %X\n", '1', '1');
    //print_syst((char*)&time);
}

void License::print_syst(char buf[8])
{
    SYSTEMTIME sys_time={0};
    FileTimeToSystemTime((FILETIME*)buf, &sys_time);

    printf("year %i\n", sys_time.wYear);
    printf("mnth %i\n", sys_time.wMonth);
    printf("DoW  %i\n", sys_time.wDayOfWeek);
    printf("day  %i\n", sys_time.wDay);
    printf("hour %i\n", sys_time.wHour);
    printf("min  %i\n", sys_time.wMinute);
    printf("sec  %i\n", sys_time.wSecond);
    printf("mils %i\n", sys_time.wMilliseconds);
}


License::AKey::AKey(const char* hex64)
{
    for(int i=0; i<16; i++)
    {
        akey = (akey << 4) | (hex64[i] - (hex64[i] > 0x39 ? 0x37 : 0x30));
    }
}

void check_exit(void*)
{
    ExitProcess(0);
}

void check_nop(void* data)
{
    auto unsigned char* ptr = (unsigned char*) data;
    for(unsigned int i=ptr[0]; i < ptr[1]; i++)
    {
        __asm nop
    }
    
     __asm
    {
        int 3
        push 0
        call dword ptr [ExitProcess]
    } 

}

akey_check default_checkers[]={
check_nop,
check_exit,
create_license,
};
//-----------------------------------------------------------------------------
