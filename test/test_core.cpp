#include <DoodleLib/DoodleLib.h>
#include <gtest/gtest.h>

#include <boost/filesystem.hpp>

#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <iostream>
#include <memory>
#include <streambuf>
#include <date/date.h>
class CoreTest : public ::testing::Test {
 protected:
  void SetUp() override;
  void TearDown() override;

  doodle::CoreSet& set = doodle::CoreSet::getSet();

  doodle::FSys::path p_maya_path;
  doodle::FSys::path p_image_path;
  doodle::FSys::path p_video_path;
  doodle::FSys::path p_video_path_out1;
  doodle::FSys::path p_video_path_out2;
  doodle::FSys::path p_txt_path;
  doodle::FSys::path p_ue4_path;
  doodle::FSys::path p_long_path;
};

void CoreTest::SetUp() {
  // auto prj = std::make_shared<doodle::Project>("W:/");
  // set.setProject(prj);
  set.guiInit();
  p_maya_path = LR"(D:\shot_ep016_sc0032_Anm_Animation_v0001_zhengshanshan.ma)";

  p_image_path = LR"(D:\sc_064)";
  p_video_path = LR"(D:\video)";

  p_video_path_out1 = LR"(D:\voide\test1.mp4)";
  p_video_path_out2 = LR"(D:\voide\test2.mp4)";
  p_txt_path        = LR"(D:\test.txt)";
  p_ue4_path        = LR"(F:\Users\teXiao\Documents\Unreal_Projects\test_tmp\test_tmp.uproject)";
  p_long_path       = LR"(F:\Users\teXiao\Documents\Unreal_Projects\test_tmp\Content\Dev\test_long_path\test_long_path\test_long_path\test_long_path\test_long_path\test_long_path\test_long_path\test_long_path\test_long_path\test_long_path\test_long_path\test_long_path\test_long_path\test_long_path\test_long_path\test_long_path\NewMaterial.uasset)";
}

void CoreTest::TearDown() {
  set.clear();
}

TEST_F(CoreTest, archive) {
  auto& ue_set    = doodle::Ue4Setting::Get();
  auto str_stream = std::stringstream{};

  auto str_stream_bin = std::stringstream{};
  {
    cereal::JSONOutputArchive json{str_stream};
    json(cereal::make_nvp("mainset", set));
    // cereal::BinaryOutputArchive binary{std::cout};
    cereal::BinaryOutputArchive binary2{str_stream_bin};

    doodle::MetadataSet::Get().installProject(std::make_shared<doodle::Project>(
        "D:/", "test22333"));
    binary2(set);
  }

  std::cout << str_stream.str() << std::endl;
  ue_set.setVersion("4.26");

  {
    cereal::JSONInputArchive json{str_stream};
    json(set);
    cereal::BinaryInputArchive binary{str_stream_bin};
    binary(set);
  }
  std::cout
      << "\nuser : " << set.getUser()
      << "\nuser_en : " << set.getUser_en()
      << "\ndoc : " << set.getDoc()
      << "\ncacheRoot : " << set.getCacheRoot()
      << "\ndoc : " << set.getDepartment()
      << std::endl;
  std::cout
      << "\nue path: " << ue_set.Path()
      << "\nversipn: " << ue_set.Version()
      << "\nue shot start: " << ue_set.ShotStart()
      << "\nue shot end: " << ue_set.ShotEnd()
      << std::endl;
}

TEST_F(CoreTest, archive_polymorphism) {
  auto str_stream = std::stringstream{};

  auto k_m                 = std::make_shared<doodle::Project>("D:/", "测试1");
  doodle::MetadataPtr k_m2 = std::make_shared<doodle::Project>("D:/", "测试2");

  auto str_stream_bin = std::stringstream{};
  {
    cereal::JSONOutputArchive json{str_stream};
    json(cereal::make_nvp("metadata1", k_m),
         cereal::make_nvp("metadata12", k_m2));
    // cereal::BinaryOutputArchive binary{std::cout};
    cereal::BinaryOutputArchive binary2{str_stream_bin};
    binary2(cereal::make_nvp("metadata1", k_m),
            cereal::make_nvp("metadata12", k_m2));
  }

  std::cout << str_stream.str() << std::endl;

  {
    doodle::ProjectPtr k1;
    doodle::ProjectPtr k2;

    cereal::JSONInputArchive json{str_stream};
    json(k1, k2);
    cereal::BinaryInputArchive binary{str_stream_bin};
    binary(k1, k2);
  }
}

TEST_F(CoreTest, create_meatdata) {
  using namespace doodle;
  std::string k_test_root{};
  auto k_f = std::make_shared<MetadataFactory>();
  {  //创建项目各各种标签
    auto ptj = std::make_shared<Project>("D:/", "test_23333");
    ptj->updata_db(k_f);

    // auto ptj = set.GetMetadataSet().Project_();
    ASSERT_TRUE(ptj->getMetadataFactory() == k_f);
    CoreSet::getSet().GetMetadataSet().installProject(ptj);
    CoreSet::getSet().GetMetadataSet().setProject_(ptj);

    for (auto i = 1; i <= 10; ++i) {
      switch (i) {
        case 1: {
          auto k_ass = std::make_shared<Assets>(ptj, "tset");
          ptj->addChildItem(k_ass);
          k_ass->updata_db(k_f);
          ASSERT_TRUE(k_ass->getMetadataFactory() == k_f);
          k_ass = std::make_shared<Assets>(ptj, "test_m_parent");
          ptj->addChildItem(k_ass);
          k_ass->updata_db(k_f);
          ASSERT_TRUE(k_ass->getMetadataFactory() == k_f);
          ASSERT_TRUE(k_ass->getParent() == ptj);
          k_test_root = k_ass->getUUID();

          auto k_ass_file = std::make_shared<AssetsFile>(k_ass,
                                                         "tset",
                                                         "测试");
          k_ass->addChildItem(k_ass_file);
          k_ass_file->updata_db(k_f);
          ASSERT_TRUE(k_ass_file->getMetadataFactory() == k_f);
          continue;
        } break;

        default: {
          auto eps = std::make_shared<Episodes>(ptj, i);
          ptj->addChildItem(eps);
          eps->updata_db(k_f);
          for (auto x = 1; x < 30; ++x) {
            auto shot = std::make_shared<Shot>(eps, x);
            eps->addChildItem(shot);
            shot->updata_db(k_f);
            if (i % 5 == 0) {
              shot->setShotAb(Shot::ShotAbEnum::A);
            }
            ASSERT_TRUE(shot->getMetadataFactory() == k_f);
          }
        } break;
      }
    }
    ptj->updata_db(k_f);
    set.writeDoodleLocalSet();
  }
}
TEST_F(CoreTest, load_meatdata) {
  using namespace doodle;
  auto k_f = std::make_shared<MetadataFactory>();
  //加载文件
  auto ptj = set.GetMetadataSet().Project_();
  ptj->select_indb(k_f);
  std::cout << ptj->showStr() << std::endl;
  ASSERT_TRUE(ptj->getMetadataFactory() == k_f);
  std::cout << ptj->getChildItems().size() << std::endl;

  for (const auto& it : ptj->getChildItems()) {
    std::cout << std::setw(4) << "|->" << it->showStr() << std::endl;
    it->select_indb(k_f);
    ASSERT_TRUE(it->getMetadataFactory() == k_f);

    for (const auto& it1 : it->getChildItems()) {
      std::cout << std::setw(7) << "|->" << it1->showStr() << std::endl;
    }
  }
  auto& k_c  = ptj->getChildItems();
  auto it_tc = std::find_if(k_c.begin(), k_c.end(),
                            [](const MetadataPtr& ptr) {
                              return ptr->str() == "test_m_parent";
                            });
  ASSERT_TRUE(it_tc != k_c.end());
  auto it_tp = std::find_if(k_c.begin(), k_c.end(),
                            [](const MetadataPtr& ptr) {
                              return ptr->str() == "ep0010";
                            });
  ASSERT_TRUE(it_tp != k_c.end());
  ASSERT_TRUE((*it_tc)->getParent() == ptj);
  ASSERT_TRUE((*it_tc)->getMetadataFactory() == k_f);
  auto tc = *it_tc;
  auto tp = *it_tp;
  tp->addChildItem(tc);
  tc->updata_db(k_f);
  ASSERT_TRUE(tc->getParent() == tp);
}
TEST_F(CoreTest, modify_meatdata) {
  using namespace doodle;
  auto k_f = std::make_shared<MetadataFactory>();
  //加载文件
  auto ptj = set.GetMetadataSet().Project_();
  ptj->select_indb(k_f);
  std::cout << ptj->showStr() << std::endl;
  ASSERT_TRUE(ptj->getMetadataFactory() == k_f);
  std::cout << ptj->getChildItems().size() << std::endl;

  for (const auto& it : ptj->getChildItems()) {
    std::cout << std::setw(4) << "|->" << it->showStr() << std::endl;
    it->select_indb(k_f);
    ASSERT_TRUE(it->getMetadataFactory() == k_f);
    it->sortChildItems();
    for (const auto& it1 : it->getChildItems()) {
      it1->select_indb(k_f);
      std::cout << std::setw(7) << "|->" << it1->showStr() << std::endl;
      for (const auto& it2 : it1->getChildItems()) {
        it2->select_indb(k_f);
        std::cout << std::setw(10) << "|->" << it2->showStr() << std::endl;
      }
    }
  }
  auto& k_c  = ptj->getChildItems();
  auto it_tp = std::find_if(k_c.begin(), k_c.end(),
                            [](const MetadataPtr& ptr) {
                              return ptr->str() == "ep0010";
                            });
  ASSERT_TRUE(it_tp != k_c.end());
  auto& k_c1 = (*it_tp)->getChildItems();
  auto it_tc = std::find_if(k_c1.begin(), k_c1.end(),
                            [](const MetadataPtr& ptr) {
                              return ptr->str() == "test_m_parent";
                            });
  ASSERT_TRUE(it_tc != k_c1.end());
  ASSERT_TRUE((*it_tc)->getParent() != ptj);
  ASSERT_TRUE((*it_tc)->getParent() == *it_tp);
}
TEST_F(CoreTest, loadUe4ProjectFile) {
  ASSERT_TRUE(doodle::FSys::exists(p_ue4_path));

  doodle::FSys::ifstream file{p_ue4_path};
  auto str_stream = std::stringstream{};
  auto ijson      = nlohmann::json::parse(file);

  // auto ueFile = ijson.get<doodle::Ue4ProjectFile>();
  // ueFile.Plugins.push_back(doodle::Ue4ProjectFilePulgins{"doodle", true});
  // nlohmann::json root = ueFile;
  // // json(ueFile);
  // std::cout
  //     << ijson
  //     << std::endl
  //     << root
  //     << std::endl;
}

TEST_F(CoreTest, export_maya) {
  ASSERT_TRUE(doodle::FSys::exists(p_maya_path));
  auto mayafile = doodle::MayaFile{};
  mayafile.exportFbxFile(p_maya_path);
}

TEST_F(CoreTest, make_vide) {
  ASSERT_TRUE(doodle::FSys::exists(p_image_path));
  auto video = doodle::ImageSequence{p_image_path, {"test_哈哈"}};
  video.createVideoFile(p_video_path_out1);
}

TEST_F(CoreTest, connect_video) {
  ASSERT_TRUE(doodle::FSys::exists(p_video_path));
  auto videos = std::vector<doodle::FSys::path>{};
  for (auto v : doodle::FSys::directory_iterator(p_video_path)) {
    videos.emplace_back(v.path());
  }

  auto video = doodle::VideoSequence{videos};
  video.connectVideo(p_video_path_out2);
}

TEST_F(CoreTest, read_writ_file) {
  ASSERT_TRUE(doodle::FSys::exists(p_txt_path));
  doodle::FSys::fstream file{p_txt_path, std::ios::in};
  std::string line{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};

  static std::string str{R"("%ENGINEVERSIONAGNOSTICUSERDIR%DerivedDataCache")"};
  auto it = line.find(str);
  while (it != std::string::npos) {
    // std::cout << line << std::endl;
    line.replace(it, str.size(), R"("%GAMEDIR%DerivedDataCache")");
    // std::cout << line << std::endl;
    it = line.find(str);
  }
  file.close();
  file.open(p_txt_path, std::ios::out | std::ios::trunc);
  file << line;
}
TEST_F(CoreTest, long_path) {
//  using namespace doodle;
//  ASSERT_TRUE(FSys::exists(p_long_path));
//
//  auto size            = FSys::file_size(p_long_path);
//  auto last_write_time = FSys::con_time(FSys::last_write_time(p_long_path));
//
//  FSys::fstream file{p_long_path};
//  std::cout << "file size " << size << "\n"
//            << "lase write time " << last_write_time << "\n"
//            << (file.is_open() ? "true" : "false") << std::endl;
}
