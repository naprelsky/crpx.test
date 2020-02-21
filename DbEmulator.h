#pragma once
#include "Service.h"

template<class T>
class Table {
	using container = std::map<int, T>;
	using iterator = typename container::iterator;
	using const_iterator = typename container::const_iterator;

public:
	iterator begin() { return rows.begin(); }
	iterator end() { return rows.end(); }
	const_iterator begin() const { return rows.begin(); }
	const_iterator end() const { return rows.end(); }

	void addRow(int id, T data) {
		rows.emplace(id, data);
	}

	T getRow(int id) {
		return rows.at(id);
	}

	int rowsCount() {
		return rows.size();
	}


private:
	std::map<int, T> rows;
};

enum PERSON_TYPE {
	LEGAL = 1,		// ёридическое лицо
	INDIVIDUAL = 2, // ‘изическое лицо
};

struct Person
{
	int ID;
	int Type;
	std::string Name;
};

struct OwnershipStake
{
	int ID;
	int CompanyID;
	int CompanyType;
	int OwnerID;
	int OwnerType;
	float Share;
};

struct SingleExecutive
{
	int ID;
	int CompanyID;
	int CompanyType;
	int OwnerID;
	int OwnerType;
};

struct PowerToGiveDirections
{
	int ID;
	int CompanyID;
	int CompanyType;
	int OwnerID;
	int OwnerType;
};

struct ReportItem
{
	std::string сompanyName;
	std::string relations;
	std::string basis;
};

class DbEmulator
{
public:
	DbEmulator();

	StringsVector getAllPersonsName();
	std::vector<ReportItem> getReport(const std::string& companyName);

private:
	Table<Person> legalPersons;
	Table<Person> individuals;
	Table<OwnershipStake> ownershipStake;
	Table<SingleExecutive> singleExecutive;
	Table<PowerToGiveDirections> powerToGiveDirections;

	void loadData();
};

