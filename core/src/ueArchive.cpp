#include "ueArchive.h"

#include "filesqlinfo.h"
#include "freeSynWrap.h"
#include "coreset.h"
CORE_NAMESPACE_S
ueArchive::ueArchive(doCore::fileSqlInfoPtr data)
    : p_info_(std::move(data)),
      p_syn(std::make_shared<freeSynWrap>()) {

}
void ueArchive::insertDB() {
  dpathList list = p_Path;
  list.push_back(p_Path.front().parent_path() / DOODLE_CONTENT);
  p_info_->setFileList(list);
  p_info_->insert();
}
void ueArchive::_generateFilePath() {
  if (!p_soureFile.empty()) {
    p_Path.push_back(
        p_info_->generatePath(
            "Scenefiles",
            boost::filesystem::extension(p_soureFile.front())
        )
    );
  } else if (!p_info_->getFileList().empty())
    for (const auto &item : p_info_->getFileList())
      p_Path.push_back(item);
//
//  if (p_soureFile.size() == 1)
//      p_soureFile.push_back(p_soureFile.front().parent_path() / "Content");
}
void ueArchive::_updata(const dpathList &pathList) {
  assert(p_Path.size() == p_cacheFilePath.size());
  fileArchive::_updata({pathList.front()});
  synPath_struct syn_path_struct{};
  syn_path_struct.local = p_soureFile.front().parent_path() /DOODLE_CONTENT;
  syn_path_struct.server = p_Path.front().parent_path() /DOODLE_CONTENT;
  p_syn->addSynFile({syn_path_struct});
  p_syn->setVersioningFolder(freeSynWrap::syn_set::upload, p_Path.front().parent_path() / DOODLE_BACKUP);
  p_syn->run();
}
void ueArchive::_down(const dpath &localPath) {
  synPath_struct syn_path_struct{};
  syn_path_struct.server = p_Path.back();
  syn_path_struct.local = localPath.back()/DOODLE_CONTENT;
  p_syn->addSynFile({syn_path_struct});
  p_syn->setVersioningFolder(freeSynWrap::syn_set::down,
                             p_Path.front().parent_path() / DOODLE_BACKUP);
  p_syn->run();
}
CORE_NAMESPACE_E

