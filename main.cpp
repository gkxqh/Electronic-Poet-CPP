
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <filesystem>
#include "httplib.h"

namespace fs = std::filesystem;

class ElectronicPoet {
private:
    std::vector<std::string> nouns;     // 名词列表
    std::vector<std::string> verbs;     // 动词列表
    std::vector<std::string> adjectives; // 形容词列表
    std::vector<std::string> sentences; // 句式列表
    std::vector<std::string> generatedPoetry; // 生成的诗句存储

    // 随机选择一个元素
    template<typename T>
    T getRandomElement(const std::vector<T>& vec) {
        int randomIndex = rand() % vec.size();
        return vec[randomIndex];
    }

    // 读取词库文件
    bool readDictionaryFile(const std::string& filename, std::vector<std::string>& target) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "打开字典" << filename << "失败！程序退出。\n";
            return false;
        }

        std::string line;
        while (std::getline(file, line)) {
            // 去除可能的换行符
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            target.push_back(line);
        }

        file.close();
        return true;
    }

    // 生成单句诗
    std::string generateSentence() {
        std::string pattern = getRandomElement(sentences);
        std::string result;

        for (char c : pattern) {
            if (c == 'n') {
                result += getRandomElement(nouns);
            } else if (c == 'v') {
                result += getRandomElement(verbs);
            } else if (c == 'a') {
                result += getRandomElement(adjectives);
            } else {
                result += c;
            }
        }

        return result;
    }

public:
    ElectronicPoet() {
        // 初始化随机数种子
        srand(static_cast<unsigned int>(time(nullptr)));
    }

    ~ElectronicPoet() {
        // vector会自动管理内存，无需手动释放
    }

    // 加载词库
    bool loadDictionaries() {
        if (!readDictionaryFile("noun", nouns)) return false;
        if (!readDictionaryFile("verb", verbs)) return false;
        if (!readDictionaryFile("adjective", adjectives)) return false;
        if (!readDictionaryFile("sentence", sentences)) return false;

        // 检查词库是否为空
        if (nouns.empty() || verbs.empty() || adjectives.empty() || sentences.empty()) {
            std::cerr << "词库为空，请检查词库文件！\n";
            return false;
        }

        return true;
    }

    // 生成指定行数的诗
    bool generatePoetry(int lines) {
        if (lines <= 0) {
            std::cerr << "行数必须大于0！\n";
            return false;
        }

        // 清空之前的生成结果
        generatedPoetry.clear();

        for (int i = 0; i < lines; i++) {
            std::string line = generateSentence();
            generatedPoetry.push_back(line);
        }

        return true;
    }

    // 获取生成的诗歌
    std::vector<std::string> getGeneratedPoetry() const {
        return generatedPoetry;
    }

    // 获取保存的诗歌列表
    std::vector<std::string> getSavedPoems() const {
        std::vector<std::string> poems;
        std::string dir = "collection";
        
        if (fs::exists(dir) && fs::is_directory(dir)) {
            for (const auto& entry : fs::directory_iterator(dir)) {
                if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                    std::string filename = entry.path().filename().string();
                    // 去掉.txt后缀
                    std::string title = filename.substr(0, filename.size() - 4);
                    poems.push_back(title);
                }
            }
        }
        
        return poems;
    }

    // 获取指定标题的诗歌内容
    std::vector<std::string> getPoemContent(const std::string& title) const {
        std::vector<std::string> content;
        std::string filename = "collection/" + title + ".txt";
        std::ifstream file(filename);
        
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                // 去除可能的换行符
                if (!line.empty() && line.back() == '\r') {
                    line.pop_back();
                }
                content.push_back(line);
            }
            file.close();
        }
        
        return content;
    }

    // 保存诗句到文件
    bool saveToFile(const std::string& title) {
        if (generatedPoetry.empty()) {
            std::cerr << "没有可保存的诗句！\n";
            return false;
        }

        // 使用filesystem库构建路径，确保跨平台兼容性
        fs::path dir = "collection";
        fs::path filename = dir / (title + ".txt");
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "创建文件失败！路径：" << filename.string() << "\n";
            return false;
        }

        for (const auto& line : generatedPoetry) {
            file << line << "\r\n";
        }

        file.close();
        std::cout << "诗句已成功保存到 " << filename.string() << "\n";
        return true;
    }
    
    // 导入诗歌到文件
    bool importPoem(const std::string& title, const std::vector<std::string>& content) {
        if (content.empty()) {
            std::cerr << "导入的诗歌内容为空！\n";
            return false;
        }

        // 使用filesystem库构建路径，确保跨平台兼容性
        fs::path dir = "collection";
        fs::path filename = dir / (title + ".txt");
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "创建文件失败！路径：" << filename.string() << "\n";
            return false;
        }

        for (const auto& line : content) {
            file << line << "\r\n";
        }

        file.close();
        std::cout << "诗歌已成功导入到 " << filename.string() << "\n";
        return true;
    }
    
    // 删除诗歌文件
    bool deletePoem(const std::string& title) {
        std::string filename = "collection/" + title + ".txt";
        if (fs::exists(filename)) {
            try {
                fs::remove(filename);
                std::cout << "诗歌已成功删除：" << filename << "\n";
                return true;
            } catch (const fs::filesystem_error& e) {
                std::cerr << "删除诗歌失败：" << e.what() << "\n";
                return false;
            }
        } else {
            std::cerr << "诗歌文件不存在：" << filename << "\n";
            return false;
        }
    }
};

// 创建collection目录
void createCollectionDir() {
    fs::create_directories("collection");
}

// 生成JSON响应
std::string generateJsonResponse(const std::string& status, const std::string& message, const std::vector<std::string>& data = {}) {
    std::ostringstream oss;
    oss << "{\"status\":\"" << status << "\",\"message\":\"" << message << "\",\"data\":";
    
    if (data.empty()) {
        oss << "[]";
    } else {
        oss << "[\"" << data[0] << "\"";
        for (size_t i = 1; i < data.size(); i++) {
            oss << ",\"" << data[i] << "\"";
        }
        oss << "]";
    }
    
    oss << "}";
    return oss.str();
}

// 生成HTML响应
std::string generateHtmlResponse() {
    std::ostringstream oss;
    oss << R"(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>电子诗人</title>
    <link rel="stylesheet" href="/style.css">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
</head>
<body>
    <div class="container">
        <nav class="menu-bar">
            <div class="menu-left">
                <h1 class="logo">电子诗人</h1>
            </div>
            <div class="menu-right">
                <button id="generate-nav-btn" class="btn btn-nav">
                    <i class="fas fa-feather-alt"></i> 生成诗歌
                </button>
                <button id="history-nav-btn" class="btn btn-nav">
                    <i class="fas fa-history"></i> 历史诗歌
                </button>
                <button id="theme-toggle" class="btn btn-nav theme-btn">
                    <i class="fas fa-moon"></i>
                </button>
            </div>
        </nav>
        
        <main>
            <section class="generate-section">
                <h2>生成诗歌</h2>
                <div class="input-group">
                    <label for="lines">请输入要生成的诗的行数：</label>
                    <input type="number" id="lines" name="lines" min="1" max="50" value="4">
                    <button id="generate-btn" class="btn">生成诗歌</button>
                </div>
            </section>
            
            <section class="poem-section" id="poem-section">
                <h2>生成的诗歌</h2>
                <div id="poem-content" class="poem-content"></div>
                <div class="save-group" id="save-group" style="display: none;">
                    <label for="title">请输入诗的标题：</label>
                    <input type="text" id="title" name="title" placeholder="诗歌标题">
                    <button id="save-btn" class="btn">保存诗歌</button>
                </div>
            </section>
            
            <section class="history-section">
                <h2>历史诗歌</h2>
                <div class="history-actions">
                    <input type="file" id="import-file" accept=".txt" style="display: none;">
                    <button id="import-btn" class="btn">
                        <i class="fas fa-upload"></i> 导入诗歌
                    </button>
                </div>
                <div id="poem-list" class="poem-list"></div>
            </section>
        </main>
        
        <footer>
            <p>&copy; 2025 电子诗人 - 基于C++和WebUI开发</p>
        </footer>
    </div>
    
    <script src="/script.js"></script>
</body>
</html>
)";
    return oss.str();
}

int main() {
    std::cout << "电子诗人Web服务器启动中...\n";
    
    // 创建collection目录
    createCollectionDir();

    ElectronicPoet poet;
    
    // 加载词库
    if (!poet.loadDictionaries()) {
        std::cerr << "加载词库失败！程序退出。\n";
        return 1;
    }
    
    // 创建HTTP服务器
    httplib::Server svr;
    
    // 根路径返回HTML页面
    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(generateHtmlResponse(), "text/html; charset=utf-8");
    });
    
    // 生成诗歌API
    svr.Post("/generate", [&poet](const httplib::Request& req, httplib::Response& res) {
        if (req.has_param("lines")) {
            int lines = std::stoi(req.get_param_value("lines"));
            if (poet.generatePoetry(lines)) {
                auto poem = poet.getGeneratedPoetry();
                res.set_content(generateJsonResponse("success", "诗歌生成成功", poem), "application/json; charset=utf-8");
            } else {
                res.set_content(generateJsonResponse("error", "诗歌生成失败"), "application/json; charset=utf-8");
            }
        } else {
            res.set_content(generateJsonResponse("error", "参数错误"), "application/json; charset=utf-8");
        }
    });
    
    // 保存诗歌API
    svr.Post("/save", [&poet](const httplib::Request& req, httplib::Response& res) {
        if (req.has_param("title")) {
            std::string title = req.get_param_value("title");
            if (poet.saveToFile(title)) {
                res.set_content(generateJsonResponse("success", "诗歌保存成功"), "application/json; charset=utf-8");
            } else {
                res.set_content(generateJsonResponse("error", "诗歌保存失败"), "application/json; charset=utf-8");
            }
        } else {
            res.set_content(generateJsonResponse("error", "参数错误"), "application/json; charset=utf-8");
        }
    });
    
    // 获取诗歌列表API
    svr.Get("/poems", [&poet](const httplib::Request&, httplib::Response& res) {
        auto poems = poet.getSavedPoems();
        res.set_content(generateJsonResponse("success", "获取诗歌列表成功", poems), "application/json; charset=utf-8");
    });
    
    // 获取诗歌内容API
    svr.Get(R"(/poem/(.*))", [&poet](const httplib::Request& req, httplib::Response& res) {
        std::string title = req.matches[1];
        auto content = poet.getPoemContent(title);
        if (!content.empty()) {
            res.set_content(generateJsonResponse("success", "获取诗歌内容成功", content), "application/json; charset=utf-8");
        } else {
            res.set_content(generateJsonResponse("error", "诗歌不存在"), "application/json; charset=utf-8");
        }
    });
    
    // 删除诗歌API
    svr.Delete(R"(/poem/(.*))", [&poet](const httplib::Request& req, httplib::Response& res) {
        std::string title = req.matches[1];
        if (poet.deletePoem(title)) {
            res.set_content(generateJsonResponse("success", "诗歌删除成功"), "application/json; charset=utf-8");
        } else {
            res.set_content(generateJsonResponse("error", "诗歌删除失败"), "application/json; charset=utf-8");
        }
    });
    
    // 导入诗歌API
    svr.Post("/import", [&poet](const httplib::Request& req, httplib::Response& res) {
        if (req.has_param("title") && req.has_param("content")) {
            std::string title = req.get_param_value("title");
            std::string content = req.get_param_value("content");
            
            // 解析诗歌内容为行列表
            std::vector<std::string> lines;
            std::istringstream stream(content);
            std::string line;
            while (std::getline(stream, line)) {
                if (!line.empty() && line.back() == '\r') {
                    line.pop_back();
                }
                if (!line.empty()) {
                    lines.push_back(line);
                }
            }
            
            if (poet.importPoem(title, lines)) {
                res.set_content(generateJsonResponse("success", "诗歌导入成功"), "application/json; charset=utf-8");
            } else {
                res.set_content(generateJsonResponse("error", "诗歌导入失败"), "application/json; charset=utf-8");
            }
        } else {
            res.set_content(generateJsonResponse("error", "参数错误"), "application/json; charset=utf-8");
        }
    });
    
    // 静态文件服务
    svr.set_mount_point("/", "./web");
    
    // 启动服务器
    std::cout << "服务器已启动，访问地址：http://localhost:8080\n";
    if (!svr.listen("0.0.0.0", 8080)) {
        std::cerr << "服务器启动失败！\n";
        return 2;
    }
    
    return 0;
}
