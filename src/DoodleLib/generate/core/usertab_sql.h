// generated by E:/vcpkg/installed/x64-windows/scripts/sqlpp11-ddl2cpp C:/Users/TD/Source/Doodle/src/DoodleLib/core/001-usertab.sql C:/Users/TD/Source/Doodle/src/DoodleLib/generate/core/usertab_sql doodle
#ifndef DOODLE_USERTAB_SQL_H
#define DOODLE_USERTAB_SQL_H

#include <sqlpp11/table.h>
#include <sqlpp11/data_types.h>
#include <sqlpp11/char_sequence.h>

namespace doodle
{
  namespace Usertab_
  {
    struct Id
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "id";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T id;
            T& operator()() { return id; }
            const T& operator()() const { return id; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::bigint_unsigned, sqlpp::tag::must_not_insert, sqlpp::tag::must_not_update, sqlpp::tag::can_be_null>;
    };
    struct UserName
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "user_name";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T userName;
            T& operator()() { return userName; }
            const T& operator()() const { return userName; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::require_insert>;
    };
    struct UuidPath
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "uuid_path";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T uuidPath;
            T& operator()() { return uuidPath; }
            const T& operator()() const { return uuidPath; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };
    struct PermissionGroup
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "permission_group";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T permissionGroup;
            T& operator()() { return permissionGroup; }
            const T& operator()() const { return permissionGroup; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::bigint>;
    };
  } // namespace Usertab_

  struct Usertab: sqlpp::table_t<Usertab,
               Usertab_::Id,
               Usertab_::UserName,
               Usertab_::UuidPath,
               Usertab_::PermissionGroup>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "usertab";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T usertab;
        T& operator()() { return usertab; }
        const T& operator()() const { return usertab; }
      };
    };
  };
} // namespace doodle
#endif
