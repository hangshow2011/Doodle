/*
 * @Author: your name
 * @Date: 2020-12-12 19:17:38
 * @LastEditTime: 2020-12-15 20:59:16
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \Doodle\doodle_server\server.cpp
 */

#include "server.h"
#include <boost/filesystem.hpp>
#include <boost/network.hpp>
#include <boost/network/uri.hpp>
#include <boost/regex.hpp>

/*保护data里面的宏__我他妈的*/
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#include <date/date.h>
/*保护data里面的宏__我他妈的*/

#include <chrono>
DOODLE_NAMESPACE_S

fileSystem::fileSystem()
    : p_project_roots() {
  p_project_roots.insert({"dubuxiaoyao3", std::make_shared<boost::filesystem::path>("D:/")});
}

bool fileSystem::has(const std::string& project_name, const path_ptr& path) {
  auto path_iter = p_project_roots.find(project_name);
  if (path_iter != p_project_roots.end())
    return boost::filesystem::exists(*(path_iter->second) / (*path));
  else
    return false;
}

bool fileSystem::add(const std::string& project_name, const path_ptr& path) {
  return false;
}

bool fileSystem::get(const std::string& project_name, const path_ptr& path) {
  return false;
}

decltype(auto) fileSystem::mata(const std::string& project_name, const path_ptr& path) {
  std::map<std::string, std::string> handle{};
  auto path_iter = p_project_roots.find(project_name);
  if (path_iter != p_project_roots.end()) {
    boost::filesystem::path path(*(path_iter->second) / (*path));
    if (boost::filesystem::is_directory(path)) {
      handle.insert({"file_size", std::to_string(-1)});
    } else {
      handle.insert({"file_size", std::to_string(boost::filesystem::file_size(path))});
    }
    auto tmp_time = boost::filesystem::last_write_time(path);

    auto time = std::chrono::system_clock::from_time_t(tmp_time);
    handle.insert({"modify_time",
                   date::format("%m %d %Y %H:%M:%S", time)});
  } else {
    handle.insert({"file_size", ""});
    handle.insert({"modify_time", ""});
  }
  return handle;
}

void fileSystem::setPrject(const std::pair<std::string, path_ptr>& project_root) {
  p_project_roots.insert(project_root);
}

/* -------------------------------------------------------------------------- */
/* ---------------------------------连接处理---------------------------------- */
/* -------------------------------------------------------------------------- */

connection_Handler::connection_Handler(fileSystem_ptr f_ptr)
    : p_fileSystem(std::move(f_ptr)), p_project_name() {}

void connection_Handler::operator()(const std::string& path, Server::connection_ptr conn,
                                    bool server_body) {
  static constexpr std::size_t MAX_INPUT_SIZE = 2 << 15;
  static const boost::regex re(R"(([0-9|a-z|/]+)[\?]?[\?|\&]\bproject=([^&]+))");
  boost::smatch match;
  if (boost::regex_search(path, match, re)) {
    if (match.size() >= 2) {
      std::string tmp_p(match[1]);
      std::string prj_name(match[2]);
      auto k_path = std::make_shared<boost::filesystem::path>(tmp_p);
      std::cout << k_path->generic_string() << ": " << prj_name << std::endl;
      if (p_fileSystem->has(p_project_name, k_path)) {
        conn->set_headers(p_fileSystem->mata(prj_name, k_path));
        conn->set_status(Server::connection::ok);
        if (server_body)
          send_file(k_path, conn);
      } else {
        static Server::response_header handle[] = {{"Connection", "close"},
                                                   {"Content-Type", "text/plain"}};
        conn->set_status(Server::connection::not_found);
        conn->set_headers(boost::make_iterator_range(handle, handle + 2));
        conn->write("File Not Found!");
      }
    }
  }
}

void connection_Handler::send_file(const path_ptr& f_ptr,
                                   Server::connection_ptr conn) {
}

/* -------------------------------------------------------------------------- */
/* ---------------------------------- 主要服务器 --------------------------------- */
/* -------------------------------------------------------------------------- */

Handler::Handler(fileSystem_ptr f_ptr) : p_fileSystem(std::move(f_ptr)) {
}
void Handler::operator()(Server::request const& request,
                         const Server::connection_ptr& connection_) {
  std::string k_dest = destination(request);
  if (request.method == "HEAD") {
    auto handle = std::make_shared<connection_Handler>(p_fileSystem);
    (*handle)(k_dest, connection_, false);
  } else if (request.method == "GET") {
    auto handle = std::make_shared<connection_Handler>(p_fileSystem);
    (*handle)(k_dest, connection_, true);
  } else if (request.method == "PUT" || request.method == "POST") {
  } else {
    static Server::response_header error_header[] = {"Connection", "close"};
    connection_->set_status(Server::connection::not_supported);
    connection_->set_headers(boost::make_iterator_range(error_header, error_header + 1));
    connection_->write("Method not supported.");
  }
}
DOODLE_NAMESPACE_E
