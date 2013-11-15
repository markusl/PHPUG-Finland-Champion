#include <CppUnitTest.h>
#include "..\Restaurants\Restaurants.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft
{
    namespace VisualStudio
    {
        namespace CppUnitTestFramework
        {
            template<> static std::wstring ToString<OpeningHours>(const OpeningHours& t) { RETURN_WIDE_STRING(t.Hours); }
        }
    }
}

namespace RestaurantTests
{
TEST_CLASS(RestaurantTests) { public:

    TEST_METHOD(Test_count_hours_single_timespan)
    {
        Assert::AreEqual(count_hours_single_timespan("Ma-Pe 10:00-16:00"), size_t(5 * 6));
        Assert::AreEqual(count_hours_single_timespan("To 09:00-16:30"), size_t(7));
        Assert::AreEqual(count_hours_single_timespan("Pe 09:30-16:00"), size_t(6));
        Assert::AreEqual(count_hours_single_timespan("Ke-Pe 10:00-16:00"), size_t(3 * 6));
        Assert::AreEqual(count_hours_single_timespan("Ma-Pe 10:00-12:00 ja 12:30-14:30"), size_t(5 * 2 + 5 * 2));
    }

    TEST_METHOD(Test_restaurants_sorted)
    {
        const auto Restaurants = restaurants_sorted({
            { "512714", "KEIKYÄN RAVINTOLA", " 32740", " SASTAMALA", " Ma-Pe 10:00-16:30", " 61.28464759999999", " 22.6969689" },
            { "513127", "KEMINMAAN RAVINTOLA", " 94400", " KEMINMAA", " Ma-Pe 10:00-18:00", " 65.8010911", " 24.5473615" },
            { "513100", "KEMIN RAVINTOLA", " 94100", " KEMI", " Ma-Pe 10:00-16:30", " 65.7367598", " 24.567006" },
            { "506002", "HUMPPILAN RAVINTOLA", "31640", "HUMPPILA", "Ma-To 09:00-16:00, Pe 09:00-16:30", "60.92403669999999", "23.3690638" },
        });
        Assert::AreEqual(Restaurants.size(), size_t(4));
        Assert::AreEqual((OpeningHours{ "KEMINMAAN RAVINTOLA", 40 }), Restaurants[0]);
        Assert::AreEqual((OpeningHours{ "HUMPPILAN RAVINTOLA", 35 }), Restaurants[1]);
    }

};
}
