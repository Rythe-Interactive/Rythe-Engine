#pragma once
#include <core/core.hpp>

class TestModule : public args::core::Module
{
public:
	virtual args::core::priority_type priority() override;
	virtual void init() override;
};

