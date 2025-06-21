#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <string>

using ::testing::Return;
using ::testing::_;

class IDBConnection {
public:
    virtual ~IDBConnection() = default;
    virtual bool open() = 0;
    virtual void close() = 0;
    virtual std::string execQuery(const std::string& query) = 0;
};

class MockDBConnection : public IDBConnection {
public:
    MOCK_METHOD(bool, open, (), (override));
    MOCK_METHOD(void, close, (), (override));
    MOCK_METHOD(std::string, execQuery, (const std::string& query), (override));
};

class ClassThatUsesDB {
public:
    explicit ClassThatUsesDB(std::shared_ptr<IDBConnection> dbConn)
        : dbConnection(std::move(dbConn)) {}

    bool openConnection() {
        return dbConnection->open();
    }

    void closeConnection() {
        dbConnection->close();
    }

    std::string useConnection(const std::string& query) {
        if (!dbConnection) return "No connection";
        return dbConnection->execQuery(query);
    }

private:
    std::shared_ptr<IDBConnection> dbConnection;
};

TEST(ClassThatUsesDBTest, OpenConnection_Success) {
    auto mockDb = std::make_shared<MockDBConnection>();
    EXPECT_CALL(*mockDb, open()).WillOnce(Return(true));

    ClassThatUsesDB testObj(mockDb);
    EXPECT_TRUE(testObj.openConnection());
}

TEST(ClassThatUsesDBTest, OpenConnection_Failure) {
    auto mockDb = std::make_shared<MockDBConnection>();
    EXPECT_CALL(*mockDb, open()).WillOnce(Return(false));

    ClassThatUsesDB testObj(mockDb);
    EXPECT_FALSE(testObj.openConnection());
}

TEST(ClassThatUsesDBTest, CloseConnection_CallsClose) {
    auto mockDb = std::make_shared<MockDBConnection>();
    EXPECT_CALL(*mockDb, close()).Times(1);

    ClassThatUsesDB testObj(mockDb);
    testObj.closeConnection();
}

TEST(ClassThatUsesDBTest, UseConnection_ReturnsExpectedResult) {
    auto mockDb = std::make_shared<MockDBConnection>();
    std::string query = "SELECT * FROM users";
    std::string expectedResult = "user data";

    EXPECT_CALL(*mockDb, execQuery(query)).WillOnce(Return(expectedResult));

    ClassThatUsesDB testObj(mockDb);
    std::string result = testObj.useConnection(query);
    EXPECT_EQ(result, expectedResult);
}

TEST(ClassThatUsesDBTest, UseConnection_EmptyQuery) {
    auto mockDb = std::make_shared<MockDBConnection>();
    std::string query = "";
    std::string expectedResult = "";

    EXPECT_CALL(*mockDb, execQuery(query)).WillOnce(Return(expectedResult));

    ClassThatUsesDB testObj(mockDb);
    EXPECT_EQ(testObj.useConnection(query), expectedResult);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
