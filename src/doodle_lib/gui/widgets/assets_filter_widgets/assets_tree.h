//
// Created by td_main on 2023/6/16.
//

#pragma once
#include <doodle_core/doodle_core_fwd.h>

#include "boost/operators.hpp"

#include <string>
#include <treehh/tree.hh>
#include <utility>
namespace doodle::gui {

class assets_tree {
  struct assets_tree_node : boost::less_than_comparable<assets_tree_node> {
   public:
    std::string name;
    bool has_select{};
    entt::handle handle{};
    assets_tree_node() = default;
    assets_tree_node(std::string in_name, bool in_has_select, const entt::handle& in_handle)
        : name(std::move(in_name)), has_select(in_has_select), handle(in_handle) {}

    bool operator<(const assets_tree_node& rhs) const;
  };

  using tree_type_t = tree<assets_tree_node>;
  tree_type_t tree_;

  void build_tree(const entt::handle& in_handle_view, const tree_type_t::iterator& in_parent);
  bool render_child(const tree_type_t::iterator& in_node);
  void popen_menu(const tree_type_t::iterator_base& in);

 public:
  assets_tree() = default;

  void init_tree();
  bool render();
};

}  // namespace doodle::gui
