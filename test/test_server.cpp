﻿//
// Created by TD on 2021/5/26.
//

#include <DoodleLib/DoodleLib.h>
#include <grpcpp/grpcpp.h>
#include <gtest/gtest.h>

TEST(Server, createPrj) {
  doodle::CoreSet::getSet().guiInit();

  auto k_f = std::make_shared<doodle::MetadataFactory>();
  auto prj = std::make_shared<doodle::Project>("D:/", "测试");
  prj->insert_into(k_f);
  std::cout << prj->getId() << std::endl;
  prj = std::make_shared<doodle::Project>("D:/", "测试2");
  prj->insert_into(k_f);

  doodle::CoreSet::getSet().guiInit();

  for (const auto& k_prj : doodle::CoreSet::getSet().p_project_vector) {
    std::cout << "id: " << k_prj->getId() << "\n"
              << "uuid: " << k_prj->getUUID() << "\n"
              << "name: " << k_prj->getName() << "\n"
              << "path: " << k_prj->getPath() << "\n";
  }
  std::cout << std::endl;

  ASSERT_TRUE(doodle::CoreSet::getSet().p_project_vector.size() == 2);
}
#include <date/date.h>

TEST(Server, dow_updata) {
  using namespace doodle;
  doodle::CoreSet::getSet().guiInit();

  auto k_ch = doodle::CoreSet::getSet().getRpcFileSystemClient();
  k_ch->Upload("D:/WinDev2012Eval.VirtualBox.7z", "test/test.file.7z");
  k_ch->Upload("D:\\Kitchen_set","test/tmp");
  auto [k_t_ex, k_t_dir] = k_ch->IsFolder("test");
  auto [k_f_ex, k_f_dir] = k_ch->IsFolder("test/test.file.7z");
  std::cout << "is ex: " << k_ch->IsExist("test/test.file.7z") << "\n"
            << "test is ex: " << k_t_ex << "\n"
            << "test is folder: " << k_t_dir << "\n"
            << "test/test.file.7z is ex: " << k_t_ex << "\n"
            << "test/test.file.7z is folder: " << k_f_dir << "\n"
            << "test/test.file.7z time: " << date::format("%Y/%m/%d %H:%M", k_ch->GetTimestamp("test/test.file.7z")) << "\n"
            << "test/test.file.7z size: " << k_ch->GetSize("test/test.file.7z") << "\n"
            << std::endl;
  doodle::CoreSet::getSet().clear();
  k_ch->Download("D:/WinDev2012Eval_test.VirtualBox.7z", "test/test.file.7z");
  k_ch->Download("D:\\Kitchen_set2","test/tmp");
}
