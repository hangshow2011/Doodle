#include "pathParsing.h"

#include <corelib/fileDBInfo/filesqlinfo.h>
#include <corelib/Exception/Exception.h>
#include <corelib/core/coreset.h>
#include <corelib/filesystem/FileSystem.h>

#include <loggerlib/Logger.h>

#include <nlohmann/json.hpp>
#include <boost/filesystem.hpp>

#include <sstream>

DOODLE_NAMESPACE_S
pathParsing::pathParsing(fileSqlInfo* file)
    : p_file_Archive(file),
      p_path_row(std::make_shared<fileSys::path>()),
      p_path_list() {
}

dpathList pathParsing::Path(const std::string& pathstr) {
  /* *
   * 情况一：
   * *传入路径
   * 情况二：
   * *传入json
   * 情况三：
   * *传入字符串 
   */
  fileSys::path path{pathstr};

  if (pathstr.empty())
    throw std::runtime_error("字符串空");

  auto& fileSys = DfileSyntem::get();

  if (fileSys.exists(pathstr) && path.extension() == ".json") {
    auto file = fileSys.readFileToString(path);

    nlohmann::json root{};
    try {
      root = nlohmann::json::parse(*file);
      for (auto&& i : root) {
        if (i.is_string())
          p_path_list.push_back(i.get<std::string>());
      }
    } catch (const nlohmann::json::parse_error& err) {
      std::cout << err.what() << std::endl;
      p_path_list.push_back(pathstr);
    }

  } else {
    try {
      //如果是pathjson流就直接解析
      auto root = nlohmann::json::parse(pathstr);
      for (auto&& i : root) {
        if (i.is_string())
          p_path_list.push_back(i.get<std::string>());
      }

    } catch (const nlohmann::json::parse_error& err) {
      std::cout << err.what() << std::endl;
      p_path_list.push_back(pathstr);
    }
  }

  return p_path_list;
}

dpathList pathParsing::Path() const {
  return p_path_list;
}

void pathParsing::setPath(const dpathList& path) {
  p_path_list = path;
}

void pathParsing::write() {
  nlohmann::json root;
  for (auto&& path : p_path_list) {
    root.push_back(path.generic_string());
  }
  try {
    *p_path_row = p_file_Archive->generatePath("doodle", ".json");
  } catch (const nullptr_error& err) {
    DOODLE_LOG_ERROR(err.what());
  }
  auto strPtr = std::make_shared<std::string>();
  strPtr->append(root.dump());
  DfileSyntem::get().writeFile(*p_path_row, strPtr);
}

std::string pathParsing::DBInfo() const {
  if (!p_path_row->empty()) {
    return p_path_row->generic_string();
  } else {
    throw nullptr_error("path is empty");
  }
}

void pathParsing::setFileSql(fileSqlInfo* file) {
  p_file_Archive = file;
}

DOODLE_NAMESPACE_E