#include <storages/postgres/detail/connection.hpp>
#include <storages/postgres/tests/util_pgtest.hpp>

namespace pg = storages::postgres;
namespace io = pg::io;

namespace static_test {

using namespace io::traits;

static_assert(kHasFormatter<std::string>);
static_assert(kHasParser<std::string>);

static_assert(kIsMappedToPg<std::string>);

static_assert(kHasFormatter<const char*>);

static_assert(kIsMappedToPg<const char*>);

static_assert(kHasFormatter<char[5]>);

static_assert(kIsMappedToPg<char[5]>);

static_assert(kHasFormatter<char>);
static_assert(kHasParser<char>);

static_assert(kIsMappedToPg<char>);

}  // namespace static_test

namespace {

TEST(PostgreIO, StringParserRegistry) {
  EXPECT_TRUE(io::HasParser(io::PredefinedOids::kChar));
  EXPECT_TRUE(io::HasParser(io::PredefinedOids::kText));
  EXPECT_TRUE(io::HasParser(io::PredefinedOids::kBpchar));
  EXPECT_TRUE(io::HasParser(io::PredefinedOids::kVarchar));
}

POSTGRE_TEST_P(StringRoundtrip) {
  ASSERT_TRUE(conn.get()) << "Expected non-empty connection pointer";
  pg::ResultSet res{nullptr};

  std::string unicode_str{"юникод µ𝞪∑∆ƒæ©⩜"};
  EXPECT_NO_THROW(res = conn->Execute("select $1", unicode_str));
  EXPECT_EQ(unicode_str, res[0][0].As<std::string>()) << "Text reply";

  EXPECT_NO_THROW(res = conn->ExperimentalExecute("select $1", unicode_str));
  EXPECT_EQ(unicode_str, res[0][0].As<std::string>()) << "Binary reply";
  auto str_res = res.AsSetOf<std::string>();
  EXPECT_EQ(unicode_str, str_res[0]);

  EXPECT_NO_THROW(res = conn->Execute("select $1", std::string{}));
  EXPECT_EQ(std::string{}, res[0][0].As<std::string>()) << "Empty string";
}

}  // namespace
