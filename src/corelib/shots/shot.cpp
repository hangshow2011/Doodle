﻿#include "shot.h"
#include <corelib/Exception/Exception.h>
#include <corelib/shots/ShotModifySQLDate.h>
#include <corelib/core/coresql.h>
#include <corelib/shots/episodes.h>

#include <loggerlib/Logger.h>
#include <boost/format.hpp>

#include <magic_enum.hpp>

#include <sqlpp11/mysql/mysql.h>
#include <sqlpp11/sqlpp11.h>
#include <corelib/coreOrm/shots_sqlOrm.h>
//反射使用
#include <rttr/registration>

#include <memory>

DOODLE_NAMESPACE_S

RTTR_REGISTRATION {
  rttr::registration::class_<shot>(DOCORE_RTTE_CLASS(shot))
      .constructor<>()(rttr::policy::ctor::as_std_shared_ptr);
}

DOODLE_INSRANCE_CPP(shot);
const std::vector<std::string> shot::e_shotAB_list = {"B", "C", "D", "E",
                                                      "F", "G", "H"};

shot::shot()
    : coresqldata(),
      std::enable_shared_from_this<shot>(),
      p_qint_shot_(-1),
      p_qenm_shotab(e_shotAB::_),
      p_eps_id(-1),
      p_ptr_eps(),
      p_inDeadline(false) {
  p_instance.insert(this);
}

shot::~shot() {
  p_instance.erase(this);
}

void shot::select(const qint64 &ID_) {
  doodle::Shots table{};
  auto db = coreSql::getCoreSql().getConnection();
  for (auto &&row : db->run(sqlpp::select(sqlpp::all_of(table))
                                .from(table)
                                .where(table.id == ID_))) {
    idP          = row.id;
    p_qint_shot_ = row.shot;
    setShotAb((dstring)row.shotab);
    p_eps_id = row.episodesId;
  }
}

void shot::insert() {
  if (idP > 0) return;

  doodle::Shots table{};
  auto db      = coreSql::getCoreSql().getConnection();
  auto install = sqlpp::insert_into(table).columns(table.episodesId, table.shot,
                                                   table.shotab);

  install.values.add(
      table.episodesId = p_eps_id, table.shot = p_qint_shot_,
      table.shotab = sqlpp::value_or_null<dstring>(getShotAb_str()));
  idP = db->insert(install);
  if (idP == 0) {
    DOODLE_LOG_WARN("无法插入镜头" << p_qint_shot_);
    throw std::runtime_error("not install shots");
  }
}

void shot::updateSQL() {
  doodle::Shots table{};
  auto db = coreSql::getCoreSql().getConnection();
  try {
    db->update(sqlpp::update(table)
                   .set(table.shot   = p_qint_shot_,
                        table.shotab = sqlpp::value_or_null<dstring>(getShotAb_str()))
                   .where(table.id == idP));
  } catch (const sqlpp::exception &err) {
    DOODLE_LOG_WARN(err.what());
  }
}

void shot::deleteSQL() {
  doodle::Shots table{};
  auto db = coreSql::getCoreSql().getConnection();
  db->remove(sqlpp::remove_from(table).where(table.id == idP));
}

shotPtrList shot::getAll(const episodesPtr &EP_) {
  EP_->ShotModifySqlDate()->selectModify();
  shotPtrList list{};
  doodle::Shots table{};
  auto db = coreSql::getCoreSql().getConnection();
  for (auto &&row :
       db->run(sqlpp::select(sqlpp::all_of(table))
                   .from(table)
                   .where(table.episodesId == EP_->getIdP())
                   .order_by(table.shot.asc(), table.shotab.asc()))) {
    auto item          = std::make_shared<shot>();
    item->idP          = row.id;
    item->p_qint_shot_ = row.shot;
    item->setShotAb(row.shotab);
    item->setEpisodes(EP_);
    item->p_inDeadline = EP_->ShotModifySqlDate()->inDeadline(item->idP);
    list.push_back(item);
  }
  return list;
}

void shot::setEpisodes(const episodesPtr &value) {
  if (!value) return;
  p_eps_id  = value->getIdP();
  p_ptr_eps = value;
}

episodesPtr shot::getEpisodes() {
  if (p_ptr_eps)
    return p_ptr_eps;
  else {
    auto epi = episodes::Instances();
    auto eps_iter =
        std::find_if(epi.begin(), epi.end(),
                     [=](episodes *ptr) -> bool { return ptr->getIdP() == p_eps_id; });
    if (eps_iter != epi.end()) {
      p_ptr_eps = (*eps_iter)->shared_from_this();

    } else {
      throw nullptr_error(std::string{"shot id :"}
                              .append(std::to_string(getIdP()))
                              .append("  ")
                              .append(getShotAndAb_str()));
    }

    setEpisodes(p_ptr_eps);
    return p_ptr_eps;
  }
}

void shot::setShot(const int64_t &sh, const e_shotAB &ab) {
  p_qint_shot_  = sh;
  p_qenm_shotab = ab;
}

void shot::setShot(const int64_t &sh, const dstring &ab) {
  p_qint_shot_ = sh;
  setShotAb(ab);
}

void shot::setShotAb(const dstring &ab) {
  if (ab.empty())
    p_qenm_shotab = e_shotAB::_;
  else {
    auto enum_ab = magic_enum::enum_cast<e_shotAB>(ab);
    if (enum_ab.has_value()) {
      p_qenm_shotab = enum_ab.value();
    }
  }
}

dstring shot::getShotAndAb_str() const {
  boost::format str("%1%%2%");
  str % getShot_str() % getShotAb_str();
  return str.str();
}

dstring shot::getShot_str() const {
  boost::format str(DOODLE_SHFORMAT);
  str % p_qint_shot_;
  return str.str();
}

dstring shot::getShotAb_str() const {
  dstring str;
  switch (p_qenm_shotab) {
    case e_shotAB::_:
      str = "";
      break;
    default:
      std::string tmpstr(magic_enum::enum_name(p_qenm_shotab));
      str = tmpstr;
      break;
  }
  return str;
}

bool shot::inDeadline() const {
  return p_inDeadline;
}

const std::unordered_set<shot *> shot::Instances() {
  return p_instance;
}
DOODLE_NAMESPACE_E
