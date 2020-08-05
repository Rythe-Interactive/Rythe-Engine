#pragma once
#include <core/core.hpp>

class TestModule : public args::Module
{
public:
	virtual args::priority_type priority() override;
	virtual void init() override;
};

