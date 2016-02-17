#include "UnitTest.h"
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <set>
#include <cstring>
#include <cstdlib>

int verbose = 0;

const char * const SEP = "::";

class UnitTestSystem
{
private:
	UnitTestSystem(): tests_(0), assertions_(0) { }
public:
	static UnitTestSystem& Instance()
	{
		static UnitTestSystem unitTestSystem;
		return unitTestSystem;
	}

	void Register(const std::string& pkg, const std::string& name, UnitTest::Proc proc)
	{
		cases_.push_back(std::make_pair(pkg + SEP + name, proc));
	}

	int Run(int argc, char *const argv[]);

	void IncreaseAssertion() { ++assertions_; }
private:
	std::vector<std::pair<std::string, UnitTest::Proc> > cases_;
	size_t tests_;
	size_t assertions_;
private:
	UnitTestSystem(const UnitTestSystem&);  // disable copy
	void operator = (const UnitTestSystem&);
};

UnitTest::UnitTest(const char* pkg, const char* name, Proc proc)
{
	UnitTestSystem::Instance().Register(pkg, name, proc);
}

void UnitTest::Test(bool res, const char* file, int line,
		const char* func, const char* expr, const char* msg)
{
	std::cout << '.' << std::flush;
	if (!res) {
		std::cout << " failed!\n" << file
#if defined(_MSC_VER)
			<< '(' << line << "): "
#else
			<< ':' << line << ": "
#endif
			<< func << ": Unit assert of '" << expr << "' failed!" << msg << std::endl;
		exit(1);
	}
	UnitTestSystem::Instance().IncreaseAssertion();
}

int UnitTestSystem::Run(int argc, char *const argv[])
{
	std::set<std::string> args;
	for (int i = 1; i < argc; ++i) {
		if (argv[i][0] == '-') {
			if (strcmp(argv[i], "-v") == 0) {
				++verbose;
			}
		} else {
			args.insert(argv[i]);
		}
	}

	for (size_t i = 0; i < cases_.size(); ++i) {
		const std::string& fullname = cases_[i].first;
		std::string pkg;
		std::string name;
		size_t pos = fullname.find(SEP);
		if (pos != std::string::npos) {
			pkg = fullname.substr(0, pos);
			name = fullname.substr(pos + 2);
		} else {
			pkg = "";
			name = fullname;
		}
		std::cout << " > unit test: '" << fullname << "' " << std::flush;
		if (args.empty() || args.find(pkg) != args.end() ||
				args.find(fullname) != args.end()) {
			(cases_[i].second)();
		}
		++tests_;

		std::cout << " passed!" << std::endl;
	}
	std::cout << "All " << tests_ << " unit tests (total "
			<< assertions_ << " assertions) passed!\n";
	return 0;
}

int main(int argc, char * const argv[])
{
	return UnitTestSystem::Instance().Run(argc, argv);
}
