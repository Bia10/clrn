#include <gtest/gtest.h>

#include <mongo/client/dbclient.h>

TEST(Mongo, Connect)
{
    mongo::DBClientConnection c;
    c.connect("localhost");
}