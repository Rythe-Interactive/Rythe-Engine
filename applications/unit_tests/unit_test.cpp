#include "unit_test.hpp"


namespace legion
{
    std::string* test_data::lastDomain = nullptr;
    cstring test_data::lastSubDomain = nullptr;
    std::vector<cstring> test_data::subdomainnames;
    std::unordered_map<cstring, std::pair<size_type, long double>> test_data::subdomaintimes;

    CheckFunc test_data::Check;
    SubDomainFunc test_data::StartSubDomain;
}
