//
// Created by TD on 2022/10/8.
//

#include "rpc_server_facet.h"

#include <doodle_app/app/program_options.h>

#include "doodle_lib/distributed_computing/server.h"
#include <doodle_lib/long_task/image_to_move.h>

#include <boost/program_options.hpp>

#include <wil/result.h>

namespace doodle::facet {

const std::string& rpc_server_facet::name() const noexcept { return name_; }
void rpc_server_facet::operator()() {
  server_attr = std::make_shared<doodle::distributed_computing::server_ptr::element_type>();
  server_attr->run();
  work_ = std::make_shared<decltype(work_)::element_type>(boost::asio::make_work_guard(g_io_context()));
}
void rpc_server_facet::deconstruction() { server_attr.reset(); }

rpc_server_facet::~rpc_server_facet() = default;
}  // namespace doodle::facet
