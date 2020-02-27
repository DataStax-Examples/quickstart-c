#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cassandra.h>

struct Users_ {
  const char* lastname;
  cass_int32_t age;
  const char* city;
  const char* email;
  const char* firstname;
};

typedef struct Users_ Users;

void print_error(CassFuture* future) {
  const char* message;
  size_t message_length;
  cass_future_error_message(future, &message, &message_length);
  fprintf(stderr, "Error: %.*s\n", (int)message_length, message);
}

CassCluster* create_cluster(const char* hosts) {
  CassCluster* cluster = cass_cluster_new();
  cass_cluster_set_contact_points(cluster, hosts);
  return cluster;
}

CassError connect_session(CassSession* session, const CassCluster* cluster, const char* keyspace) {
  CassError rc = CASS_OK;
  CassFuture* future = cass_session_connect_keyspace(session, cluster, keyspace);

  cass_future_wait(future);
  rc = cass_future_error_code(future);
  if (rc != CASS_OK) {
    print_error(future);
  }
  cass_future_free(future);

  return rc;
}

CassError insert_user(CassSession* session, const Users* users) {
  CassError rc = CASS_OK;
  CassStatement* statement = NULL;
  CassFuture* future = NULL;
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

CassError select_user(CassSession* session, const char* lastname) {
  CassError rc = CASS_OK;
  CassStatement* statement = NULL;
  CassFuture* future = NULL;
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

CassError update_user_age(CassSession* session, const char* lastname, cass_int32_t age) {
  CassError rc = CASS_OK;
  CassStatement* statement = NULL;
  CassFuture* future = NULL;
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

int main(int argc, char* argv[]) {
  CassCluster* cluster = NULL;
  CassSession* session = cass_session_new();
  char* hosts = "127.0.0.1";
  char* keyspace = "demo";

  Users input = { "Jones", 35, "Austin", "bob@example.com", "Bob" };

  if (argc > 1) {
    hosts = argv[1];
  }
  cluster = create_cluster(hosts);

  if (connect_session(session, cluster, keyspace) != CASS_OK) {
    cass_cluster_free(cluster);
    cass_session_free(session);
    return -1;
  }

  insert_user(session, &input);
  select_user(session, "Jones");
  update_user_age(session, "Jones", 36);
  select_user(session, "Jones");
  delete_user(session, "Jones");

  cass_cluster_free(cluster);
  cass_session_free(session);

  return 0;
}
