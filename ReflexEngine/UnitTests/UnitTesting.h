#pragma once

#include <string.h>
#include <functional>

class UnitTesting
{
public:
	UnitTesting() { }

	void Run()
	{
		unsigned titleIdx = 0;

		OnMessage( Stream( "Unit Testing Starting (" << tests.size() << " test, " << titles.size() << " sections)" ) );
		OnMessage( titles.front() );

		for( size_t idx = 0; idx < tests.size(); ++idx )
		{
			const auto& test = tests[idx];

			if( test.titleIdx > titleIdx )
			{
				OnMessage( titles[titleIdx] );
				++titleIdx;
			}

			OnMessage( Stream( "[" << ( idx + 1 ) << "/" << tests.size() << "] " << test.name ) );
			if( test.func() )
				OnSuccess( test );
			else
				OnFail( test );
		}
	}

protected:
	struct Test
	{
		std::function< bool() > func;
		size_t titleIdx = 0;
		std::string messageSuccess;
		std::string messageFail;
		std::string name;
	};

	virtual void OnMessage( const std::string& message ) const
	{
		std::cout << message << "\n";
	}

	virtual void OnSuccess( const Test& test ) const
	{
		std::cout << "\t- Success" << test.messageSuccess << "\n";
	}

	virtual void OnFail( const Test& test ) const
	{
		std::cout << "\t- Fail" << test.messageFail << "\n";
	}

	void RegisterSection( const std::string& title )
	{
		titles.push_back( std::move( title ) );
	}

	template< typename Bind >
	void RegisterTest( const Bind b, const bool expectedResult = true, const std::string& messageSuccess = "", const std::string& messageFail = "", const std::string& name = "" )
	{
		if( titles.empty() )
			titles.emplace_back();

		Test test;
		test.titleIdx = titles.size() - 1;
		test.func = [=](){ return b() == expectedResult; };
		test.messageSuccess = std::move( messageSuccess );
		test.messageFail = std::move( messageFail );
		test.name = std::move( name );
		tests.push_back( std::move( test ) );
	}

private:
	std::vector< std::string > titles;
	std::vector< Test > tests;
};