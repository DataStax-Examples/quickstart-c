# DataStax C/C++ Driver for Apache Cassandra Quickstart
[![Build Status](https://travis-ci.org/beccam/quickstart-c.svg?branch=master)](https://travis-ci.org/beccam/quickstart-c)

A basic demo CRUD application using the DataStax C/C++ Driver for Apache Cassandra. 
The intent is to help users get up and running quickly with the driver. 
If you are having trouble, the complete code solution for `quickstart.c` can be found [here](https://gist.github.com/beccam/bcb2fdb81db931abb0d175ba0356c1fc).

## Prerequisites
  * A running instance of [Apache Cassandra®](http://cassandra.apache.org/download/) 1.2+
  * [CMake](http://www.cmake.org/download) v2.6.4+
  * [libuv](http://libuv.org/) 1.x
  * [OpenSSL](https://www.openssl.org/) v1.0.x or v1.1.x

## Create the keyspace and table
The `users.cql` file provides the schema used for this project:

```sql
CREATE KEYSPACE demo
    WITH replication = {'class': 'SimpleStrategy', 'replication_factor': '1'};

CREATE TABLE demo.users (
    lastname text PRIMARY KEY,
    age int,
    city text,
    email text,
    firstname text);
```
## Getting the Driver
You can obtain binary versions of the driver, available for various operating systems and architectures, from our [DataStax download server](https://downloads.datastax.com/cpp-driver/).The driver source code is available via [GitHub](https://github.com/datastax/cpp-driver).The C/C++ driver will build on most standard Unix-like and Microsoft Windows platforms. Packages are available for the following platforms:

* [CentOS 6](https://downloads.datastax.com/cpp-driver/centos/6/cassandra/)
* [CentOS 7](https://downloads.datastax.com/cpp-driver/centos/7/cassandra/)
* [Ubuntu 14.04 LTS](https://downloads.datastax.com/cpp-driver//ubuntu/14.04/cassandra)
* [Ubuntu 16.04 LTS](https://downloads.datastax.com/cpp-driver//ubuntu/16.04/cassandra)
* [Ubuntu 18.04 LTS](https://downloads.datastax.com/cpp-driver//ubuntu/18.04/cassandra)
* [Windows](https://downloads.datastax.com/cpp-driver//windows/cassandra)

If you are on MacOS, once you have the source code and nescesaary dependencies, you can build the driver with CMake:
```bash
mkdir build
pushd build
cmake ..
make
make install
popd
```
For more help with getting dependencies and building the driver from source code on different operating systems, check out the documentation on [building the driver]https://docs.datastax.com/en/developer/cpp-driver/latest/topics/building/). 

Once you have completed filling in the code blocks (with the driver installed on Linux/MacOS) you also have the option of generating the quickstart executable with CMake, and the CMake configuration file provided. Follow these steps:
* Clone quickstart-c repository
* `cd quickstart-c`
* `mkdir build`
* `cd build`
* `cmake ..`
* `make`

## Connect to your cluster

All of our code is contained in the `quickstart.c` file. 
A new  `CassCluster` object will need to be created and configured to connect a session to our Cassandra instance. The minimal configuration needed to connect is a list of hosts. If you are not using `localhost` for your instance, provide the ip address of of your node in the main() function.
```c
// TO DO: Fill in your own host 
int main(int argc, char* argv[]) {
  CassCluster* cluster = NULL;
  CassSession* session = cass_session_new();
  char* hosts = "127.0.0.1";
  char* keyspace = "demo";
```

## CRUD Operations
Fill the code in the functions that will add a user, get a user, update a user and delete a user from the table with the driver.

### INSERT a user
```javascript
CassError insert_user(CassSession* session, const Users* users) {
  CassError rc = CASS_OK;
  CassStatement* statement = NULL;
  CassFuture* future = NULL;
  
  // TO DO: execute a simple statement that inserts one user into the table
  const char* query =
      "INSERT INTO users (lastname, age, city, email, firstname) VALUES (?, ?, ?, ?, ?)";

  statement = cass_statement_new(query, 5);

  cass_statement_bind_string(statement, 0, users->lastname);
  cass_statement_bind_int32(statement, 1, users->age);
  cass_statement_bind_string(statement, 2, users->city);
  cass_statement_bind_string(statement, 3, users->email);
  cass_statement_bind_string(statement, 4, users->firstname);

  future = cass_session_execute(session, statement);
  cass_future_wait(future);

  rc = cass_future_error_code(future);
  if (rc != CASS_OK) {
    print_error(future);
  }

  cass_future_free(future);
  cass_statement_free(statement);

  return rc;
}
```
### SELECT a user
```c
CassError select_user(CassSession* session, const char* lastname) {
  CassError rc = CASS_OK;
  CassStatement* statement = NULL;
  CassFuture* future = NULL;
  
  // TO DO: execute a simple statement that retrieves one user from the table
  const char* query = "SELECT * FROM users WHERE lastname=?";

  statement = cass_statement_new(query, 1);

  cass_statement_bind_string(statement, 0, lastname);

  future = cass_session_execute(session, statement);
  cass_future_wait(future);

  rc = cass_future_error_code(future);
  if (rc != CASS_OK) {
    print_error(future);
  } else {
    const CassResult* result = cass_future_get_result(future);
    const CassRow* row = cass_result_first_row(result);

    if (row) {
      const char *firstname = NULL;
      size_t firstname_length = 0;
      cass_int32_t age = 0;

      //TO DO: print firstname and age of user
      cass_value_get_string(cass_row_get_column_by_name(row, "firstname"), &firstname,
                            &firstname_length);
      cass_value_get_int32(cass_row_get_column_by_name(row, "age"), &age);

      printf("firstname: '%.*s' age: %d\n", (int)firstname_length, firstname, age);
    }

    cass_result_free(result);
  }

  cass_statement_free(statement);
  cass_future_free(future);

  return rc;
}
```

### UPDATE a user's age
```c
CassError update_user_age(CassSession* session, const char* lastname, cass_int32_t age) {
  CassError rc = CASS_OK;
  CassStatement* statement = NULL;
  CassFuture* future = NULL;
  
  // TO DO: execute a simple statement that updates the age of one user
  const char* query = "UPDATE users SET age =? WHERE lastname =?";

  statement = cass_statement_new(query, 2);

  cass_statement_bind_int32(statement, 0, age);
  cass_statement_bind_string(statement, 1, lastname);

  future = cass_session_execute(session, statement);
  cass_future_wait(future);

  rc = cass_future_error_code(future);
  if (rc != CASS_OK) {
    print_error(future);
  }

  cass_future_free(future);
  cass_statement_free(statement);

  return rc;
}

```   

### DELETE a user
```c
CassError delete_user(CassSession* session, const char* lastname) {
  CassError rc = CASS_OK;
  CassStatement* statement = NULL;
  CassFuture* future = NULL;
  const char* query = "DELETE FROM users WHERE lastname=?";

  statement = cass_statement_new(query, 1);

  cass_statement_bind_string(statement, 0, lastname);

  future = cass_session_execute(session, statement);
  cass_future_wait(future);

  rc = cass_future_error_code(future);
  if (rc != CASS_OK) {
    print_error(future);
  }

  cass_future_free(future);
  cass_statement_free(statement);

  return rc;
}
```
 ## License
Copyright 2019 Rebecca Mills

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.   


