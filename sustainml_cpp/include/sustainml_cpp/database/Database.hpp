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

#include <sqlite/sqlite3.h>
#include <string>

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
     * @brief Save a JSON payload into the database.
     *
     * Database must be open.
     * @return SQLite result code.
     */
    int save(std::string& json_in);

    /**
     * @brief Load JSON payload(s) from the database into json_out.
     *
     * Database must be open.
     * @return SQLite result code.
     */
    int load(std::string& json_out);

    /**
     * @brief True if the database was created during initialize().
     */
    bool created_on_open() const { return created_on_open_; }

    /**
     * @brief Dump database contents to stdout (for debugging).
     *
     * @return SQLite result code.
     */
    int dump_to_stdout();

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
