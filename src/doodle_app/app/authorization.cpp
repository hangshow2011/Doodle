//
// Created by TD on 2022/3/23.
//

#include "authorization.h"

#include "doodle_core/core/core_set.h"
#include "doodle_core/metadata/time_point_wrap.h"
#include <doodle_core/logger/logger.h>

#include "cryptopp/files.h"
#include "cryptopp/hex.h"
#include <cryptopp/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/gcm.h>

namespace doodle {
class authorization::impl {
  friend void to_json(nlohmann::json& j, const impl& p) { j["time"] = p.l_time; }
  friend void from_json(const nlohmann::json& j, impl& p) { p.l_time = j.at("time").get<time_point_wrap>(); }

 public:
  time_point_wrap l_time{2020, 1, 1};
  std::string ciphertext_data{};
};

bool authorization::is_build_near() {
  /// 优先检查构建时间
  chrono::sys_seconds l_build_time_;
  std::istringstream l_time{version::build_info::get().build_time};
  l_time >> chrono::parse("%Y %m %d %H %M %S", l_build_time_);
  chrono::sys_time_pos l_point{l_build_time_};
  l_point += chrono::months{3};
  if (chrono::system_clock::now() < l_point) {
    p_i->l_time = l_point;
    return true;
  }
  return false;
}

void authorization::load_authorization_data(const std::string& in_data) {
  DOODLE_LOG_INFO("开始检查授权文件内容");
  std::string ciphertext{in_data};
  std::string decryptedtext{};

  {
    CryptoPP::GCM<CryptoPP::AES>::Decryption l_decryption{};
    l_decryption.SetKeyWithIV(
        doodle_config::cryptopp_key.data(), CryptoPP::AES::DEFAULT_KEYLENGTH, doodle_config::cryptopp_iv.data(),
        CryptoPP::AES::BLOCKSIZE
    );

    const std::string& enc = ciphertext.substr(0, ciphertext.length() - doodle_config::cryptopp_iv.size());
    const std::string& mac = ciphertext.substr(ciphertext.length() - doodle_config::cryptopp_iv.size());

    DOODLE_CHICK(ciphertext.size() == enc.size() + mac.size(), doodle_error{"授权码解码失误"});
    DOODLE_CHICK(doodle_config::cryptopp_iv.size() == mac.size(), doodle_error{"授权码解码失误"});

    CryptoPP::AuthenticatedDecryptionFilter df{
        l_decryption, new CryptoPP::StringSink{decryptedtext},
        CryptoPP::AuthenticatedDecryptionFilter::MAC_AT_BEGIN |
            CryptoPP::AuthenticatedDecryptionFilter::THROW_EXCEPTION,
        doodle_config::cryptopp_iv.size()};

    df.ChannelPut(CryptoPP::DEFAULT_CHANNEL, (const CryptoPP::byte*)mac.data(), mac.size());
    df.ChannelPut(
        CryptoPP::AAD_CHANNEL, (const CryptoPP::byte*)doodle_config::authorization_data.data(),
        doodle_config::authorization_data.size()
    );
    df.ChannelPut(CryptoPP::DEFAULT_CHANNEL, (const CryptoPP::byte*)enc.data(), enc.size());

    df.ChannelMessageEnd(CryptoPP::AAD_CHANNEL);
    df.ChannelMessageEnd(CryptoPP::DEFAULT_CHANNEL);
  }
  *p_i                 = nlohmann::json::parse(decryptedtext).get<impl>();
  p_i->ciphertext_data = std::move(ciphertext);
}

void authorization::load_authorization_data(std::istream& in_path) {
  DOODLE_LOG_INFO("开始检查授权文件内容");

  std::string decryptedtext{};

  {
    CryptoPP::GCM<CryptoPP::AES>::Decryption l_decryption{};
    l_decryption.SetKeyWithIV(
        doodle_config::cryptopp_key.data(), doodle_config::cryptopp_key.size(), doodle_config::cryptopp_iv.data(),
        doodle_config::cryptopp_iv.size()
    );

    CryptoPP::FileSource l_file{
        in_path, true,
        new CryptoPP::HexDecoder{new CryptoPP::AuthenticatedDecryptionFilter{
            l_decryption, new CryptoPP::StringSink{decryptedtext},
            CryptoPP::AuthenticatedDecryptionFilter::DEFAULT_FLAGS, doodle_config::cryptopp_tag_size}}};
    //    l_file.Flush(true);
  }
  *p_i = nlohmann::json::parse(decryptedtext).get<impl>();
}

authorization::authorization(const std::string& in_data) : p_i(std::make_unique<impl>()) {
  load_authorization_data(in_data);
}

authorization::authorization() : p_i(std::make_unique<impl>()) {
  auto l_p  = core_set::get_set().program_location() / doodle_config::token_name.data();
  auto l_p1 = core_set::get_set().get_doc() / doodle_config::token_name.data();

  if (FSys::exists(l_p) && FSys::exists(l_p1))
    l_p = FSys::last_write_time_point(l_p) < FSys::last_write_time_point(l_p1) ? l_p1 : l_p;
  else
    l_p = FSys::exists(l_p) ? l_p : l_p1;

  if (is_build_near()) {
    DOODLE_LOG_INFO("近期构建不检查授权内容");
    return;
  }

  if (FSys::exists(l_p)) {
    FSys::ifstream ifstream{l_p, std::ifstream::binary};
    //    std::string l_s{std::istream_iterator<char>{ifstream}, std::istream_iterator<char>{}};
    load_authorization_data(ifstream);
    //    load_authorization_data(l_s);
  }
}
authorization::~authorization() { save(); }
bool authorization::is_expire() const { return p_i->l_time > time_point_wrap::now(); }
void authorization::generate_token(const FSys::path& in_path) {
  DOODLE_CHICK(!in_path.empty(), doodle_error{"传入路径为空"});
  impl l_impl{};
  l_impl.l_time           = time_point_wrap(chrono::system_clock::now() + chrono::months{3});
  nlohmann::json out_json = l_impl;

  {
    FSys::ofstream l_f_s{
        in_path / FSys::path{doodle_config::token_name.data()},
        std::ofstream::binary | std::ofstream::out | std::ofstream::trunc};

    CryptoPP::GCM<CryptoPP::AES>::Encryption aes_Encryption{};
    aes_Encryption.SetKeyWithIV(
        doodle_config::cryptopp_key.data(), doodle_config::cryptopp_key.size(), doodle_config::cryptopp_iv.data(),
        doodle_config::cryptopp_iv.size()
    );
    CryptoPP::StringSource l_string_source{
        out_json.dump(), true,
        new CryptoPP::AuthenticatedEncryptionFilter{
            aes_Encryption, new CryptoPP::HexEncoder{new CryptoPP::FileSink{l_f_s}}, false,
            doodle_config::cryptopp_tag_size}};
  }
}
void authorization::save(const FSys::path& in_path) const {
  if (p_i->ciphertext_data.empty()) return;
  if (!exists(in_path.parent_path())) create_directories(in_path.parent_path());
  FSys::ofstream{in_path, std::ofstream::binary | std::ofstream::out | std::ofstream::trunc} << p_i->ciphertext_data;
}
void authorization::save() const { save(core_set::get_set().get_doc() / FSys::path{doodle_config::token_name.data()}); }

}  // namespace doodle
