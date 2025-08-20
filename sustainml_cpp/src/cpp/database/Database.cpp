// Copyright 2025 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file Database.cpp
 */

#include <sustainml_cpp/database/Database.hpp>
#include <iostream>
#include <fstream>

namespace sustainml {
namespace database {

Database::Database(const std::string& db_file)
    : db_file_(db_file)
    , db_(nullptr)
{
}


Database::~Database()
{
    close();
}


int Database::close()
{
    if (!db_)
    {
        return SQLITE_OK;
    }
    int rc = sqlite3_close(db_);
    if (rc == SQLITE_OK)
    {
        db_ = nullptr;
    }
    return rc;
}


int Database::initialize()
{
    if (db_)
    {
        return SQLITE_OK;
    }

    auto file_exists = [](const std::string& p) -> bool {
        std::ifstream f(p);
        return f.good();
    };
    created_on_open_ = !file_exists(db_file_);

    int rc = sqlite3_open(db_file_.c_str(), &db_);
    if (rc != SQLITE_OK)
    {
        std::cerr << "sqlite3_open failed: " << rc << "\n";
        return rc;
    }

    // WIP - If the database already exist we should load its data to the backend
    // WIP - If the database doesn't exist it would be just opening it and created the schema with the raw style of the data
    // WIP - Apart from opening the database nothing more solve be done in the initialize. The load is callable method by the user
    if (created_on_open_)
    {
        rc = create_schema();
        if (rc != SQLITE_OK)
        {
            std::cerr << "create_schema failed: " << rc << "\n";
            return rc;
        }
    }
    // ------------------------- debug
    // else
    // {
    //     std::string db_json;
    //     rc = load(db_json);
    //     if (rc != SQLITE_OK)
    //     {
    //         std::cerr << "load failed: " << rc << "\n";
    //         return rc;
    //     }


    //     std::cout << "db from internal db: " << std::endl;
    //     dump_to_stdout();   // show actual data from db
    //     std::cout << "db from json loaded: " << std::endl;
    //     std::cout << "db_json (" << db_json.size() << " bytes):\n" << db_json << "\n" << std::endl;

    //     std::string new_json = R"JSON([
    //         {"id":1,"problem_id":1,"iteration_id":1,"UserInputJson":{"user":"demo_updated"},"AppRequirementsJson":{"latency_ms":120},"MLModelMetadataJson":{"version":"0.2"},"MLModelJson":{"name":"ModelA","params":{"alpha":0.1}},"HWConstraintsJson":{"max_power_w":45},"HWResourcesJson":{"cpu":"4c","mem":"2GB"},"CarbonFootprintJson":{"co2_g":11.0},"created_at":"2025-08-19 10:15:27"},
    //         {"id":2,"problem_id":1,"iteration_id":2,"UserInputJson":{"user":"tester"},"AppRequirementsJson":{"latency_ms":80},"MLModelMetadataJson":{"version":"1.0"},"MLModelJson":{"name":"ModelB","params":{"beta":1}},"HWConstraintsJson":{"max_power_w":30},"HWResourcesJson":{"cpu":"2c","mem":"1GB"},"CarbonFootprintJson":{"co2_g":5.5},"created_at":"2025-08-19 11:00:00"}
    //     ])JSON";

    //     int rc = save(new_json);
    //     if (rc != SQLITE_OK)
    //     {
    //             std::cerr << "save failed: " << rc << "\n";
    //             return rc;
    //     }
    //     std::cout << "Saved new JSON (" << new_json.size() << " bytes)\n";

    //     // Show DB rows after saving
    //     std::cout << "Database content after save:\n";
    //     dump_to_stdout();

    //     std::string reloaded_json;
    //     rc = load(reloaded_json);
    //     if (rc != SQLITE_OK)
    //     {
    //             std::cerr << "load after save failed: " << rc << "\n";
    //             return rc;
    //     }
    //     std::cout << "Reloaded JSON (" << reloaded_json.size() << " bytes):\n" << reloaded_json << "\n";
    // }
    // ------------------------- debug

    return SQLITE_OK;
}


int Database::exec(const std::string& sql)
{
    if (!db_)
    {
        return SQLITE_MISUSE;
    }

    char* errmsg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errmsg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "sqlite3_exec error (" << rc << "): "
                  << (errmsg ? errmsg : "unknown") << "\n";
        if (errmsg) sqlite3_free(errmsg);
    }
    return rc;
}


int Database::create_schema()
{
    const char* ddl =
        "CREATE TABLE IF NOT EXISTS task_results ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  problem_id INTEGER NOT NULL,"
        "  iteration_id INTEGER NOT NULL,"
        "  UserInputJson TEXT NOT NULL,"
        "  AppRequirementsJson TEXT NOT NULL,"
        "  MLModelMetadataJson TEXT NOT NULL,"
        "  MLModelJson TEXT NOT NULL,"
        "  HWConstraintsJson TEXT NOT NULL,"
        "  HWResourcesJson TEXT NOT NULL,"
        "  CarbonFootprintJson TEXT NOT NULL,"
        "  created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "  CHECK (typeof(UserInputJson)='text'),"
        "  CHECK (typeof(AppRequirementsJson)='text'),"
        "  CHECK (typeof(MLModelMetadataJson)='text'),"
        "  CHECK (typeof(MLModelJson)='text'),"
        "  CHECK (typeof(HWConstraintsJson)='text'),"
        "  CHECK (typeof(HWResourcesJson)='text'),"
        "  CHECK (typeof(CarbonFootprintJson)='text')"
    #ifdef SQLITE_ENABLE_JSON1
        "  , CHECK (json_valid(UserInputJson))"
        "  , CHECK (json_valid(AppRequirementsJson))"
        "  , CHECK (json_valid(MLModelMetadataJson))"
        "  , CHECK (json_valid(MLModelJson))"
        "  , CHECK (json_valid(HWConstraintsJson))"
        "  , CHECK (json_valid(HWResourcesJson))"
        "  , CHECK (json_valid(CarbonFootprintJson))"
    #endif
        "  , UNIQUE(problem_id, iteration_id)"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_task_results_problem_iter "
        "ON task_results(problem_id, iteration_id);";

    int rc = exec(ddl);
    if (rc != SQLITE_OK)
    {
        return rc;
    }

    // Test seed: Insert a couple of rows to ensure everything works.
    // #ifdef SQLITE_ENABLE_JSON1
    // const char* seed_sql =
    //     "BEGIN;"
    //     "INSERT OR IGNORE INTO task_results "
    //     "(problem_id, iteration_id, UserInputJson, AppRequirementsJson, "
    //     " MLModelMetadataJson, MLModelJson, HWConstraintsJson, HWResourcesJson, CarbonFootprintJson) "
    //     "VALUES "
    //     "(1, 1, "
    //     " json('{\"user\":\"demo\"}'),"
    //     " json('{\"latency_ms\":100}'),"
    //     " json('{\"version\":\"0.1\"}'),"
    //     " json('{\"name\":\"ModelA\",\"params\":{}}'),"
    //     " json('{\"max_power_w\":50}'),"
    //     " json('{\"cpu\":\"4c\",\"mem\":\"2GB\"}'),"
    //     " json('{\"co2_g\":12.3}')"
    //     ");"
    //     "COMMIT;";
    // #else
    // const char* seed_sql =
    //     "BEGIN;"
    //     "INSERT OR IGNORE INTO task_results "
    //     "(problem_id, iteration_id, UserInputJson, AppRequirementsJson, "
    //     " MLModelMetadataJson, MLModelJson, HWConstraintsJson, HWResourcesJson, CarbonFootprintJson) "
    //     "VALUES "
    //     "(1, 1, "
    //     " '{\"user\":\"demo\"}',"
    //     " '{\"latency_ms\":100}',"
    //     " '{\"version\":\"0.1\"}',"
    //     " '{\"name\":\"ModelA\",\"params\":{}}',"
    //     " '{\"max_power_w\":50}',"
    //     " '{\"cpu\":\"4c\",\"mem\":\"2GB\"}',"
    //     " '{\"co2_g\":12.3}'"
    //     ");"
    //     "COMMIT;";
    // #endif

    // rc = exec(seed_sql);
    // if (rc != SQLITE_OK)
    // {
    //     std::cerr << "seed data insert failed: " << rc << "\n";
    //     return rc;
    // }

    return SQLITE_OK;
}


int Database::save(std::string& json_in)
{
    if (!db_)
    {
        return SQLITE_MISUSE;
    }

    int rc = exec("BEGIN;");
    if (rc != SQLITE_OK)
    {
        return rc;
    }

    rc = exec("DELETE FROM task_results;");
    if (rc != SQLITE_OK)
    {
        exec("ROLLBACK;");
        return rc;
    }

    const char* insert_sql =
        "INSERT INTO task_results ("
        "  id, problem_id, iteration_id, "
        "  UserInputJson, AppRequirementsJson, "
        "  MLModelMetadataJson, MLModelJson, "
        "  HWConstraintsJson, HWResourcesJson, "
        "  CarbonFootprintJson, created_at)"
        "SELECT "
        "  json_extract(value, '$.id'),"
        "  json_extract(value, '$.problem_id'),"
        "  json_extract(value, '$.iteration_id'),"
    #ifdef SQLITE_ENABLE_JSON1
        "  json(json_extract(value, '$.UserInputJson')),"
        "  json(json_extract(value, '$.AppRequirementsJson')),"
        "  json(json_extract(value, '$.MLModelMetadataJson')),"
        "  json(json_extract(value, '$.MLModelJson')),"
        "  json(json_extract(value, '$.HWConstraintsJson')),"
        "  json(json_extract(value, '$.HWResourcesJson')),"
        "  json(json_extract(value, '$.CarbonFootprintJson')),"
    #else
        "  json_extract(value, '$.UserInputJson'),"
        "  json_extract(value, '$.AppRequirementsJson'),"
        "  json_extract(value, '$.MLModelMetadataJson'),"
        "  json_extract(value, '$.MLModelJson'),"
        "  json_extract(value, '$.HWConstraintsJson'),"
        "  json_extract(value, '$.HWResourcesJson'),"
        "  json_extract(value, '$.CarbonFootprintJson'),"
    #endif
        "  COALESCE(json_extract(value, '$.created_at'), CURRENT_TIMESTAMP)"
        "FROM json_each(?) "
        "WHERE json_type(value) = 'object';";

    sqlite3_stmt* stmt = nullptr;
    rc = sqlite3_prepare_v2(db_, insert_sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "sqlite3_prepare_v2(save) failed: " << rc
                  << " msg=" << sqlite3_errmsg(db_) << "\n";
        exec("ROLLBACK;");
        return rc;
    }

    rc = sqlite3_bind_text(stmt, 1, json_in.c_str(),
                           static_cast<int>(json_in.size()), SQLITE_TRANSIENT);
    if (rc != SQLITE_OK)
    {
        std::cerr << "sqlite3_bind_text failed: " << rc << "\n";
        sqlite3_finalize(stmt);
        exec("ROLLBACK;");
        return rc;
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        std::cerr << "sqlite3_step(save) failed: " << rc
                  << " msg=" << sqlite3_errmsg(db_) << "\n";
        sqlite3_finalize(stmt);
        exec("ROLLBACK;");
        return rc;
    }
    sqlite3_finalize(stmt);

    rc = exec("COMMIT;");
    if (rc != SQLITE_OK)
    {
        exec("ROLLBACK;");
        return rc;
    }

    return SQLITE_OK;
}


int Database::load(std::string& json_out)
{
    if (!db_)
    {
        return SQLITE_MISUSE;
    }

    const char* q =
        "SELECT COALESCE(json_group_array(json_object("
        "  'id', id,"
        "  'problem_id', problem_id,"
        "  'iteration_id', iteration_id,"
        "  'UserInputJson', json(UserInputJson),"
        "  'AppRequirementsJson', json(AppRequirementsJson),"
        "  'MLModelMetadataJson', json(MLModelMetadataJson),"
        "  'MLModelJson', json(MLModelJson),"
        "  'HWConstraintsJson', json(HWConstraintsJson),"
        "  'HWResourcesJson', json(HWResourcesJson),"
        "  'CarbonFootprintJson', json(CarbonFootprintJson),"
        "  'created_at', created_at"
        ")), '[]') "
        "FROM task_results;";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, q, -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "sqlite3_prepare_v2 failed: " << rc
                  << " msg=" << sqlite3_errmsg(db_) << "\n";
        return rc;
    }

    int step_rc = sqlite3_step(stmt);
    if (step_rc == SQLITE_ROW)
    {
        const unsigned char* txt = sqlite3_column_text(stmt, 0);
        json_out = txt ? reinterpret_cast<const char*>(txt) : "[]";
        rc = SQLITE_OK;
    }
    else if (step_rc == SQLITE_DONE)
    {
        json_out = "[]";
        rc = SQLITE_OK;
    }
    else
    {
        std::cerr << "sqlite3_step json build failed: " << step_rc
                  << " msg=" << sqlite3_errmsg(db_) << "\n";
        rc = step_rc;
    }

    sqlite3_finalize(stmt);
    return rc;
}

// ---------------------------- debug
int Database::dump_to_stdout()
{
    if (!db_)
    {
        return SQLITE_MISUSE;
    }

    // For the new schema; JSON1 normalizes JSON columns
    const char* q =
        "SELECT "
        " id, problem_id, iteration_id, "
        " json(UserInputJson), "
        " json(AppRequirementsJson), "
        " json(MLModelMetadataJson), "
        " json(MLModelJson), "
        " json(HWConstraintsJson), "
        " json(HWResourcesJson), "
        " json(CarbonFootprintJson), "
        " created_at "
        "FROM task_results "
        "ORDER BY problem_id, iteration_id;";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, q, -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "sqlite3_prepare_v2 failed: " << rc
                  << " msg=" << sqlite3_errmsg(db_) << "\n";
        return rc;
    }

    bool any = false;
    int step_rc = SQLITE_OK;
    while ((step_rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        any = true;

        const auto txt = [&](int col) -> const char*
        {
            const unsigned char* v = sqlite3_column_text(stmt, col);
            return v ? reinterpret_cast<const char*>(v) : "(null)";
        };

        long long id          = static_cast<long long>(sqlite3_column_int64(stmt, 0));
        long long problem_id  = static_cast<long long>(sqlite3_column_int64(stmt, 1));
        long long iteration   = static_cast<long long>(sqlite3_column_int64(stmt, 2));
        const char* user_in   = txt(3);
        const char* app_req   = txt(4);
        const char* mdl_meta  = txt(5);
        const char* mdl_json  = txt(6);
        const char* hw_const  = txt(7);
        const char* hw_res    = txt(8);
        const char* co2       = txt(9);
        const char* created   = txt(10);

        std::cout
            << "id=" << id
            << " problem_id=" << problem_id
            << " iteration_id=" << iteration
            << " created_at=" << created
            << "\n  UserInputJson=" << user_in
            << "\n  AppRequirementsJson=" << app_req
            << "\n  MLModelMetadataJson=" << mdl_meta
            << "\n  MLModelJson=" << mdl_json
            << "\n  HWConstraintsJson=" << hw_const
            << "\n  HWResourcesJson=" << hw_res
            << "\n  CarbonFootprintJson=" << co2
            << "\n";
    }

    if (step_rc != SQLITE_DONE)
    {
        std::cerr << "sqlite3_step select failed: " << step_rc
                  << " msg=" << sqlite3_errmsg(db_) << "\n";
        sqlite3_finalize(stmt);
        return step_rc;
    }

    if (!any)
    {
        std::cout << "task_results: (empty)\n";
    }

    sqlite3_finalize(stmt);
    return SQLITE_OK;
}


sqlite3* Database::getDB() const
{
    return db_;
}

} // database
} // sustainml
