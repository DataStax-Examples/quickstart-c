# DataStax C/C++ Driver for Apache Cassandra Quickstart

A basic demo CRUD application using the DataStax C/C++ Driver for Apache Cassandra. 
The intent is to help users get up and running quickly with the driver. 
If you are having trouble, the complete code solution for `quickstart.c` can be found [here](https://gist.github.com/beccam/bcb2fdb81db931abb0d175ba0356c1fc).

## Prerequisites
  * A running instance of [Apache Cassandra®](http://cassandra.apache.org/download/) 1.2+

  
## Connect to your cluster

All of our code is contained in the `quickstart.c` file. 
A new `Cluster` is created through the `create_cluster()` function, and a new session through the `connect_session()` function. Through the Session 
we connect to our Cassandra cluster. If you are not using `localhost` for your instance, provide the ip address of of your node in the main() function.
```c
// TO DO: Fill in your own host 
int main(int argc, char* argv[]) {
  CassCluster* cluster = NULL;
  CassSession* session = cass_session_new();
  char* hosts = "127.0.0.1";
```

## CRUD Operations
Fill the code in the functions that will add a user, get a user, update a user and delete a user from the table with the driver.

### INSERT a user
```javascript
// TO DO: execute a simple statement that inserts one user into the table
CassError insert_user(CassSession* session, const Users* users) {
  CassError rc = CASS_OK;
  CassStatement* statement = NULL;
  CassFuture* future = NULL;
  const char* query =
      "INSERT INTO demo.users (lastname, age, city, email, firstname) VALUES (?, ?, ?, ?, ?)";

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
  const char* query = "SELECT * FROM demo.users WHERE lastname=?";

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

      cass_value_get_string(cass_row_get_column_by_name(row, "firstname"), &firstname,
                            &firstname_length);
      cass_value_get_int32(cass_row_get_column_by_name(row, "age"), &age);

      printf("firstname: '%.*s' age: %d\n", (int)firstname_length,
             firstname, age);
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
// TO DO: execute a simple statement that updates the age of one user
CassError update_user_age(CassSession* session, const char* lastname, cass_int32_t age) {
  CassError rc = CASS_OK;
  CassStatement* statement = NULL;
  CassFuture* future = NULL;
  const char* query = "UPDATE demo.users SET age =?  WHERE lastname =? ";

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
  const char* query = "DELETE FROM demo.users WHERE lastname=?";

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


