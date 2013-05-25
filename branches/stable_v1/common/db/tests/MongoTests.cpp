#include <gtest/gtest.h>

#include <mongo/client/dbclient.h>
#include <mongo/bson/bsonelement.h>

#include <vector>
#include <memory>

#include <boost/assign/list_of.hpp>

TEST(Mongo, Connect)
{
    mongo::DBClientConnection c;
    c.connect("localhost");
}

void TestOperations()
{
    mongo::DBClientConnection c;
    c.connect("localhost");

    std::vector<bson::bo> players;
    {
        const std::vector<bson::bo> actions = boost::assign::list_of
            (bson::bob().append("id", 0).append("amount", 0).obj())
            (bson::bob().append("id", 1).append("amount", 0).obj())
            (bson::bob().append("id", 1).append("amount", 0).obj())
            (bson::bob().append("id", 2).append("amount", 100).obj());

        const std::vector<bson::bo> streets = boost::assign::list_of
            (bson::bob().append("actions", actions).obj())
            (bson::bob().append("actions", actions).obj());

        bson::bob builder;
        builder
            .append("name", "CLRN")
            .append("stack", 1500)
            .append("streets", streets);

        players.push_back(builder.obj());
    }

    {
        const std::vector<bson::bo> actions = boost::assign::list_of
            (bson::bob().append("id", 1).append("amount", 100).obj())
            (bson::bob().append("id", 2).append("amount", 200).obj())
            (bson::bob().append("id", 3).append("amount", 400).obj())
            (bson::bob().append("id", 4).append("amount", 800).obj());

        const std::vector<bson::bo> streets = boost::assign::list_of
            (bson::bob().append("actions", actions).obj())
            (bson::bob().append("actions", actions).obj());

        bson::bob builder;
        builder
            .append("name", "bot")
            .append("stack", 100)
            .append("streets", streets);

        players.push_back(builder.obj());
    }

    //c.insert("test.players", players);

    {
        const std::auto_ptr<mongo::DBClientCursor> cursor = c.query("test.players", bson::bo());
        while (cursor->more()) 
        {
            const bson::bo p = cursor->next();
            std::cout << p["name"].str() << std::endl;
        }
    }

    {
        const std::vector<bson::bo> actions = boost::assign::list_of
            (bson::bob().append("id", 1).append("amount", 0).obj())
            (bson::bob().append("id", 2).append("amount", 1).obj());

        static const bson::bo returnProjection = bson::bob().append("players.$", 1).obj();


        mongo::Query query =             
        BSON("players" << BSON("$all" 
                                    << BSON_ARRAY(
                                            BSON("$elemMatch" 
                                                << BSON("name" << "CLRN"))
                                            << BSON("$elemMatch" 
                                                << BSON("name" << "2L84H8"))
                                        )));
        query.sort("_id", 0);

        const std::auto_ptr<mongo::DBClientCursor> cursor = c.query
        (
            "stat.games", 
            query,
            20, 
            0, 
            &returnProjection
        );

        while (cursor->more()) 
        {
            const bson::bo p = cursor->next();

            const std::string result = p.toString();

            const std::vector<bson::be> cards = p.getFieldDotted("players.0.cards").Array();

//             std::vector<bson::be> cardsElems;
//             cards.elems(cardsElems);


            std::cout << p["_id"].str() << std::endl;
        }
    } 
}

TEST(Mongo, Operations)
{
    TestOperations();
}