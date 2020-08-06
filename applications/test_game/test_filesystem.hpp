#pragma once
#include <core/filesystem/filesystem.hpp>

#include <iostream>

namespace fs = args::core::filesystem;

class mock_resolver : public fs::filesystem_resolver
{
public:
    [[nodiscard]] bool is_file() const noexcept override
    {
        if(get_target() == "test.txt") return true;
        return false;
    }
    [[nodiscard]] bool is_directory() const noexcept override { return false; }
    [[nodiscard]] bool is_valid() const noexcept override
    {
        if(get_target() == "test.txt") return true;
        return false;
    }
    [[nodiscard]] bool writeable() const noexcept override { return false; }
    [[nodiscard]] bool readable() const noexcept override {
        if(get_target() == "test.txt") return true;
        return false;
    }
    [[nodiscard]] bool creatable() const noexcept override { return false; }
    [[nodiscard]] bool exists() const noexcept override {
        if(get_target() == "test.txt") return true;
        return false;
    }
    [[nodiscard]] std::set<std::string> ls() const noexcept override { return {"test.txt"}; }

    args::core::common::result<args::core::filesystem::basic_resource, args::core::fs_error>
    get(args::core::interfaces::implement_signal_t) const noexcept override
    {
        using args::core::common::Err, args::core::common::Ok;
        if(get_target() == "test.txt") return Ok(args::core::filesystem::basic_resource("Hello Filesystem World!"));
        return Err(args_fs_error("this mock interface does not support file access"));
    }

    bool set(args::core::interfaces::implement_signal_t, const args::core::filesystem::basic_resource& res) override
    {
        (void) res;
        return false;
    }
    void erase(args::core::interfaces::implement_signal_t) const noexcept override
    {
    }



    //surrogate ctor
    [[nodiscard]] filesystem_resolver* make() override
    {
        return new mock_resolver;
    }
};


inline void test_filesystem()
{

    fs::provider_registry::domain_create_resolver<mock_resolver>("only_test_valid://");
    auto traits = fs::view("only_test_valid://test.txt").file_info();
    if(traits != fs::invalid_file_t)
    {
        std::cout << "[arm-fs] -> mock provider was successfully queried" << std::endl;
        std::cout << "[arm-fs] -> " <<static_cast<filesystem::basic_resource>(fs::view("only_test_valid://test.txt").get()).to_string() << std::endl;
    }else
    {
        __debugbreak();
    }
}

