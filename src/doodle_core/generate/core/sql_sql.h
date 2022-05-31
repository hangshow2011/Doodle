// generated by E:/vcpkg/installed/x64-windows/scripts/sqlpp11-ddl2cpp C:/Users/TD/Source/Doodle/src/doodle_core/core/sql_file.sql C:/Users/TD/Source/Doodle/src/doodle_core/generate/core/sql_sql doodle
#ifndef DOODLE_SQL_SQL_H
#define DOODLE_SQL_SQL_H

#include <sqlpp11/table.h>
#include <sqlpp11/data_types.h>
#include <sqlpp11/char_sequence.h>

namespace doodle
{
  namespace Context_
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
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::must_not_insert, sqlpp::tag::must_not_update, sqlpp::tag::can_be_null>;
    };
    struct ComHash
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "com_hash";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T comHash;
            T& operator()() { return comHash; }
            const T& operator()() const { return comHash; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct JsonData
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "json_data";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T jsonData;
            T& operator()() { return jsonData; }
            const T& operator()() const { return jsonData; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };
  } // namespace Context_

  struct Context: sqlpp::table_t<Context,
               Context_::Id,
               Context_::ComHash,
               Context_::JsonData>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "context";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T context;
        T& operator()() { return context; }
        const T& operator()() const { return context; }
      };
    };
  };
  namespace Entity_
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
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::must_not_insert, sqlpp::tag::must_not_update, sqlpp::tag::can_be_null>;
    };
    struct UuidData
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "uuid_data";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T uuidData;
            T& operator()() { return uuidData; }
            const T& operator()() const { return uuidData; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };
    struct UpdateTime
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "update_time";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T updateTime;
            T& operator()() { return updateTime; }
            const T& operator()() const { return updateTime; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::time_point, sqlpp::tag::can_be_null>;
    };
  } // namespace Entity_

  struct Entity: sqlpp::table_t<Entity,
               Entity_::Id,
               Entity_::UuidData,
               Entity_::UpdateTime>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "entity";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T entity;
        T& operator()() { return entity; }
        const T& operator()() const { return entity; }
      };
    };
  };
  namespace ComEntity_
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
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::must_not_insert, sqlpp::tag::must_not_update, sqlpp::tag::can_be_null>;
    };
    struct EntityId
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "entity_id";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T entityId;
            T& operator()() { return entityId; }
            const T& operator()() const { return entityId; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct ComHash
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "com_hash";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T comHash;
            T& operator()() { return comHash; }
            const T& operator()() const { return comHash; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct JsonData
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "json_data";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T jsonData;
            T& operator()() { return jsonData; }
            const T& operator()() const { return jsonData; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };
  } // namespace ComEntity_

  struct ComEntity: sqlpp::table_t<ComEntity,
               ComEntity_::Id,
               ComEntity_::EntityId,
               ComEntity_::ComHash,
               ComEntity_::JsonData>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "com_entity";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T comEntity;
        T& operator()() { return comEntity; }
        const T& operator()() const { return comEntity; }
      };
    };
  };
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
      using _traits = sqlpp::make_traits<sqlpp::integer_unsigned, sqlpp::tag::must_not_insert, sqlpp::tag::must_not_update, sqlpp::tag::can_be_null>;
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
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
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
