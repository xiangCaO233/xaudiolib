#ifndef X_NCM_H
#define X_NCM_H

#include <cstdlib>
#include <filesystem>
#include <string>

namespace xutil {
inline static void convert_music(const std::string& path,
                                 std::string& desdirpath) {
    std::filesystem::path srcpath = std::filesystem::path(path);
    std::filesystem::path desdir = std::filesystem::path("temp/");
    // 包裹单引号防空格
    auto absolutesrcpath = std::filesystem::absolute(srcpath).string();
    absolutesrcpath.insert(absolutesrcpath.begin(), '\'');
    absolutesrcpath.append("\'");
    auto absolutedesdir = std::filesystem::absolute(desdir).string();
    auto filename = srcpath.filename().replace_extension("").string();
    filename.insert(filename.begin(), '\'');
    filename.append("\'");
    desdirpath = absolutedesdir + filename;
#ifdef __APPLE__
    desdirpath.append("/");
    std::string command =
        std::string("../lib/ncmdump ") + absolutesrcpath + " -o " + desdirpath;
    // 执行命令
    std::system(command.c_str());
    filename = srcpath.filename().replace_extension("").string();
    absolutesrcpath = std::filesystem::absolute(srcpath).string();
    desdirpath = absolutedesdir + filename;
#endif  //__APPLE__
}
}  // namespace xutil

#endif  // X_NCM_H
