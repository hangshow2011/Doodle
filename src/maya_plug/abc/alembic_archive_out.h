#pragma once

#include <maya_plug/maya_plug_fwd.h>

#include <Alembic/Abc/All.h>
#include <Alembic/Abc/OArchive.h>
#include <Alembic/Abc/OTypedScalarProperty.h>
#include <Alembic/AbcCoreAbstract/TimeSampling.h>
#include <Alembic/AbcGeom/OPolyMesh.h>
#include <Alembic/AbcGeom/OXform.h>
#include <Alembic/Util/All.h>
#include <cstdint>
#include <filesystem>
#include <maya/MApiNamespace.h>
#include <maya/MDagPath.h>
#include <maya/MTime.h>
#include <memory>
#include <treehh/tree.hh>
#include <utility>
#include <vector>
namespace doodle::alembic {

class archive_out {
 public:
  using time_sampling_ptr    = Alembic::AbcCoreAbstract::TimeSamplingPtr;
  using o_archive_ptr        = std::shared_ptr<Alembic::Abc::OArchive>;
  using o_box3d_property_ptr = std::shared_ptr<Alembic::Abc::OBox3dProperty>;
  using o_xform_ptr          = std::shared_ptr<Alembic::AbcGeom::OXform>;
  using o_mesh_ptr           = std::shared_ptr<Alembic::AbcGeom::OPolyMesh>;

 private:
  struct dag_path_out_data {
    MDagPath dag_path_{};
    o_xform_ptr o_xform_ptr_{};
    o_mesh_ptr o_mesh_ptr_{};
    std::string name_{};
    bool write_mesh_{};
  };
  std::string m_name{};
  MTime begin_time{};
  MTime end_time{};
  FSys::path out_path_{};

  o_archive_ptr o_archive_{};
  time_sampling_ptr shape_time_sampling_{};
  time_sampling_ptr transform_time_sampling_{};

  std::int32_t shape_time_index_{};
  std::int32_t transform_time_index_{};
  o_box3d_property_ptr o_box3d_property_ptr_{};
  bool init_{};

  tree<dag_path_out_data> root_{};
  void open(const std::vector<MDagPath>& in_out_path);

  static std::tuple<std::uint16_t, std::uint16_t, std::uint16_t> get_rot_order();

  void wirte_transform(dag_path_out_data& in_path, const o_xform_ptr& in_parent_xform);
  void wirte_mesh(dag_path_out_data& in_path);
  void write_box();
  void wirte_frame(const dag_path_out_data& in_path);
  void write_frame_box();
  void create_time_sampling_2(const MTime& in_time_begin, const MTime& in_time_end);

 public:
  explicit archive_out(
      FSys::path in_path, const std::vector<MDagPath>& in_dag_path, const MTime& in_begin_time, const MTime& in_end_time
  )
      : out_path_(std::move(in_path)) {
    create_time_sampling_2(in_begin_time, in_end_time);
    open(in_dag_path);
  }
  void write();
};

}  // namespace doodle::alembic