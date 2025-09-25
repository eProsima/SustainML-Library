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
 * @file Database.hpp
 */

#ifndef SUSTAINMLCPP_DATABASE_DATABASE_HPP
#define SUSTAINMLCPP_DATABASE_DATABASE_HPP

#include <sqlite3.h>
#include <string>
#include <vector>

namespace sustainml {
namespace database {

/**
 * @brief RAII wrapper around an SQLite3 database.
 *
 * Manages opening, closing and basic schema operations for a local SQLite
 * database file used to persist task metadata and JSON payloads for the
 * SustainML application.
 */
class Database
{
public:

    struct Row
    {
        long long problem_id{0};
        long long iteration_id{0};
        std::string user_input_json;
        std::string app_requirements_json;
        std::string ml_model_metadata_json;
        std::string ml_model_json;
        std::string hw_constraints_json;
        std::string hw_resources_json;
        std::string carbon_footprint_json;
    };

    /**
     * @brief Construct with the database file path.
     *
     * Does not open the database. Default: "sustainml.db".
     */
    explicit Database(const std::string& db_file = "sustainml.db");

    /**
     * @brief Destructor closes the database if open.
     */
    ~Database();

    /**
     * @brief Open or create the SQLite database and perform setup.
     *
     * @return SQLITE_OK on success or an SQLite error code on failure.
     */
    int initialize();

    /**
     * @brief Close the SQLite database if open.
     *
     * @return SQLITE_OK on success or an SQLite error code on failure.
     */
    int close();

    /**
     * @brief Execute a SQL statement that does not return rows.
     *
     * @return SQLite result code.
     */
    int exec(const std::string& sql);

    /**
     * @brief Ensure required tables and indices exist.
     *
     * @return SQLite result code.
     */
    int create_schema();

    /**
     * @brief Replace all rows in the database with the provided set of rows.
     *
     * @param rows Vector of rows to insert/replace.
     * @return SQLite result code.
     */
    int replace_all_rows(const std::vector<Row>& rows);

    /**
     * @brief Load all rows from the database into the provided vector.
     *
     * @param out_rows Vector to fill with loaded rows.
     * @return SQLite result code.
     */
    int read_all_rows(std::vector<Row>& out_rows);


    /**
     * @brief Access the raw sqlite3* handle (or nullptr if closed).
     */
    sqlite3* getDB() const;

private:
    std::string db_file_;
    sqlite3* db_;
    bool created_on_open_{false};
};

} // namespace database
} // namespace sustainml

#endif // SUSTAINMLCPP_DATABASE_DATABASE_HPP
