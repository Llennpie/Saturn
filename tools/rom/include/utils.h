#ifndef FSUtils_H
#define FSUtils_H

#include <filesystem>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cassert>
#include <cstring>
#include <vector>
#include <string>
#include <cstdio>
#include <memory>
#include <array>

#define IS_64_BIT (UINTPTR_MAX == 0xFFFFFFFFFFFFFFFFU)
#define IS_BIG_ENDIAN (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)

namespace fs = std::filesystem;

#ifndef CWD
#define CWD "."
#endif

namespace MoonUtils {

#if IS_BIG_ENDIAN
    const char* endian = "big";
#else
    const char* endian = "little";
#endif

#if IS_64_BIT
    const char* bitwidth = "64";
#else
    const char* bitwidth = "32";
#endif

    const char* cwd = NULL;

    char *sys_strdup(const char *src) {
        const unsigned len = strlen(src) + 1;
        char *newstr = new char[len];
        if (newstr) memcpy(newstr, src, len);
        return newstr;
    }

    std::string normalize(std::string path){
        std::replace(path.begin(), path.end(), '\\', '/');
        return path;
    }

    std::string join(std::string base, std::string file){
        if( file == "/" ) return normalize(base + file);
        return normalize( (fs::path(base) / fs::path(file)).string() );
    }

    std::string dirname(std::string path){
        return normalize( fs::path(path).parent_path().string());
    }

    std::string filename(std::string path){
        return normalize( fs::path(path).filename().string() );
    }

    std::string basename(std::string path){
        return normalize( fs::path(path).stem().string() );
    }

    template< typename... Args >
    std::string format( const char* format, Args... args ) {
        int length = std::snprintf( nullptr, 0, format, args... );
        assert( length >= 0 );

        char* buf = new char[length + 1];
        std::snprintf( buf, length + 1, format, args... );

        std::string str( buf );
        delete[] buf;
        return str;
    }

    std::string exec(std::string cmd) {
        std::string tcwd(CWD);
        cwd = sys_strdup(tcwd.substr(0, tcwd.find_last_of("/")).c_str());
        std::array<char, 128> buffer;
        std::string result;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
        if (!pipe) {
            throw std::runtime_error("popen() failed!");
        }
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
        return result;
    }

    bool exists(std::string path){
        return fs::exists(path);
    }

    void mkdir(std::string path){
        fs::create_directories(path);
    }

    void rm(std::string path){
        if(!fs::exists(path)) return;
        fs::remove_all(path);
    }

    std::string relative(std::string parent, std::string child){
        return normalize(fs::relative(child, parent).string());
    }

    void move(std::string src, std::string dst){
        std::vector<std::string> trashcan;
        for(auto& p: fs::recursive_directory_iterator(src)){
            // Create path in target, if not existing.
            const auto relativeSrc = fs::relative(p, src);
            const auto targetParentPath = dst / relativeSrc.parent_path();
            fs::create_directories(targetParentPath);

            // Copy to the targetParentPath which we just created.
            fs::copy(p, targetParentPath, fs::copy_options::overwrite_existing);

            std::string dname = p.path().parent_path().string();
            trashcan.push_back(dname);
        }
       for(auto& p: trashcan) rm(p);
    }

    void copy(std::string src, std::string dst){
        if(!fs::exists(dst))
            fs::create_directories(dst);
        fs::copy(src, dst, fs::copy_options::overwrite_existing | fs::copy_options::recursive);
    }

    void writeFile(std::string path, std::string content){
        std::ofstream out(path);
        out << content;
        out.close();
    }

    void dirscan(std::string path, std::vector<std::string> &files){
        if(!fs::is_directory(path)) return;
        for (auto& p : fs::directory_iterator(path)){
            std::string fpath = p.path().string();
            files.push_back(fpath);
            dirscan(fpath, files);
        }
    }

    std::string extname(std::string file) {
        const char *filename = file.c_str();
        const char *dot = strrchr(filename, '.');
        if(!dot || dot == filename) return "";
        return std::string(dot + 1);
    }
}

#endif