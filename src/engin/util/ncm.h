#ifndef X_NCM_H
#define X_NCM_H

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>

namespace xutil {
inline static void convert_music(const std::string& path,
                                 std::string& desdirpath) {
  std::filesystem::path srcpath = std::filesystem::path(path);
  // 源文件路径
  auto absolutesrcpath = std::filesystem::absolute(srcpath).string();
  // 包裹单引号防空格
  absolutesrcpath.insert(absolutesrcpath.begin(), '\'');
  absolutesrcpath.append("\'");

  // 目标输出路径
#ifdef __APPLE__
  std::filesystem::path desdir = std::filesystem::path("ncmtemp/");
#endif  //__APPLE__
#ifdef __unix
  std::filesystem::path desdir = std::filesystem::path("ncmtemp/");
#endif  //__unix
#ifdef _WIN32
  std::filesystem::path desdir = std::filesystem::path("ncmtemp\\");
#endif  //__unix
  auto absolutedesdir = std::filesystem::absolute(desdir).string();
  // 目标文件名(去后缀)
  auto filename = srcpath.filename().stem().string();
  desdirpath = absolutedesdir + filename;

  std::cout << "输出路径:[" + desdirpath + "]" << std::endl;

  // 判断是否已经转换过(目标文件夹是否有同名文件)
  if (std::filesystem::is_directory(desdirpath) &&
      std::filesystem::directory_iterator(desdirpath) !=
          std::filesystem::end(
              std::filesystem::directory_iterator(desdirpath))) {
    std::cout << "检测到输出路径非空" << std::endl;
    // 目标文件夹是文件夹且不为空
    for (const auto& entry : std::filesystem::directory_iterator(desdirpath)) {
      // 判断是否是普通文件
      if (entry.is_regular_file()) {
        std::string ffileName = entry.path().filename().stem().string();
        // 检查文件名是否存在
        if (ffileName == filename) {
          // 存在,不需要再转换
          std::cout << "[" + ffileName + "]已转换过,跳过" << std::endl;
          return;
        }
      }
    }
  }

  // 目标路径(包裹单引号)
  desdirpath.insert(desdirpath.begin(), '\'');
  desdirpath.append("\'");

#ifdef __APPLE__
  std::string command = std::string("../lib/ncmdump-macos ") + absolutesrcpath +
                        " -o " + desdirpath;
#endif  //__APPLE__
#ifdef __linux__
  std::string command = std::string("../lib/ncmdump-linux ") + absolutesrcpath +
                        " -o " + desdirpath;
#endif  //__linux__
#ifdef _WIN32
  std::string command = std::string("../lib/ncmdump-macos ") + absolutesrcpath +
                        " -o " + desdirpath;
#endif  //_WIN32
  // 执行命令
  std::system(command.c_str());
  filename = srcpath.filename().replace_extension("").string();
  absolutesrcpath = std::filesystem::absolute(srcpath).string();
  desdirpath = absolutedesdir + filename;
}
}  // namespace xutil

#endif  // X_NCM_H
