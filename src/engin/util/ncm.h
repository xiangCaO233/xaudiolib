#ifndef X_NCM_H
#define X_NCM_H

#include <cstdlib>
#include <filesystem>
#include <string>

#include "../../log/colorful-log.h"
#ifdef _WIN32
#include <windows.h>
#endif

namespace xutil {
inline static void convert_music(const std::string &path,
                                 std::string &desdirpath) {
  auto srcpath = std::filesystem::path(path);
  // 源文件路径
  auto absolutesrcpath = std::filesystem::absolute(srcpath).string();
  // 包裹单引号防空格
  absolutesrcpath.insert(absolutesrcpath.begin(), '"');

  auto tmppath = std::filesystem::path("ncmtemp/");
  // 创建ncmtemp目录存放ncmdump导出的东西
  if (!std::filesystem::exists(tmppath)) {
    std::filesystem::create_directories(tmppath);
  }
  // 目标输出路径
#ifdef __APPLE__
  std::filesystem::path desdir = std::filesystem::path("ncmtemp/");
  auto absolutedesdir = std::filesystem::absolute(desdir).string();
  // 目标文件名(去后缀)
  auto filename = srcpath.filename().stem().string();

  desdirpath = absolutedesdir + filename;
#endif  //__APPLE__
#ifdef __unix
  auto desdir = std::filesystem::path("ncmtemp/");
  auto absolutedesdir = std::filesystem::absolute(desdir).string();
  // 目标文件名(去后缀)
  auto filename = srcpath.filename().stem().string();

  desdirpath = absolutedesdir + filename;
#endif  //__unix
#ifdef _WIN32
  absolutesrcpath.replace(absolutesrcpath.find_last_of("\\"), 1, "/'");
  absolutesrcpath.append("\'");
  std::filesystem::path desdir = std::filesystem::path("ncmtemp/");
  auto absolutedesdir = std::filesystem::absolute(desdir).string();
  // 目标文件名(去后缀)
  auto filename = srcpath.filename().stem().string();

  desdirpath = absolutedesdir + filename;
#endif  //__unix
  absolutesrcpath.append("\"");

  XINFO("ncmdump输出路径:[" + desdirpath + "]");

  // 判断是否已经转换过(目标文件夹是否有同名文件)
  if (std::filesystem::is_directory(desdirpath) &&
      std::filesystem::directory_iterator(desdirpath) !=
          std::filesystem::end(
              std::filesystem::directory_iterator(desdirpath))) {
    XINFO("检测到输出路径非空");
    // 目标文件夹是文件夹且不为空
    for (const auto &entry : std::filesystem::directory_iterator(desdirpath)) {
      // 判断是否是普通文件
      if (entry.is_regular_file()) {
        std::string ffileName = entry.path().filename().stem().string();
        // 检查文件名是否存在
        if (ffileName == filename) {
          // 存在,不需要再转换
          XINFO("[" + ffileName + "]已转换过,跳过");
          return;
        }
      }
    }
  }

#ifdef _WIN32
  desdirpath = absolutedesdir + "\'" + filename + "\'";
#endif

  // 目标路径(包裹引号)
  desdirpath.insert(desdirpath.begin(), '"');
  desdirpath.append("\"");

#ifdef __APPLE__
  std::string command = std::string("../lib/ncmdump-macos ") + absolutesrcpath +
                        " -o " + desdirpath;
  // 执行命令
  std::system(command.c_str());
#endif  //__APPLE__
#ifdef __linux__
  std::string command = std::string("../lib/ncmdump-linux ") + absolutesrcpath +
                        " -o " + desdirpath;
  // 执行命令
  std::system(command.c_str());
#endif  //__linux__
#ifdef _WIN32
  // 获取当前工作区路径
  char buffer[MAX_PATH];
  DWORD result = GetCurrentDirectory(MAX_PATH, buffer);
  if (result == 0) {
    // 获取失败
    XERROR("获取工作目录失败(" + std::to_string(GetLastError()) + ").");
  } else if (result > MAX_PATH) {
    // 缓冲区太小
    XERROR("缓冲区过小. 需求大小: " + std::to_string(result));
  }
  auto wd = std::string(buffer);
  std::string command = std::string("powershell.exe -Command cd " + wd +
                                    "; ..\\lib\\ncmdump.exe ") +
                        absolutesrcpath + " -o " + desdirpath;

  // 获取所需的缓冲区大小
  int bufferSize =
      MultiByteToWideChar(CP_UTF8, 0, command.c_str(), -1, nullptr, 0);
  if (bufferSize == 0) {
    XERROR("多字节字符转换失败");
  }
  // 分配缓冲区
  std::wstring wcommand(bufferSize, 0);
  // 执行转换
  if (MultiByteToWideChar(CP_UTF8, 0, command.c_str(), -1, &wcommand[0],
                          bufferSize) == 0) {
    XERROR("多字节字符转换失败");
  }

  // 移除末尾的空字符
  wcommand.resize(bufferSize - 1);
  wchar_t cmdLineCopy[256];
  wcscpy_s(cmdLineCopy, wcommand.c_str());
  XDEBUG("command context:" + command);

  // 初始化 STARTUPINFO 和 PROCESS_INFORMATION 结构体
  STARTUPINFOW si;
  PROCESS_INFORMATION pi;

  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));

  // 创建子进程
  if (!CreateProcessW(NULL,         // 不使用模块名
                      cmdLineCopy,  // 命令行
                      NULL,         // 进程句柄不可继承
                      NULL,         // 线程句柄不可继承
                      FALSE,        // 不继承句柄
                      0,            // 无创建标志
                      NULL,         // 使用父进程的环境块
                      NULL,         // 使用父进程的起始目录
                      &si,          // 指向 STARTUPINFO 的指针
                      &pi           // 指向 PROCESS_INFORMATION 的指针
                      )) {
    XERROR("进程创建失败(" + std::to_string(GetLastError()) + ").");
  }

  // 等待子进程结束
  WaitForSingleObject(pi.hProcess, INFINITE);

  // 关闭进程和线程句柄
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
#endif  //_WIN32
  filename = srcpath.filename().replace_extension("").string();
  absolutesrcpath = std::filesystem::absolute(srcpath).string();
  desdirpath = absolutedesdir + filename;
}
}  // namespace xutil

#endif  // X_NCM_H
