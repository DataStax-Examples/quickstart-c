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

    //TO DO: execute a simple statement that inserts one user into the table

}

CassError select_user(CassSession* session, const char* lastname) {

   //TO DO: execute SimpleStatement that retrieves one user from the table

   //TO DO: print firstname and age of user

}

CassError update_user_age(CassSession* session, const char* lastname, cass_int32_t age) {

    // TO DO: execute a simple statement that retrieves one user from the table

}

CassError delete_user(CassSession* session, const char* lastname) {

    // TO DO: execute a simple statement that updates the age of one user

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
