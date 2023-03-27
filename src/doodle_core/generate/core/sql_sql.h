// generated by E:/Doodle/build/Ninja_debug/vcpkg_installed/x64-windows/scripts/sqlpp11-ddl2cpp E:/Doodle/src/doodle_core/core/sql_file.sql E:/Doodle/src/doodle_core/generate/core/sql_sql doodle_database
#ifndef DOODLE_DATABASE_SQL_SQL_H
#define DOODLE_DATABASE_SQL_SQL_H

#include <sqlpp11/table.h>
#include <sqlpp11/data_types.h>
#include <sqlpp11/char_sequence.h>

namespace doodle_database
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
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
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
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
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
  namespace DoodleInfo_
  {
    struct VersionMajor
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "version_major";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T versionMajor;
            T& operator()() { return versionMajor; }
            const T& operator()() const { return versionMajor; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::require_insert>;
    };
    struct VersionMinor
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "version_minor";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T versionMinor;
            T& operator()() { return versionMinor; }
            const T& operator()() const { return versionMinor; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::require_insert>;
    };
  } // namespace DoodleInfo_

  struct DoodleInfo: sqlpp::table_t<DoodleInfo,
               DoodleInfo_::VersionMajor,
               DoodleInfo_::VersionMinor>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "doodle_info";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T doodleInfo;
        T& operator()() { return doodleInfo; }
        const T& operator()() const { return doodleInfo; }
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
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
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
               Usertab_::EntityId,
               Usertab_::UserName,
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
  namespace WorkTaskInfo_
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
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
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
    struct UserId
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "user_id";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T userId;
            T& operator()() { return userId; }
            const T& operator()() const { return userId; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };
    struct TaskName
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "task_name";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T taskName;
            T& operator()() { return taskName; }
            const T& operator()() const { return taskName; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };
    struct Region
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "region";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T region;
            T& operator()() { return region; }
            const T& operator()() const { return region; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };
    struct Abstract
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "abstract";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T abstract;
            T& operator()() { return abstract; }
            const T& operator()() const { return abstract; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };
    struct TimePoint
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "time_point";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T timePoint;
            T& operator()() { return timePoint; }
            const T& operator()() const { return timePoint; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::time_point, sqlpp::tag::can_be_null>;
    };
  } // namespace WorkTaskInfo_

  struct WorkTaskInfo: sqlpp::table_t<WorkTaskInfo,
               WorkTaskInfo_::Id,
               WorkTaskInfo_::EntityId,
               WorkTaskInfo_::UserId,
               WorkTaskInfo_::TaskName,
               WorkTaskInfo_::Region,
               WorkTaskInfo_::Abstract,
               WorkTaskInfo_::TimePoint>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "work_task_info";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T workTaskInfo;
        T& operator()() { return workTaskInfo; }
        const T& operator()() const { return workTaskInfo; }
      };
    };
  };
  namespace Episodes_
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
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
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
    struct Eps
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "eps";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T eps;
            T& operator()() { return eps; }
            const T& operator()() const { return eps; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
  } // namespace Episodes_

  struct Episodes: sqlpp::table_t<Episodes,
               Episodes_::Id,
               Episodes_::EntityId,
               Episodes_::Eps>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "episodes";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T episodes;
        T& operator()() { return episodes; }
        const T& operator()() const { return episodes; }
      };
    };
  };
  namespace Shot_
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
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
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
    struct ShotInt
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "shot_int";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T shotInt;
            T& operator()() { return shotInt; }
            const T& operator()() const { return shotInt; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct ShotAb
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "shot_ab";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T shotAb;
            T& operator()() { return shotAb; }
            const T& operator()() const { return shotAb; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };
  } // namespace Shot_

  struct Shot: sqlpp::table_t<Shot,
               Shot_::Id,
               Shot_::EntityId,
               Shot_::ShotInt,
               Shot_::ShotAb>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "shot";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T shot;
        T& operator()() { return shot; }
        const T& operator()() const { return shot; }
      };
    };
  };
  namespace RedirectionPathInfo_
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
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
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
    struct RedirectionPath
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "redirection_path";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T redirectionPath;
            T& operator()() { return redirectionPath; }
            const T& operator()() const { return redirectionPath; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };
    struct RedirectionFileName
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "redirection_file_name";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T redirectionFileName;
            T& operator()() { return redirectionFileName; }
            const T& operator()() const { return redirectionFileName; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };
  } // namespace RedirectionPathInfo_

  struct RedirectionPathInfo: sqlpp::table_t<RedirectionPathInfo,
               RedirectionPathInfo_::Id,
               RedirectionPathInfo_::EntityId,
               RedirectionPathInfo_::RedirectionPath,
               RedirectionPathInfo_::RedirectionFileName>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "redirection_path_info";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T redirectionPathInfo;
        T& operator()() { return redirectionPathInfo; }
        const T& operator()() const { return redirectionPathInfo; }
      };
    };
  };
  namespace Assets_
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
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
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
    struct AssetsPath
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "assets_path";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T assetsPath;
            T& operator()() { return assetsPath; }
            const T& operator()() const { return assetsPath; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };
  } // namespace Assets_

  struct Assets: sqlpp::table_t<Assets,
               Assets_::Id,
               Assets_::EntityId,
               Assets_::AssetsPath>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "assets";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T assets;
        T& operator()() { return assets; }
        const T& operator()() const { return assets; }
      };
    };
  };
  namespace Comment_
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
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
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
    struct CommentString
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "comment_string";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T commentString;
            T& operator()() { return commentString; }
            const T& operator()() const { return commentString; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };
    struct CommentTime
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "comment_time";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T commentTime;
            T& operator()() { return commentTime; }
            const T& operator()() const { return commentTime; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };
  } // namespace Comment_

  struct Comment: sqlpp::table_t<Comment,
               Comment_::Id,
               Comment_::EntityId,
               Comment_::CommentString,
               Comment_::CommentTime>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "comment";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T comment;
        T& operator()() { return comment; }
        const T& operator()() const { return comment; }
      };
    };
  };
  namespace ExportFileInfo_
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
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
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
    struct FilePath
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "file_path";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T filePath;
            T& operator()() { return filePath; }
            const T& operator()() const { return filePath; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };
    struct StartFrame
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "start_frame";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T startFrame;
            T& operator()() { return startFrame; }
            const T& operator()() const { return startFrame; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct EndFrame
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "end_frame";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T endFrame;
            T& operator()() { return endFrame; }
            const T& operator()() const { return endFrame; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct RefFile
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "ref_file";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T refFile;
            T& operator()() { return refFile; }
            const T& operator()() const { return refFile; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };
    struct ExportType_
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "export_type_";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T exportType_;
            T& operator()() { return exportType_; }
            const T& operator()() const { return exportType_; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };
  } // namespace ExportFileInfo_

  struct ExportFileInfo: sqlpp::table_t<ExportFileInfo,
               ExportFileInfo_::Id,
               ExportFileInfo_::EntityId,
               ExportFileInfo_::FilePath,
               ExportFileInfo_::StartFrame,
               ExportFileInfo_::EndFrame,
               ExportFileInfo_::RefFile,
               ExportFileInfo_::ExportType_>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "export_file_info";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T exportFileInfo;
        T& operator()() { return exportFileInfo; }
        const T& operator()() const { return exportFileInfo; }
      };
    };
  };
  namespace ImageIcon_
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
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
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
    struct Path
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "path";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T path;
            T& operator()() { return path; }
            const T& operator()() const { return path; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };
  } // namespace ImageIcon_

  struct ImageIcon: sqlpp::table_t<ImageIcon,
               ImageIcon_::Id,
               ImageIcon_::EntityId,
               ImageIcon_::Path>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "image_icon";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T imageIcon;
        T& operator()() { return imageIcon; }
        const T& operator()() const { return imageIcon; }
      };
    };
  };
} // namespace doodle_database
#endif
